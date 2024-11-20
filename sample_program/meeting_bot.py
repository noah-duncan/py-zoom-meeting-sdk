import zoom_meeting_sdk as zoom
import jwt
from deepgram_transcriber import DeepgramTranscriber
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

        self.audio_raw_data_sender = None
        self.virtual_audio_mic_event_passthrough = None

        self.deepgram_transcriber = DeepgramTranscriber()

        self.my_participant_id = None
        self.other_participant_id = None
        self.participants_ctrl = None
        self.meeting_reminder_event = None
        self.audio_print_counter = 0

        self.video_helper = None
        self.renderer_delegate = None
        self.video_frame_counter = 0

        # Add new properties for video recording
        self.video_frames = Queue()
        self.pipeline = None
        self.appsrc = None
        self.recording_active = False
        
        # Initialize GStreamer
        Gst.init(None)

        self.last_frame_time = None
        self.first_frame_time = None

        # Add new properties for audio recording
        self.audio_pipeline = None
        self.audio_appsrc = None
        self.audio_recording_active = False

    def setup_gstreamer_pipeline(self):
        """Initialize GStreamer pipelines for MP4 video and MP3 audio recording"""
        # Video pipeline (existing)
        pipeline_str = (
            'appsrc name=source do-timestamp=false ! '
            'videoconvert ! '
            'x264enc tune=zerolatency bitrate=2000 ! '
            'h264parse ! '
            'mp4mux ! '
            'filesink location=meeting_recording.mp4'
        )
        self.pipeline = Gst.parse_launch(pipeline_str)
        self.appsrc = self.pipeline.get_by_name('source')
        
        # Configure video appsrc (existing code)
        caps = Gst.Caps.from_string('video/x-raw,format=BGR,width=640,height=360,framerate=30/1')
        self.appsrc.set_property('caps', caps)
        self.appsrc.set_property('format', Gst.Format.TIME)
        self.appsrc.set_property('block', True)
        self.appsrc.set_property('is-live', True)

        # Add audio pipeline
        audio_pipeline_str = (
            'appsrc name=audio_source do-timestamp=false ! '
            'audioconvert ! '
            'audio/x-raw,format=S16LE,channels=1,rate=32000 ! '
            'lamemp3enc bitrate=128 ! '
            'filesink location=meeting_recording.mp3'
        )
        


#rawaudioparse use-sink-caps=false format=pcm pcm-format=s16le sample-rate=32000 num-channels=1 ! audioconvert ! lamemp3enc ! filesink location=output.mp3

        self.audio_pipeline = Gst.parse_launch(audio_pipeline_str)
        self.audio_appsrc = self.audio_pipeline.get_by_name('audio_source')

        # Configure audio appsrc
        audio_caps = Gst.Caps.from_string(
            'audio/x-raw,format=S16LE,channels=1,rate=32000,layout=interleaved'
        )
        self.audio_appsrc.set_property('caps', audio_caps)
        self.audio_appsrc.set_property('format', Gst.Format.TIME)
        self.audio_appsrc.set_property('do-timestamp', False)
        self.audio_appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM = 0

        # Set up bus for both pipelines
        for pipeline, name in [(self.pipeline, 'video'), (self.audio_pipeline, 'audio')]:
            bus = pipeline.get_bus()
            bus.add_signal_watch()
            bus.connect('message', lambda b, m, n=name: self.on_pipeline_message(b, m, n))
            pipeline.set_state(Gst.State.PLAYING)

        self.recording_active = True
        self.audio_recording_active = True

        self.process_frames_thread = threading.Thread(target=self.process_frames)
        self.process_frames_thread.daemon = True
        self.process_frames_thread.start()

    def on_pipeline_message(self, bus, message, pipeline_name):
        """Handle pipeline messages"""
        t = message.type
        if t == Gst.MessageType.ERROR:
            err, debug = message.parse_error()
            print(f"GStreamer {pipeline_name} Error: {err}, Debug: {debug}")
        elif t == Gst.MessageType.EOS:
            print(f"GStreamer {pipeline_name} pipeline reached end of stream")

    def process_frames(self):
        """Process frames from queue and push to GStreamer pipeline"""
        while self.recording_active or not self.video_frames.empty():
            try:
                frame, timestamp_ns = self.video_frames.get(timeout=1.0)
                if frame is None:
                    continue
                
                if self.first_frame_time is None:
                    self.first_frame_time = timestamp_ns
                
                # Calculate buffer timestamp relative to first frame
                buffer_pts = timestamp_ns - self.first_frame_time
                
                # Create buffer with timestamp
                buffer = Gst.Buffer.new_wrapped(frame.tobytes())
                buffer.pts = buffer_pts
                
                # Calculate duration based on time until next frame
                # Default to 33ms (30fps) if this is the last frame
                buffer.duration = 33 * 1000 * 1000  # 33ms in nanoseconds
                
                # Push buffer to pipeline
                ret = self.appsrc.emit('push-buffer', buffer)
                if ret != Gst.FlowReturn.OK:
                    print(f"Warning: Failed to push buffer to pipeline: {ret}")
                
            except Exception as e:
                print(f"Error processing frame: {e}")
                continue

    def cleanup(self):
        print("Starting cleanup...")
        
        # Stop both video and audio recording
        self.recording_active = False
        self.audio_recording_active = False
        
        # Existing video cleanup...
        if hasattr(self, 'process_frames_thread') and self.process_frames_thread:
            print("Waiting for frame processing to complete...")
            self.process_frames_thread.join(timeout=5.0)
        
        # Clean up both pipelines
        for pipeline, appsrc, name in [
            (self.pipeline, self.appsrc, 'video'),
            (self.audio_pipeline, self.audio_appsrc, 'audio')
        ]:
            if pipeline:
                print(f"Shutting down GStreamer {name} pipeline...")
                if appsrc:
                    appsrc.emit('end-of-stream')
                
                bus = pipeline.get_bus()
                msg = bus.timed_pop_filtered(
                    Gst.CLOCK_TIME_NONE,
                    Gst.MessageType.EOS | Gst.MessageType.ERROR
                )
                
                if msg and msg.type == Gst.MessageType.ERROR:
                    err, debug = msg.parse_error()
                    print(f"Error during {name} pipeline shutdown: {err}, {debug}")
                
                pipeline.set_state(Gst.State.NULL)
                print(f"GStreamer {name} pipeline shut down")

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

        print("CleanUPSDK() called")
        zoom.CleanUPSDK()
        print("CleanUPSDK() finished")

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

    def on_mic_initialize_callback(self, sender):
        self.audio_raw_data_sender = sender

    def on_mic_start_send_callback(self):
        return
        with open('sample_program/input_audio/test_audio_16778240.pcm', 'rb') as pcm_file:
            chunk = pcm_file.read(64000*10)
            self.audio_raw_data_sender.send(chunk, 32000, zoom.ZoomSDKAudioChannel_Mono)

    def on_one_way_audio_raw_data_received_callback(self, data, node_id):
        # Add GStreamer audio recording
        if self.audio_recording_active and self.audio_appsrc:
            try:
                buffer_bytes = data.GetBuffer()
                buffer = Gst.Buffer.new_wrapped(buffer_bytes)
                
                # Calculate timestamp if needed
                if hasattr(self, 'first_audio_time') and self.first_audio_time:
                    buffer.pts = time.time_ns() - self.first_audio_time
                else:
                    self.first_audio_time = time.time_ns()
                    buffer.pts = 0
                
                ret = self.audio_appsrc.emit('push-buffer', buffer)
                if ret != Gst.FlowReturn.OK:
                    print(f"Warning: Failed to push audio buffer to pipeline: {ret}")
            except Exception as e:
                print(f"Error processing audio data: {e}")

        # Existing audio processing code...
        if os.environ.get('DEEPGRAM_API_KEY') is None:
            volume = normalized_rms_audio(data.GetBuffer())
            if self.audio_print_counter % 20 < 2 and volume > 0.01:
                print("Received audio from user", self.participants_ctrl.GetUserByUserID(node_id).GetUserName(), "with volume", volume)
                print("To get transcript add DEEPGRAM_API_KEY to the .env file")
            self.audio_print_counter += 1
            return

    def on_raw_data_frame_received_callback(self, data):
        try:
            # Convert YUV420 to BGR
            yuv_data = np.frombuffer(data.GetBuffer(), dtype=np.uint8)
            yuv_frame = yuv_data.reshape((360 * 3//2, 640))  # Assuming 640x360 resolution
            bgr_frame = cv2.cvtColor(yuv_frame, cv2.COLOR_YUV2BGR_I420)
            
            # Verify frame is valid
            if bgr_frame is None or bgr_frame.size == 0:
                print("Warning: Invalid frame received")
                return
            
            # Get current timestamp in nanoseconds
            current_time_ns = time.time_ns()
            
            # Add frame and timestamp to queue for processing
            self.video_frames.put((bgr_frame, current_time_ns))
            
        except Exception as e:
            print(f"Error processing video frame: {e}")

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
            self.audio_source = zoom.ZoomSDKAudioRawDataDelegateCallbacks(onOneWayAudioRawDataReceivedCallback=self.on_one_way_audio_raw_data_received_callback, collectPerformanceData=True)


        audio_helper_subscribe_result = self.audio_helper.subscribe(self.audio_source, False)
        print("audio_helper_subscribe_result =",audio_helper_subscribe_result)

        self.virtual_audio_mic_event_passthrough = zoom.ZoomSDKVirtualAudioMicEventCallbacks(onMicInitializeCallback=self.on_mic_initialize_callback,onMicStartSendCallback=self.on_mic_start_send_callback)
        audio_helper_set_external_audio_source_result = self.audio_helper.setExternalAudioSource(self.virtual_audio_mic_event_passthrough)
        print("audio_helper_set_external_audio_source_result =", audio_helper_set_external_audio_source_result)

        self.renderer_delegate = zoom.ZoomSDKRendererDelegateCallbacks(onRawDataFrameReceivedCallback=self.on_raw_data_frame_received_callback)
        self.video_helper = zoom.createRenderer(self.renderer_delegate)

        self.video_helper.setRawDataResolution(zoom.ZoomSDKResolution_720P)
        subscribe_result = self.video_helper.subscribe(self.other_participant_id, zoom.ZoomSDKRawDataType.RAW_DATA_TYPE_VIDEO)
        print("video_helper subscribe_result =", subscribe_result)

        # Initialize GStreamer pipeline when starting recording
        self.setup_gstreamer_pipeline()

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