import zoom_meeting_sdk as zoom
from datetime import datetime, timedelta
import jwt
from dotenv import load_dotenv
import os
import time
import random
import urllib.parse

import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib

import signal
import sys
from multiprocessing import Process

import numpy as np
from python_speech_features import mfcc
from fastdtw import fastdtw
from scipy.spatial.distance import cosine

def generate_jwt(client_id, client_secret):
    iat = datetime.utcnow()
    exp = iat + timedelta(hours=24)
    
    payload = {
        "iat": iat,
        "exp": exp,
        "appKey": client_id,
        "tokenExp": int(exp.timestamp())
    }
    
    token = jwt.encode(payload, client_secret, algorithm="HS256")
    return token

def get_random_meeting():
    urls = os.environ.get('MEETING_URLS')
    url_list = urls.split('\n')

    # Choose a random URL from the list
    chosen_url = random.choice(url_list)

    # Parse the URL
    parsed_url = urllib.parse.urlparse(chosen_url)

    # Extract the path and query components
    path = parsed_url.path
    query = urllib.parse.parse_qs(parsed_url.query)

    # Extract meeting ID from the path
    meeting_id = path.split('/')[-1]

    password = query.get('pwd', [None])[0]

    return int(meeting_id), password

class TestBot():
    def __init__(self):
        self.main_loop = None
        self.audio_helper = None
    def init(self):
        init_param = zoom.InitParam()

        init_param.strWebDomain = "https://zoom.us"
        init_param.strSupportUrl = "https://zoom.us"
        init_param.enableGenerateDump = True
        init_param.emLanguageID = zoom.SDK_LANGUAGE_ID.LANGUAGE_English
        init_param.enableLogByDefault = True

        init_sdk_result = zoom.InitSDK(init_param)
        if init_sdk_result != zoom.SDKERR_SUCCESS:
            raise Exception('InitSDK failed')

        self.meeting_service_event = zoom.MeetingServiceEventCallbacks(onMeetingStatusChangedCallback=self.meeting_status_changed)
        self.meeting_service = zoom.CreateMeetingService()  
        self.meeting_service.SetEvent(self.meeting_service_event)

        self.auth_event = zoom.AuthServiceEventCallbacks(onAuthenticationReturnCallback=self.auth_return)
        self.auth_service = zoom.CreateAuthService()
        self.auth_service.SetEvent(self.auth_event)

        # Use the auth service
        auth_context = zoom.AuthContext()
        auth_context.jwt_token = generate_jwt(os.environ.get('ZOOM_APP_CLIENT_ID'), os.environ.get('ZOOM_APP_CLIENT_SECRET'))
        result = self.auth_service.SDKAuth(auth_context)
        if result != zoom.SDKError.SDKERR_SUCCESS:
            raise Exception('SDKAuth failed!')
        
        self.setting_service = zoom.CreateSettingService()

    def meeting_status_changed(self, status, iResult):
        if status == zoom.MEETING_STATUS_INMEETING:
            print("joined meeting")

            self.after_join_action()

    def after_join_action(self):
        my_user_name = self.meeting_service.GetMeetingParticipantsController().GetMySelfUser().GetUserName()
        if my_user_name != "TestJoinBot":
            raise Exception("Failed to get username")

        with open("/tmp/test_passed", 'w') as f:
            f.write('test_passed')
        print("self.meeting_service after joined", self.meeting_service)
        GLib.timeout_add_seconds(4, self.exit_process)

    def exit_process(self):
        if self.main_loop:
            self.main_loop.quit()
        return False  # To stop the timeout from repeating

    def auth_return(self, result):
        if result == zoom.AUTHRET_SUCCESS:
            print("Auth completed successfully.")

            meeting_number, password = get_random_meeting()

            display_name = self.get_display_name()

            join_param = zoom.JoinParam()
            join_param.userType = zoom.SDKUserType.SDK_UT_WITHOUT_LOGIN

            param = join_param.param
            param.meetingNumber = meeting_number
            param.userName = display_name
            param.psw = password
            param.vanityID = ""
            param.customer_key = ""
            param.webinarToken = ""
            param.isVideoOff = False
            param.isAudioOff = False

            self.meeting_service.Join(join_param)

            self.audio_settings = self.setting_service.GetAudioSettings()
            self.audio_settings.EnableAutoJoinAudio(True)
            return

        raise Exception("Failed to authorize. result = ", result)

    def leave(self):
        if self.meeting_service is None:
            return
        
        if self.meeting_service.GetMeetingStatus() == zoom.MEETING_STATUS_IDLE:
            return
        
        self.meeting_service.Leave(zoom.LEAVE_MEETING)

    def cleanup(self):
        if self.meeting_service:
            print("DestroyMeetingService")
            zoom.DestroyMeetingService(self.meeting_service)
            print("EndDestroyMeetingService")
        if self.setting_service:
            zoom.DestroySettingService(self.setting_service)
            print("Destroyed Setting service")
        if self.auth_service:
            print("DestroyAuthService")
            zoom.DestroyAuthService(self.auth_service)
            print("EndDestroyAuthService")

        if self.audio_helper:
            audio_helper_unsubscribe_result = self.audio_helper.unSubscribe()
            print("audio_helper.unSubscribe() returned", audio_helper_unsubscribe_result)

        print("CleanUPSDK")
        zoom.CleanUPSDK()
        print("EndCleanUPSDK")

    def on_timeout(self):
        return True  # Keeping the timeout active
        
    def on_exit(self):
        print("Exiting...")
        self.leave()
        print("cleaning...")
        self.cleanup()
        
    def run(self):
        self.init()
        
        # Create a GLib main loop
        self.main_loop = GLib.MainLoop()
        print("self.main_loop", self.main_loop)
        # Add a timeout function that will be called every 100ms
        GLib.timeout_add(100, self.on_timeout)
        
        # Run the main loop
        try:
            self.main_loop.run()
        except KeyboardInterrupt:
            print("Interrupted by user, shutting down...")
        finally:
            self.main_loop.quit()

    def get_display_name(self):
        return "TestJoinBot"

class PlayAudioBot(TestBot):
    def __init__(self):
        super().__init__()
        self.virtual_audio_mic_event_passthrough = None
        self.audio_raw_data_sender = None
        self.audio_source = None

    def get_display_name(self):
        return "PlayAudioBot"
        
    def on_mic_initialize_callback(self, sender):
        print("on_mic_initialize_callback")
        self.audio_raw_data_sender = sender

    def on_mic_start_send_callback(self):
        print("start playing audio")
        with open('test_scripts/manual_rec-01.raw', 'rb') as pcm_file:
            chunk = pcm_file.read(1164160) #pcm_file.read(64000*10)
            self.audio_raw_data_sender.send(chunk, 32000, zoom.ZoomSDKAudioChannel_Mono)
    
    def after_join_action(self):
        print("start_raw_recording timeout sent to run in 1 second")
        GLib.timeout_add_seconds(5, self.start_raw_recording)

        print("leave meeting timeout sent to run in 60 seconds")
        GLib.timeout_add_seconds(60, self.exit_process)
        
    def start_raw_recording(self):
        self.audio_helper = zoom.GetAudioRawdataHelper()
        
        self.virtual_audio_mic_event_passthrough = zoom.ZoomSDKVirtualAudioMicEventCallbacks(
            onMicInitializeCallback=self.on_mic_initialize_callback,
            onMicStartSendCallback=self.on_mic_start_send_callback
        )
        self.audio_helper.setExternalAudioSource(self.virtual_audio_mic_event_passthrough)

class RecordAudioBot(TestBot):
    def __init__(self):
        super().__init__()
        self.virtual_audio_mic_event_passthrough = None
        self.audio_raw_data_sender = None
        self.audio_source = None
        self.meeting_reminder_event = None
        self.reminder_controller = None
        self.recording_ctrl = None
        self.recording_event = None
        self.use_raw_recording = True
        self.participants_ctrl = None
        self.my_participant_id = None
        
        # Add new buffer storage
        self.audio_buffers = {}  # Dictionary to store audio data per node_id
        self.audio_buffer_counters = {}  # Dictionary to store buffer counters per node_id
    def get_display_name(self):
        return "RecordAudioBot"
        
    def after_join_action(self):
        self.meeting_reminder_event = zoom.MeetingReminderEventCallbacks(onReminderNotifyCallback=self.on_reminder_notify)
        self.reminder_controller = self.meeting_service.GetMeetingReminderController()
        self.reminder_controller.SetEvent(self.meeting_reminder_event)

        if self.use_raw_recording:
            self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

            def on_recording_privilege_changed(can_rec):
                print("on_recording_privilege_changed called. can_record =", can_rec)
                if can_rec:
                    self.start_raw_recording()
                else:
                    self.stop_raw_recording()

            self.recording_event = zoom.MeetingRecordingCtrlEventCallbacks(onRecordPrivilegeChangedCallback=on_recording_privilege_changed)
            self.recording_ctrl.SetEvent(self.recording_event)

            self.start_raw_recording()

        # leave after 15 seconds
        GLib.timeout_add_seconds(20, self.exit_process)

        self.participants_ctrl = self.meeting_service.GetMeetingParticipantsController()
        self.my_participant_id = self.participants_ctrl.GetMySelfUser().GetUserID()
        
    def start_raw_recording(self):
        self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

        can_start_recording_result = self.recording_ctrl.CanStartRawRecording()
        if can_start_recording_result != zoom.SDKERR_SUCCESS:
            self.recording_ctrl.RequestLocalRecordingPrivilege()
            print("Requesting recording privilege.")
            return

        start_raw_recording_result = self.recording_ctrl.StartRawRecording()
        if start_raw_recording_result != zoom.SDKERR_SUCCESS:
            print("Start raw recording failed.")
            return

        self.audio_helper = zoom.GetAudioRawdataHelper()
        if self.audio_helper is None:
            print("audio_helper is None")
            return
        
        if self.audio_source is None:
            self.audio_source = zoom.ZoomSDKAudioRawDataDelegateCallbacks(onOneWayAudioRawDataReceivedCallback=self.on_one_way_audio_raw_data_received_callback)

        audio_helper_subscribe_result = self.audio_helper.subscribe(self.audio_source, False)
        print("audio_helper_subscribe_result =",audio_helper_subscribe_result)

    def on_one_way_audio_raw_data_received_callback(self, data, node_id):
       
        if node_id == self.my_participant_id:
            return
            
        # Accumulate audio data for this node_id
        if node_id not in self.audio_buffers:
            self.audio_buffers[node_id] = bytearray()
            self.audio_buffer_counters[node_id] = 0
        self.audio_buffers[node_id].extend(data.GetBuffer())
        self.audio_buffer_counters[node_id] += 1
        
        current_time_ns = time.perf_counter_ns()
        current_time_ms = current_time_ns / 1_000_000  # Convert nanoseconds to milliseconds
        print(f"Received audio at {current_time_ms:.3f} ms from node {node_id}")

    def on_reminder_notify(self, content, handler):
        if handler:
            handler.accept()

    def on_exit(self):
        print("RecordAudioBot on_exit")
        print("total processing time", self.audio_source.getTotalProcessingTimeMicroseconds())
        print("num on one way audio raw data received calls", self.audio_source.getNumOnOneWayAudioRawDataReceivedCalls())
        super().on_exit()
        
        try:
            # Write each recorded audio buffer to a file
            for node_id, recorded_audio in self.audio_buffers.items():
                output_filename = f'recorded_audio_node_{node_id}.raw'
                with open(output_filename, 'wb') as f:
                    f.write(recorded_audio)
                print(f"Wrote {len(recorded_audio)} bytes to {output_filename}")
            
            # Read original audio and convert to numpy array
            sr = 32000
            with open('test_scripts/manual_rec-01.raw', 'rb') as f:
                original_audio = f.read()
                original_array = np.frombuffer(original_audio, dtype=np.int16)
                mfcc1 = mfcc(original_array, samplerate=sr, nfft=2048)

            mfcc_distances = {}
            for node_id, recorded_audio in self.audio_buffers.items():
                print(f"Analyzing audio from node {node_id}")
                print(f"Original length: {len(original_audio)} bytes")
                print(f"Recorded length: {len(recorded_audio)} bytes")
                
                # Convert recorded audio to numpy array
                recorded_array = np.frombuffer(recorded_audio, dtype=np.int16)
                recorded_array = recorded_array[:len(original_array)]

                mfcc2 = mfcc(recorded_array, samplerate=sr, nfft=2048)
                mfcc_distance, _ = fastdtw(mfcc1, mfcc2, dist=cosine)

                print(f"MFCC distance for node {node_id}: {mfcc_distance:.4f}")
                mfcc_distances[node_id] = mfcc_distance

            print("mfcc_distances", mfcc_distances)
            print("average mfcc distance", np.mean(list(mfcc_distances.values())))
            print("max mfcc distance", max(mfcc_distances.values()))
            print("min mfcc distance", min(mfcc_distances.values()))
            print("audio_buffer_counters", self.audio_buffer_counters)

        except Exception as e:
            print(f"Error processing audio files: {e}")
            

class RecordAudioBotBuffered(TestBot):
    def __init__(self):
        super().__init__()
        self.virtual_audio_mic_event_passthrough = None
        self.audio_raw_data_sender = None
        self.audio_source = None
        self.meeting_reminder_event = None
        self.reminder_controller = None
        self.recording_ctrl = None
        self.recording_event = None
        self.use_raw_recording = True
        self.participants_ctrl = None
        self.my_participant_id = None
        
        # Add new buffer storage
        self.audio_buffers = {}  # Dictionary to store audio data per node_id
        
    def get_display_name(self):
        return "RecordAudioBotBuffered"
        
    def after_join_action(self):
        self.meeting_reminder_event = zoom.MeetingReminderEventCallbacks(onReminderNotifyCallback=self.on_reminder_notify)
        self.reminder_controller = self.meeting_service.GetMeetingReminderController()
        self.reminder_controller.SetEvent(self.meeting_reminder_event)

        if self.use_raw_recording:
            self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

            def on_recording_privilege_changed(can_rec):
                print("on_recording_privilege_changed called. can_record =", can_rec)
                if can_rec:
                    self.start_raw_recording()
                else:
                    self.stop_raw_recording()

            self.recording_event = zoom.MeetingRecordingCtrlEventCallbacks(onRecordPrivilegeChangedCallback=on_recording_privilege_changed)
            self.recording_ctrl.SetEvent(self.recording_event)

            self.start_raw_recording()

        # leave after 15 seconds
        GLib.timeout_add_seconds(20, self.exit_process)

        self.participants_ctrl = self.meeting_service.GetMeetingParticipantsController()
        self.my_participant_id = self.participants_ctrl.GetMySelfUser().GetUserID()
        
    def start_raw_recording(self):
        self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

        can_start_recording_result = self.recording_ctrl.CanStartRawRecording()
        if can_start_recording_result != zoom.SDKERR_SUCCESS:
            self.recording_ctrl.RequestLocalRecordingPrivilege()
            print("Requesting recording privilege.")
            return

        start_raw_recording_result = self.recording_ctrl.StartRawRecording()
        if start_raw_recording_result != zoom.SDKERR_SUCCESS:
            print("Start raw recording failed.")
            return

        self.audio_helper = zoom.GetAudioRawdataHelper()
        if self.audio_helper is None:
            print("audio_helper is None")
            return
        
        if self.audio_source is None:
            self.audio_source = zoom.ZoomSDKAudioRawDataDelegateBuffered()

        audio_helper_subscribe_result = self.audio_helper.subscribe(self.audio_source, False)
        print("audio_helper_subscribe_result =",audio_helper_subscribe_result)

    def on_reminder_notify(self, content, handler):
        if handler:
            handler.accept()

    def on_exit(self):
        active_nodes = self.audio_source.getActiveNodes()
        print("active_nodes", active_nodes)

        print("total processing time", self.audio_source.getTotalProcessingTimeMicroseconds())
        print("num on one way audio raw data received calls", self.audio_source.getNumOnOneWayAudioRawDataReceivedCalls())
        for node_id in active_nodes:
            if node_id not in self.audio_buffers:
                self.audio_buffers[node_id] = bytearray()
            self.audio_buffers[node_id].extend(self.audio_source.getBufferData(node_id))


        print("RecordAudioBot on_exit")
        super().on_exit()
        
        try:
            # Write each recorded audio buffer to a file
            for node_id, recorded_audio in self.audio_buffers.items():
                output_filename = f'recorded_audio_node_{node_id}.raw'
                with open(output_filename, 'wb') as f:
                    f.write(recorded_audio)
                print(f"Wrote {len(recorded_audio)} bytes to {output_filename}")
            
            # Read original audio and convert to numpy array
            sr = 32000
            with open('test_scripts/manual_rec-01.raw', 'rb') as f:
                original_audio = f.read()
                original_array = np.frombuffer(original_audio, dtype=np.int16)
                mfcc1 = mfcc(original_array, samplerate=sr, nfft=2048)

            mfcc_distances = {}
            for node_id, recorded_audio in self.audio_buffers.items():
                print(f"Analyzing audio from node {node_id}")
                print(f"Original length: {len(original_audio)} bytes")
                print(f"Recorded length: {len(recorded_audio)} bytes")
                
                # Convert recorded audio to numpy array
                recorded_array = np.frombuffer(recorded_audio, dtype=np.int16)
                recorded_array = recorded_array[:len(original_array)]

                mfcc2 = mfcc(recorded_array, samplerate=sr, nfft=2048)
                mfcc_distance, _ = fastdtw(mfcc1, mfcc2, dist=cosine)

                print(f"MFCC distance for node {node_id}: {mfcc_distance:.4f}")
                mfcc_distances[node_id] = mfcc_distance

            print("mfcc_distances", mfcc_distances)
            print("average mfcc distance", np.mean(list(mfcc_distances.values())))
            print("max mfcc distance", max(mfcc_distances.values()))
            print("min mfcc distance", min(mfcc_distances.values()))

        except Exception as e:
            print(f"Error processing audio files: {e}")

def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def main():
    load_dotenv()

    # Set up signal handlers
    signal.signal(signal.SIGINT, on_signal)
    signal.signal(signal.SIGTERM, on_signal)

    # Create and run multiple bots
    processes = []
    for i in range(2):
        # Pass bot_type as an argument, not a keyword argument
        p = Process(target=run_bot, args=(RecordAudioBot if i == 0 else PlayAudioBot,))
        p.daemon = True
        p.start()
        processes.append(p)

    try:
        for p in processes:
            p.join()
    except KeyboardInterrupt:
        for p in processes:
            p.terminate()
            p.join()

def run_bot(bot_type):
    bot = bot_type()
    import atexit
    atexit.register(bot.on_exit)
    bot.run()
    bot.on_exit()

if __name__ == "__main__":
    main()


# 10 ms of audio sent per callback call
# With 3 speakers:

# python:
# total processing time 458291 microseconds
# num on one way audio raw data received calls 3428
# c++:
# total processing time 89244 microseconds
# num on one way audio raw data received calls 3428

# With 1 speaker:
# python:
# total processing time 204227
# num on one way audio raw data received calls 1303
# total processing time 239766
# num on one way audio raw data received calls 1348
# total processing time 230297
# num on one way audio raw data received calls 1352