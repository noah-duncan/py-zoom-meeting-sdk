import zoom_meeting_sdk as zoom
import jwt
from deepgram_transcriber import DeepgramTranscriber
from video_input_manager import VideoInputManager
from streaming_uploader import StreamingUploader
from gstreamer_pipeline import GstreamerPipeline
from datetime import datetime, timedelta
import os

import cv2
import numpy as np
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib
from queue import Queue
import threading
import time

def save_yuv420_frame_as_png(frame_bytes, width, height, output_path):
    """
    Convert a raw YUV420/I420 frame to PNG and save it

    Args:
    frame_bytes: Raw frame data as bytes or bytearray
    width: Frame width in pixels
    height: Frame height in pixels 
    output_path: Path to save the PNG file
    """
    # Convert bytes to numpy array
    yuv_data = np.frombuffer(frame_bytes, dtype=np.uint8)

    # Reshape into I420 format with U/V planes
    yuv_frame = yuv_data.reshape((height * 3//2, width))

    # Convert from YUV420 to BGR
    bgr_frame = cv2.cvtColor(yuv_frame, cv2.COLOR_YUV2BGR_I420)

    # Save as PNG
    cv2.imwrite(output_path, bgr_frame)

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

def normalized_rms_audio(pcm_data: bytes, sample_width: int = 2) -> bool:
    """
    Determine if PCM audio data contains significant audio or is essentially silence.
    
    Args:
        pcm_data: Bytes object containing PCM audio data in linear16 format
        threshold: RMS amplitude threshold below which audio is considered silent (0.0 to 1.0)
        sample_width: Number of bytes per sample (2 for linear16)
        
    Returns:
        bool: True if the audio is essentially silence, False if it contains significant audio
    """
    if len(pcm_data) == 0:
        return True
        
    # Convert bytes to 16-bit integers
    import array
    samples = array.array('h')  # signed short integer array
    samples.frombytes(pcm_data)
    
    # Calculate RMS amplitude
    sum_squares = sum(sample * sample for sample in samples)
    rms = (sum_squares / len(samples)) ** 0.5
    
    # Normalize RMS to 0.0-1.0 range
    # For 16-bit audio, max value is 32767
    normalized_rms = rms / 32767.0
    
    return normalized_rms

class MeetingBot:
    def __init__(self):
        
        self.meeting_service = None
        self.setting_service = None
        self.auth_service = None

        self.auth_event = None
        self.recording_event = None
        self.meeting_service_event = None

        self.audio_source = None
        self.audio_helper = None

        self.audio_settings = None

        self.use_raw_recording = True

        self.reminder_controller = None

        self.recording_ctrl = None
        self.audio_ctrl = None
        self.audio_ctrl_event = None
        self.audio_raw_data_sender = None
        self.virtual_audio_mic_event_passthrough = None

        self.deepgram_transcriber = DeepgramTranscriber()

        self.my_participant_id = None
        self.other_participant_id = None
        self.active_speaker_id = None
        self.participants_ctrl = None
        self.meeting_reminder_event = None

        self.pipeline = GstreamerPipeline(on_new_sample_callback = self.on_new_sample_from_pipeline)
        self.video_input_manager = VideoInputManager(new_frame_callback=self.pipeline.on_new_video_frame, wants_any_frames_callback=self.pipeline.wants_any_video_frames)

        # Add new properties for video recording
        # self.pipeline = None
        # self.appsrc = None
        # self.recording_active = False
        
        # Initialize GStreamer
        # Gst.init(None)

        # Add new properties for audio recording
        # self.audio_appsrc = None
        # self.audio_recording_active = False

        # Add shared timestamp reference
        # self.start_time_ns = None  # Will be set on first frame/audio sample

        self.uploader = StreamingUploader(os.environ.get('AWS_RECORDING_STORAGE_BUCKET_NAME'), 'teststreamupload.mp4')
        self.uploader.start_upload()
        
        # Add counters for dropped buffers
        # self.queue_drops = {f'q{i}': 0 for i in range(1, 8)}
        # self.last_reported_drops = {f'q{i}': 0 for i in range(1, 8)}
    
    def on_new_sample_from_pipeline(self, data):
        self.uploader.upload_part(data)
    
    # def on_new_sample_from_appsink(self, sink):
    #     """Handle new samples from the appsink"""
    #     sample = sink.emit('pull-sample')
    #     if sample:
    #         buffer = sample.get_buffer()
    #         data = buffer.extract_dup(0, buffer.get_size())
    #         self.uploader.upload_part(data)
    #         return Gst.FlowReturn.OK
    #     return Gst.FlowReturn.ERROR

#     def setup_gstreamer_pipeline(self):
#         """Initialize GStreamer pipeline for combined MP4 recording with audio and video"""
#         self.start_time_ns = None
        
# # gst-launch-1.0 ximagesrc xid=$XID ! video/x-raw,framerate=30/1 ! queue ! videoconvert ! videorate ! queue ! x264enc ! queue ! avimux name=mux ! queue ! filesink location=out.avi //pulsesrc device=$DEV ! queue ! audioconvert ! queue ! lamemp3enc bitrate=192 ! queue ! mux.

#         BUCKET_NAME = os.environ.get('AWS_RECORDING_STORAGE_BUCKET_NAME')
#         OBJECT_KEY = 'teststreamupload.mp4'
#         AWS_ACCESS_KEY = os.environ.get('AWS_ACCESS_KEY_ID')
#         AWS_SECRET_KEY = os.environ.get('AWS_SECRET_ACCESS_KEY')

#         reduce_framerate_pipeline_str = (
#             'appsrc name=video_source do-timestamp=false stream-type=0 format=time ! '
#             'queue name=q1 ! '
#             'videoconvert ! '
#             'videorate ! video/x-raw,framerate=15/1 ! ' # Reduce framerate to 15fps
#             'queue name=q2 ! '
#             'x264enc tune=zerolatency speed-preset=ultrafast ! '
#             'queue name=q3 ! '
#             'mp4mux name=muxer ! queue name=q4 ! appsink name=sink emit-signals=true sync=false drop=false '
#             'appsrc name=audio_source do-timestamp=false stream-type=0 format=time ! '
#             'queue name=q5 ! '
#             'audioconvert ! '
#             'audiorate ! '
#             'audioresample ! audio/x-raw,rate=16000 ! '
#             'queue name=q6 ! '
#             'voaacenc bitrate=64000 ! '
#             'queue name=q7 ! '
#             'muxer. '
#         )

#         reduce_video_resolution_pipeline_str = (
#             'appsrc name=video_source do-timestamp=false stream-type=0 format=time ! '
#             'queue name=q1 ! '
#             'videoconvert ! '
#             'videoscale ! video/x-raw,width=320,height=180 ! '  # Downscale video
#             'videorate ! '
#             'queue name=q2 ! '
#             'x264enc tune=zerolatency speed-preset=ultrafast ! '
#             'queue name=q3 ! '
#             'mp4mux name=muxer ! queue name=q4 ! appsink name=sink emit-signals=true sync=false drop=false '
#             'appsrc name=audio_source do-timestamp=false stream-type=0 format=time ! '
#             'queue name=q5 ! '
#             'audioconvert ! '
#             'audiorate ! '
#             'queue name=q6 ! '
#             'voaacenc bitrate=128000 ! '
#             'queue name=q7 ! '
#             'muxer. '
#         )

#         pipeline_str = reduce_video_resolution_pipeline_str
        
#         self.pipeline = Gst.parse_launch(pipeline_str)
        
#         # Get both appsrc elements
#         self.appsrc = self.pipeline.get_by_name('video_source')
#         self.audio_appsrc = self.pipeline.get_by_name('audio_source')
        
#         # Configure video appsrc
#         video_caps = Gst.Caps.from_string('video/x-raw,format=BGR,width=640,height=360,framerate=30/1')
#         self.appsrc.set_property('caps', video_caps)
#         self.appsrc.set_property('format', Gst.Format.TIME)
#         self.appsrc.set_property('is-live', True)
#         self.appsrc.set_property('do-timestamp', False)
#         self.appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM
#         self.appsrc.set_property('block', True)  # This helps with synchronization

#         # Configure audio appsrc
#         audio_caps = Gst.Caps.from_string(
#             'audio/x-raw,format=S16LE,channels=1,rate=32000,layout=interleaved'
#         )
#         self.audio_appsrc.set_property('caps', audio_caps)
#         self.audio_appsrc.set_property('format', Gst.Format.TIME)
#         self.audio_appsrc.set_property('is-live', True)
#         self.audio_appsrc.set_property('do-timestamp', False)
#         self.audio_appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM
#         self.audio_appsrc.set_property('block', True)  # This helps with synchronization

#         # Set up bus
#         bus = self.pipeline.get_bus()
#         bus.add_signal_watch()
#         bus.connect('message', self.on_pipeline_message)

#         # Connect to the sink element
#         sink = self.pipeline.get_by_name('sink')
#         sink.connect("new-sample", self.on_new_sample_from_appsink)
        
#         # Start the pipeline
#         self.pipeline.set_state(Gst.State.PLAYING)

#         self.recording_active = True
#         self.audio_recording_active = True

#         # Start statistics monitoring
#         self.monitoring_active = True
#         GLib.timeout_add_seconds(15, self.monitor_pipeline_stats)

#         # Connect drop signals for all queues
#         for i in range(1, 8):
#             queue = self.pipeline.get_by_name(f'q{i}')
#             if queue:
#                 queue.connect('overrun', self.on_queue_overrun, f'q{i}')

    # def on_pipeline_message(self, bus, message):
    #     """Handle pipeline messages"""
    #     t = message.type
    #     if t == Gst.MessageType.ERROR:
    #         err, debug = message.parse_error()
    #         print(f"GStreamer Error: {err}, Debug: {debug}")
    #     elif t == Gst.MessageType.EOS:
    #         print(f"GStreamer pipeline reached end of stream")

    def cleanup(self):
        print("Starting cleanup...")
        
        # Stop recording
        self.recording_active = False
        self.audio_recording_active = False
        
        if hasattr(self, 'process_frames_thread') and self.process_frames_thread:
            print("Waiting for frame processing to complete...")
            self.process_frames_thread.join(timeout=60.0)
        
        # Clean up pipeline
        if self.pipeline:
            self.pipeline.cleanup()

        if self.uploader:
            self.uploader.complete_upload()

        if self.meeting_service:
            zoom.DestroyMeetingService(self.meeting_service)
            print("Destroyed Meeting service")
        if self.setting_service:
            zoom.DestroySettingService(self.setting_service)
            print("Destroyed Setting service")
        if self.auth_service:
            zoom.DestroyAuthService(self.auth_service)
            print("Destroyed Auth service")

        if self.audio_helper:
            audio_helper_unsubscribe_result = self.audio_helper.unSubscribe()
            print("audio_helper.unSubscribe() returned", audio_helper_unsubscribe_result)

        if self.video_input_manager:
            self.video_input_manager.cleanup()

        print("CleanUPSDK() called")
        zoom.CleanUPSDK()
        print("CleanUPSDK() finished")

        # Stop monitoring before cleanup
        #self.monitoring_active = False
        #if self.stats_timer:
        #    self.stats_timer.join(timeout=1.0)

    def init(self):
        if os.environ.get('MEETING_ID') is None:
            raise Exception('No MEETING_ID found in environment. Please define this in a .env file located in the repository root')
        if os.environ.get('MEETING_PWD') is None:
            raise Exception('No MEETING_PWD found in environment. Please define this in a .env file located in the repository root')
        if os.environ.get('ZOOM_APP_CLIENT_ID') is None:
            raise Exception('No ZOOM_APP_CLIENT_ID found in environment. Please define this in a .env file located in the repository root')
        if os.environ.get('ZOOM_APP_CLIENT_SECRET') is None:
            raise Exception('No ZOOM_APP_CLIENT_SECRET found in environment. Please define this in a .env file located in the repository root')

        init_param = zoom.InitParam()

        init_param.strWebDomain = "https://zoom.us"
        init_param.strSupportUrl = "https://zoom.us"
        init_param.enableGenerateDump = True
        init_param.emLanguageID = zoom.SDK_LANGUAGE_ID.LANGUAGE_English
        init_param.enableLogByDefault = True

        init_sdk_result = zoom.InitSDK(init_param)
        if init_sdk_result != zoom.SDKERR_SUCCESS:
            raise Exception('InitSDK failed')
        
        self.create_services()

    def on_join(self):
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

        self.participants_ctrl = self.meeting_service.GetMeetingParticipantsController()
        self.my_participant_id = self.participants_ctrl.GetMySelfUser().GetUserID()

        participant_ids_list = self.participants_ctrl.GetParticipantsList()
        print("participant_ids_list", participant_ids_list)
        for participant_id in participant_ids_list:
            if participant_id != self.my_participant_id:
                self.other_participant_id = participant_id
                break
        print("other_participant_id", self.other_participant_id)

        self.audio_ctrl = self.meeting_service.GetMeetingAudioController()
        self.audio_ctrl_event = zoom.MeetingAudioCtrlEventCallbacks(onUserAudioStatusChangeCallback=self.on_user_audio_status_change_callback, onUserActiveAudioChangeCallback=self.on_user_active_audio_change_callback)
        self.audio_ctrl.SetEvent(self.audio_ctrl_event)

    def on_user_active_audio_change_callback(self, user_ids):
        print("on_user_active_audio_change_callback called. user_ids =", user_ids)

        if self.active_speaker_id == user_ids[0]:
            return

        self.active_speaker_id = user_ids[0]

        if self.pipeline.wants_any_video_frames():
            self.video_input_manager.set_mode(mode=VideoInputManager.Mode.ACTIVE_SPEAKER, active_speaker_id=self.active_speaker_id)


    def on_user_audio_status_change_callback(self, user_audio_statuses, otherstuff):
        print("on_user_audio_status_change_callback called. user_audio_statuses =", user_audio_statuses, "otherstuff =", otherstuff)

    def on_mic_initialize_callback(self, sender):
        self.audio_raw_data_sender = sender

    def on_mic_start_send_callback(self):
        return
        with open('sample_program/input_audio/test_audio_16778240.pcm', 'rb') as pcm_file:
            chunk = pcm_file.read(64000*10)
            self.audio_raw_data_sender.send(chunk, 32000, zoom.ZoomSDKAudioChannel_Mono)
    
    def write_to_deepgram(self, data):
        try:
            buffer_bytes = data.GetBuffer()
            self.deepgram_transcriber.send(buffer_bytes)
        except IOError as e:
            print(f"Error: failed to open or write to audio file path: {path}. Error: {e}")
            return
        except Exception as e:
            print(f"Unexpected error occurred: {e}")
            return

    def write_to_file(self, path, data):
        try:
            buffer_bytes = data.GetBuffer()          

            with open(path, 'ab') as file:
                file.write(buffer_bytes)
        except IOError as e:
            print(f"Error: failed to open or write to audio file path: {path}. Error: {e}")
            return
        except Exception as e:
            print(f"Unexpected error occurred: {e}")
            return

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
            self.audio_source = zoom.ZoomSDKAudioRawDataDelegateCallbacks(onMixedAudioRawDataReceivedCallback=self.pipeline.on_mixed_audio_raw_data_received_callback, collectPerformanceData=True)


        audio_helper_subscribe_result = self.audio_helper.subscribe(self.audio_source, False)
        print("audio_helper_subscribe_result =",audio_helper_subscribe_result)

        self.virtual_audio_mic_event_passthrough = zoom.ZoomSDKVirtualAudioMicEventCallbacks(onMicInitializeCallback=self.on_mic_initialize_callback,onMicStartSendCallback=self.on_mic_start_send_callback)
        audio_helper_set_external_audio_source_result = self.audio_helper.setExternalAudioSource(self.virtual_audio_mic_event_passthrough)
        print("audio_helper_set_external_audio_source_result =", audio_helper_set_external_audio_source_result)

        if self.other_participant_id:
            self.video_input_manager.set_mode(mode=VideoInputManager.Mode.ACTIVE_SPEAKER, active_speaker_id=self.other_participant_id)

        # Initialize GStreamer pipeline when starting recording
        self.pipeline.setup_gstreamer_pipeline()

    def stop_raw_recording(self):
        rec_ctrl = self.meeting_service.StopRawRecording()
        if rec_ctrl.StopRawRecording() != zoom.SDKERR_SUCCESS:
            raise Exception("Error with stop raw recording")

    def leave(self):
        if self.audio_source:
            performance_data = self.audio_source.getPerformanceData()
            print("totalProcessingTimeMicroseconds =", performance_data.totalProcessingTimeMicroseconds)
            print("numCalls =", performance_data.numCalls)
            print("maxProcessingTimeMicroseconds =", performance_data.maxProcessingTimeMicroseconds)
            print("minProcessingTimeMicroseconds =", performance_data.minProcessingTimeMicroseconds)
            print("meanProcessingTimeMicroseconds =", float(performance_data.totalProcessingTimeMicroseconds) / performance_data.numCalls)
            
            # Print processing time distribution
            bin_size = (performance_data.processingTimeBinMax - performance_data.processingTimeBinMin) / len(performance_data.processingTimeBinCounts)
            print("\nProcessing time distribution (microseconds):")
            for bin_idx, count in enumerate(performance_data.processingTimeBinCounts):
                if count > 0:
                    bin_start = bin_idx * bin_size
                    bin_end = (bin_idx + 1) * bin_size
                    print(f"{bin_start:6.0f} - {bin_end:6.0f} us: {count:5d} calls")

        if self.meeting_service is None:
            return
        
        status = self.meeting_service.GetMeetingStatus()
        if status == zoom.MEETING_STATUS_IDLE:
            return

        self.meeting_service.Leave(zoom.LEAVE_MEETING)

    def join_meeting(self):
        mid = os.environ.get('MEETING_ID')
        password = os.environ.get('MEETING_PWD')
        display_name = "My meeting bot"

        meeting_number = int(mid)

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

        join_result = self.meeting_service.Join(join_param)
        print("join_result =",join_result)

        self.audio_settings = self.setting_service.GetAudioSettings()
        self.audio_settings.EnableAutoJoinAudio(True)

    def on_reminder_notify(self, content, handler):
        if handler:
            handler.accept()

    def auth_return(self, result):
        if result == zoom.AUTHRET_SUCCESS:
            print("Auth completed successfully.")
            return self.join_meeting()

        raise Exception("Failed to authorize. result =", result)
    
    def meeting_status_changed(self, status, iResult):
        if status == zoom.MEETING_STATUS_INMEETING:
            return self.on_join()
        
        print("meeting_status_changed called. status =",status,"iResult=",iResult)

    def create_services(self):
        self.meeting_service = zoom.CreateMeetingService()
        
        self.setting_service = zoom.CreateSettingService()

        self.meeting_service_event = zoom.MeetingServiceEventCallbacks(onMeetingStatusChangedCallback=self.meeting_status_changed)
                
        meeting_service_set_revent_result = self.meeting_service.SetEvent(self.meeting_service_event)
        if meeting_service_set_revent_result != zoom.SDKERR_SUCCESS:
            raise Exception("Meeting Service set event failed")
        
        self.auth_event = zoom.AuthServiceEventCallbacks(onAuthenticationReturnCallback=self.auth_return)

        self.auth_service = zoom.CreateAuthService()

        set_event_result = self.auth_service.SetEvent(self.auth_event)
        print("set_event_result =",set_event_result)
    
        # Use the auth service
        auth_context = zoom.AuthContext()
        auth_context.jwt_token = generate_jwt(os.environ.get('ZOOM_APP_CLIENT_ID'), os.environ.get('ZOOM_APP_CLIENT_SECRET'))

        result = self.auth_service.SDKAuth(auth_context)
    
        if result == zoom.SDKError.SDKERR_SUCCESS:
            print("Authentication successful")
        else:
            print("Authentication failed with error:", result)

    # def monitor_pipeline_stats(self):
    #     """Periodically print pipeline statistics"""
    #     if not self.recording_active:
    #         return False
        
    #     try:
    #         # Print dropped buffer counts since last check
    #         print("\nDropped Buffers Since Last Check:")
    #         for queue_name in self.queue_drops:
    #             drops = self.queue_drops[queue_name] - self.last_reported_drops[queue_name]
    #             if drops > 0:
    #                 print(f"  {queue_name}: {drops} buffers dropped")
    #             self.last_reported_drops[queue_name] = self.queue_drops[queue_name]

    #         # # Get stats from all queue elements
    #         # for i in range(1, 8):
    #         #     queue = self.pipeline.get_by_name(f'q{i}')
    #         #     if queue:
    #         #         stats = {
    #         #             'current_level_buffers': queue.get_property('current-level-buffers'),
    #         #             'current_level_bytes': queue.get_property('current-level-bytes'),
    #         #             'current_level_time': queue.get_property('current-level-time') / Gst.SECOND,
    #         #             'max_size_buffers': queue.get_property('max-size-buffers'),
    #         #             'max_size_bytes': queue.get_property('max-size-bytes'),
    #         #             'max_size_time': queue.get_property('max-size-time') / Gst.SECOND,
    #         #         }
                    
    #         #         print(f"\nQueue {i} Statistics:")
    #         #         for key, value in stats.items():
    #         #             print(f"  {key}: {value}")

    #         # Get encoder stats if available
    #         # x264enc = self.pipeline.get_by_name('x264enc')
    #         # if x264enc:
    #         #     print("\nX264 Encoder Statistics:")
    #         #     print(f"  Bitrate: {x264enc.get_property('bitrate')} kbps")
                
    #         # # Get audio encoder stats
    #         # voaacenc = self.pipeline.get_by_name('voaacenc')
    #         # if voaacenc:
    #         #     print("\nAAC Encoder Statistics:")
    #         #     print(f"  Bitrate: {voaacenc.get_property('bitrate')} bps")

    #     except Exception as e:
    #         print(f"Error getting pipeline stats: {e}")
        
    #     return True  # Continue timer

    # def on_queue_overrun(self, queue, queue_name):
    #     """Callback for when a queue drops buffers"""
    #     self.queue_drops[queue_name] += 1
    #     return True