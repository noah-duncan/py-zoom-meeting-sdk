import zoom_meeting_sdk as zoom
import numpy as np
import cv2
import time
from gi.repository import GLib

def convert_yuv420_frame_to_bgr2(frame_bytes, width, height):
    # Convert bytes to numpy array
    yuv_data = np.frombuffer(frame_bytes.GetBuffer(), dtype=np.uint8)

    # Reshape into I420 format with U/V planes
    yuv_frame = yuv_data.reshape((height * 3//2, width))

    # Convert from YUV420 to BGR
    bgr_frame = cv2.cvtColor(yuv_frame, cv2.COLOR_YUV2BGR_I420)

    return bgr_frame

def convert_yuv420_frame_to_bgr(frame_bytes, width, height):
    # Allocate a buffer for the complete YUV420 data
    total_size = height * width * 3 // 2
    yuv_data = np.empty((total_size,), dtype=np.uint8)
    
    # Get the separate Y, U, V buffers from the input
    y_buffer = frame_bytes.GetYBuffer()
    u_buffer = frame_bytes.GetUBuffer()
    v_buffer = frame_bytes.GetVBuffer()
    
    # Copy Y buffer
    y_size = height * width
    yuv_data[0:y_size] = np.frombuffer(y_buffer, dtype=np.uint8)
    
    # Copy U buffer
    u_size = height * width // 4
    yuv_data[y_size:y_size + u_size] = np.frombuffer(u_buffer, dtype=np.uint8)
    
    # Copy V buffer
    yuv_data[y_size + u_size:] = np.frombuffer(v_buffer, dtype=np.uint8)
    
    
    # Now convert YUV420 to BGR using OpenCV
    yuv_image = yuv_data.reshape((height * 3 // 2, width))
    bgr_image = cv2.cvtColor(yuv_image, cv2.COLOR_YUV2BGR_I420)
    #bgr_image_scaled = cv2.resize(bgr_image, (640, 360), interpolation=cv2.INTER_LINEAR)
    #cv2.imwrite('outputfres.png', bgr_image_scaled)
    
    return bgr_image

class VideoInputStream:
    def __init__(self, video_input_manager, user_id):
        self.video_input_manager = video_input_manager
        self.user_id = user_id
        self.cleaned_up = False
        self.last_frame_time = time.time()
        self.black_frame_timer_id = None

        self.renderer_delegate = zoom.ZoomSDKRendererDelegateCallbacks(
            onRawDataFrameReceivedCallback=self.on_raw_video_frame_received_callback,
            onRawDataStatusChangedCallback=self.on_raw_data_status_changed_callback
        )

        self.renderer = zoom.createRenderer(self.renderer_delegate)
        self.renderer.setRawDataResolution(zoom.ZoomSDKResolution_1080P)
        subscribe_result = self.renderer.subscribe(self.user_id, zoom.ZoomSDKRawDataType.RAW_DATA_TYPE_SHARE)
        print("subscribe result", subscribe_result)
        self.raw_data_status = zoom.RawData_Off
        
        # Start the black frame timer (80ms = 80000 microseconds)
        self.black_frame_timer_id = GLib.timeout_add(80, self.send_black_frame)

    def send_black_frame(self):
        if self.cleaned_up:
            return False
            
        current_time = time.time()
        if current_time - self.last_frame_time >= 0.08 and self.raw_data_status == zoom.RawData_Off:
            # Create a black frame of the same dimensions
            black_frame = np.zeros((360, 640, 3), dtype=np.uint8)  # BGR format
            self.video_input_manager.new_frame_callback(black_frame)
            
        return not self.cleaned_up  # Continue timer if not cleaned up

    def cleanup(self):
        print("starting cleanup input stream for user", self.user_id)
        if self.black_frame_timer_id is not None:
            GLib.source_remove(self.black_frame_timer_id)
            self.black_frame_timer_id = None
        self.renderer.unSubscribe()
        print("finished cleanup input stream for user", self.user_id)
        self.cleaned_up = True

    def on_raw_video_frame_received_callback(self, data):
        print("GOT SHARE FRAME2")
        if self.cleaned_up:
            return
        
        if not self.video_input_manager.wants_frames_for_user(self.user_id):
            return

        self.last_frame_time = time.time()
        bgr_frame = convert_yuv420_frame_to_bgr(data, data.GetStreamWidth(), data.GetStreamHeight())

        if bgr_frame is None or bgr_frame.size == 0:
            print("Warning: Invalid frame received")
            return

        print("GOT SHARE FRAME44 buffer len", len(data.GetBuffer()), "width", data.GetStreamWidth(), "height", data.GetStreamHeight(), "is limited", data.IsLimitedI420())
        self.video_input_manager.new_frame_callback(bgr_frame)

    def on_raw_data_status_changed_callback(self, status):
        self.raw_data_status = status
    
class VideoInputManager:
    class Mode:
        ACTIVE_SPEAKER = 1

    def __init__(self, *, new_frame_callback, wants_any_frames_callback):
        self.new_frame_callback = new_frame_callback
        self.wants_any_frames_callback = wants_any_frames_callback
        self.mode = None
        self.input_streams = []

    def add_input_streams_if_needed(self, user_ids):
        streams_to_remove = [
            input_stream for input_stream in self.input_streams 
            if input_stream.user_id not in user_ids
        ]

        for stream in streams_to_remove:
            stream.cleanup()
            self.input_streams.remove(stream)

        for user_id in user_ids:
            if any(input_stream.user_id == user_id for input_stream in self.input_streams):
                continue

            self.input_streams.append(VideoInputStream(self, user_id))

    def cleanup(self):
        for input_stream in self.input_streams:
            input_stream.cleanup()

    def set_mode(self, *, mode, active_speaker_id):
        if mode != VideoInputManager.Mode.ACTIVE_SPEAKER:
            raise Exception("Unsupported mode " + str(mode))
        
        self.mode = mode

        if self.mode == VideoInputManager.Mode.ACTIVE_SPEAKER:
            self.active_speaker_id = active_speaker_id
            self.add_input_streams_if_needed([active_speaker_id])

    def wants_frames_for_user(self, user_id):
        if not self.wants_any_frames_callback():
            return False
    
        if self.mode == VideoInputManager.Mode.ACTIVE_SPEAKER and user_id != self.active_speaker_id:
            return False

        return True