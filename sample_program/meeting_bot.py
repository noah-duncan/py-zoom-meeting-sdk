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
import queue

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
        self.video_appsrc = None
        self.audio_appsrc = None
        self.recording_active = False
        
        # Initialize GStreamer
        Gst.init(None)

        self.last_audio_timestamp = None
        self.audio_discontinuity = False
        self.recording_start_time = None
        self.video_recording_start_time = None

        # Add new audio queue property
        self.audio_frames = Queue()
        
        # Add audio processing thread property
        self.process_audio_thread = None

    def setup_gstreamer_pipeline(self):
        """Initialize GStreamer pipeline for MP4 recording with audio"""
        pipeline_str = (
            # Video branch with simpler configuration
            'appsrc name=video_source do-timestamp=false ! '
            'videoconvert ! '
            'video/x-raw,format=I420,width=640,height=360,framerate=30/1 ! '
            'x264enc tune=zerolatency speed-preset=ultrafast ! '
            'video/x-h264,profile=baseline ! '
            'h264parse ! '
            'queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 ! '
            'mux. '
            
            # Audio branch with simpler configuration
            'appsrc name=audio_source do-timestamp=false ! '
            'audioconvert ! '
            'audio/x-raw,format=S16LE,channels=1,rate=32000 ! '
            'voaacenc bitrate=128000 ! '
            'aacparse ! '
            'queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 ! '
            'mux. '
            
            # MP4 muxer
            'mp4mux name=mux faststart=true ! '
            'filesink location=meeting_recording.mp4'
        )
        
        try:
            print("Creating pipeline...")
            self.pipeline = Gst.parse_launch(pipeline_str)
            
            print("Configuring video source...")
            self.video_appsrc = self.pipeline.get_by_name('video_source')
            caps = Gst.Caps.from_string(
                'video/x-raw,format=BGR,width=640,height=360,framerate=30/1'
            )
            self.video_appsrc.set_property('caps', caps)
            self.video_appsrc.set_property('format', Gst.Format.TIME)
            self.video_appsrc.set_property('do-timestamp', True)
            self.video_appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM = 0
            
            print("Configuring audio source...")
            self.audio_appsrc = self.pipeline.get_by_name('audio_source')
            audio_caps = Gst.Caps.from_string(
                'audio/x-raw,format=S16LE,channels=1,rate=32000,layout=interleaved'
            )
            self.audio_appsrc.set_property('caps', audio_caps)
            self.audio_appsrc.set_property('format', Gst.Format.TIME)
            self.audio_appsrc.set_property('do-timestamp', True)
            self.audio_appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM = 0

            # Set up bus monitoring
            bus = self.pipeline.get_bus()
            bus.add_signal_watch()
            bus.connect('message', self.on_pipeline_message)
            
            # First set pipeline to READY state
            print("Setting pipeline to READY...")
            ret = self.pipeline.set_state(Gst.State.READY)
            if ret == Gst.StateChangeReturn.FAILURE:
                raise Exception("Failed to set pipeline to READY")
            
            # Wait for READY state
            timeout = 5 * Gst.SECOND
            state_change = self.pipeline.get_state(timeout)
            if state_change[0] == Gst.StateChangeReturn.FAILURE:
                raise Exception("Failed to reach READY state")
            
            # Now set to PLAYING state
            print("Setting pipeline to PLAYING...")
            ret = self.pipeline.set_state(Gst.State.PLAYING)
            
            # Handle async state change
            if ret == Gst.StateChangeReturn.ASYNC:
                print("Pipeline is changing state asynchronously...")
                # Wait for up to 5 seconds for state change
                state_change = self.pipeline.get_state(5 * Gst.SECOND)
                if state_change[0] == Gst.StateChangeReturn.SUCCESS:
                    print("Pipeline successfully entered PLAYING state")
                elif state_change[0] == Gst.StateChangeReturn.ASYNC:
                    print("Pipeline is still changing state asynchronously, continuing anyway...")
                else:
                    raise Exception(f"Failed to reach PLAYING state: {state_change[0]}")
            elif ret == Gst.StateChangeReturn.FAILURE:
                raise Exception("Failed to set pipeline to PLAYING")
            
            self.recording_active = True
            self.audio_timestamp = 0
            
            # Start frame processing thread
            self.process_frames_thread = threading.Thread(target=self.process_frames)
            self.process_frames_thread.daemon = True
            self.process_frames_thread.start()
            
            # Start audio processing thread along with video
            self.process_audio_thread = threading.Thread(target=self.process_audio_frames)
            self.process_audio_thread.daemon = True
            self.process_audio_thread.start()
            
        except Exception as e:
            print(f"Failed to setup GStreamer pipeline: {e}")
            if self.pipeline:
                # Debug info
                print("\nPipeline state:")
                state = self.pipeline.get_state(0)
                print(f"Current: {state[1]}, Pending: {state[2]}")
                
                # Print element states
                print("\nElement states:")
                iterator = self.pipeline.iterate_elements()
                while True:
                    result = iterator.next()
                    if result[0] == Gst.IteratorResult.DONE:
                        break
                    if result[0] == Gst.IteratorResult.OK:
                        element = result[1]
                        state = element.get_state(0)
                        print(f"{element.get_name()}: {state[1]}")
            
            self.cleanup()
            raise

    def _on_video_buffer_probe(self, pad, info):
        """Monitor video buffers for debugging"""
        buffer = info.get_buffer()
        if buffer:
            print(f"Video buffer: pts={buffer.pts}, size={buffer.get_size()}")
        return Gst.PadProbeReturn.OK


    def on_pipeline_message(self, bus, message):
        """Handle pipeline messages"""
        t = message.type
        if t == Gst.MessageType.ERROR:
            err, debug = message.parse_error()
            print(f"GStreamer Error: {err}, Debug: {debug}")
            # Get element that caused the error
            src = message.src
            print(f"Error from element {src.get_name()}")
            self.cleanup()
        elif t == Gst.MessageType.EOS:
            print("GStreamer pipeline reached end of stream")
            self.cleanup()
        elif t == Gst.MessageType.WARNING:
            warn, debug = message.parse_warning()
            print(f"GStreamer Warning: {warn}, Debug: {debug}")
        elif t == Gst.MessageType.STATE_CHANGED:
            old, new, pending = message.parse_state_changed()
            #print(f"Element {message.src.get_name()} state changed from {old} to {new} (pending: {pending})")

    def process_frames(self):
        """Process frames from queue and push to GStreamer pipeline"""
        while self.recording_active or not self.video_frames.empty():
            try:
                try:
                    frame, capture_time = self.video_frames.get(timeout=1.0/25)
                except queue.Empty:
                    continue

                if frame is None:
                    continue
                
                # Create buffer with proper timing information
                buffer = Gst.Buffer.new_wrapped(frame.tobytes())
                
                # Use absolute timestamp relative to recording start
                if self.video_recording_start_time is None:
                    self.video_recording_start_time = capture_time
                
                buffer.pts = capture_time - self.video_recording_start_time
                buffer.duration = int(1e9 / 30)  # Duration for 30fps in nanoseconds
                buffer.dts = buffer.pts
                #print("video buffer.pts = ", float(buffer.pts) / Gst.SECOND)
                #print("video buffer.duration = ", float(buffer.duration) / Gst.SECOND)
                
                # Push buffer to pipeline with error checking
                if self.video_appsrc:
                    ret = self.video_appsrc.emit('push-buffer', buffer)
                    if ret != Gst.FlowReturn.OK:
                        print(f"Warning: Failed to push buffer to pipeline: {ret}")
                
            except Exception as e:
                print(f"Error processing frame: {e}")
                continue

    def process_audio_frames(self):
        """Process audio frames from queue and push to GStreamer pipeline"""
        while self.recording_active or not self.audio_frames.empty():
            try:
                try:
                    audio_data, capture_time = self.audio_frames.get(timeout=1.0/100)  # More frequent timeout for audio
                except queue.Empty:
                    continue

                if audio_data is None:
                    continue
                
                if self.recording_start_time is None:
                    self.recording_start_time = capture_time
                
                # Create buffer with proper timing information
                buffer = Gst.Buffer.new_wrapped(audio_data)
                
                # Calculate duration based on sample rate and buffer size
                num_samples = len(audio_data) // 2  # 16-bit = 2 bytes per sample
                duration = num_samples * Gst.SECOND // 32000  # Convert to nanoseconds
                
                # Set buffer timing info
                buffer.pts = capture_time - self.recording_start_time
                buffer.duration = duration
                if normalized_rms_audio(audio_data) > 0.01:
                    print("buffer.pts = ", float(buffer.pts) / Gst.SECOND)
                    print("buffer.duration = ", float(buffer.duration) / Gst.SECOND)
                    print("recording_start_time = ", float(self.recording_start_time) / Gst.SECOND)
                # Handle discontinuity
                if self.audio_discontinuity:
                    buffer.set_flags(Gst.BufferFlags.DISCONT)
                    self.audio_discontinuity = False
                
                # Push buffer to pipeline with error checking
                if self.audio_appsrc:
                    ret = self.audio_appsrc.emit('push-buffer', buffer)
                    if ret != Gst.FlowReturn.OK:
                        print(f"Warning: Failed to push audio buffer to pipeline: {ret}")
                
            except Exception as e:
                print(f"Error processing audio frame: {e}")
                continue

    def cleanup(self):
        """Cleanup resources and shutdown pipeline properly"""
        print("Starting cleanup...")
        
        # Set flag to stop recording
        self.recording_active = False
        
        try:
            # Wait for both video and audio processing threads
            if hasattr(self, 'process_frames_thread') and self.process_frames_thread:
                self.process_frames_thread.join(timeout=5.0)
            if hasattr(self, 'process_audio_thread') and self.process_audio_thread:
                self.process_audio_thread.join(timeout=5.0)
            
            if self.pipeline:
                print("Shutting down GStreamer pipeline...")
                # Send EOS events
                if hasattr(self, 'video_appsrc'):
                    self.video_appsrc.emit('end-of-stream')
                if hasattr(self, 'audio_appsrc'):
                    self.audio_appsrc.emit('end-of-stream')
                
                # Wait for EOS to propagate
                self.pipeline.send_event(Gst.Event.new_eos())
                
                # Wait for pipeline to process remaining data (with timeout)
                timeout = 5 * Gst.SECOND
                self.pipeline.get_bus().timed_pop_filtered(timeout, Gst.MessageType.EOS | Gst.MessageType.ERROR)
                
                # Set pipeline state to NULL
                self.pipeline.set_state(Gst.State.NULL)
                
        except Exception as e:
            print(f"Error during cleanup: {e}")
        finally:
            # Clear references
            self.pipeline = None
            self.video_appsrc = None
            self.audio_appsrc = None
            print("Cleanup completed")

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
        if self.recording_active and hasattr(self, 'audio_appsrc') and hasattr(self, 'video_appsrc'):
            try:
                capture_time = time.time_ns()
                audio_data = data.GetBuffer()
                self.audio_frames.put((audio_data, capture_time))
                
            except Exception as e:
                print(f"Error queueing audio data: {e}")
                self.cleanup()

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

    def on_raw_data_frame_received_callback(self, data):
        try:
            if self.recording_active and hasattr(self, 'video_appsrc')  and hasattr(self, 'audio_appsrc'):
                capture_time = time.time_ns()
                yuv_data = np.frombuffer(data.GetBuffer(), dtype=np.uint8)
                yuv_frame = yuv_data.reshape((360 * 3//2, 640))
                bgr_frame = cv2.cvtColor(yuv_frame, cv2.COLOR_YUV2BGR_I420)
                
                if bgr_frame is None or bgr_frame.size == 0:
                    print("Warning: Invalid frame received")
                    return
                
                self.video_frames.put((bgr_frame, capture_time))
                
        except Exception as e:
            print(f"Error processing video frame: {e}")

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
