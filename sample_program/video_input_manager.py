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
    
    #cv2.imwrite('outputfres.png', bgr_image_scaled)
    
    return bgr_image

class VideoInputStream:
    def __init__(self, video_input_manager, user_id, stream_type):
        self.video_input_manager = video_input_manager
        self.user_id = user_id
        self.stream_type = stream_type
        self.cleaned_up = False
        self.last_frame_time = time.time()
        self.black_frame_timer_id = None
        self.frame_counter = 0
        # Add FPS tracking variables
        self.fps = 0
        self.fps_start_time = time.time()
        self.FPS_UPDATE_INTERVAL = 1.0  # Update FPS every second

        self.renderer_delegate = zoom.ZoomSDKRendererDelegateCallbacks(
            onRawDataFrameReceivedCallback=self.on_raw_video_frame_received_callback,
            onRawDataStatusChangedCallback=self.on_raw_data_status_changed_callback,
            collectPerformanceData=True
        )

        self.renderer = zoom.createRenderer(self.renderer_delegate)
        self.renderer.setRawDataResolution(zoom.ZoomSDKResolution_90P)
        
        # Choose the correct raw data type based on stream_type
        raw_data_type = (zoom.ZoomSDKRawDataType.RAW_DATA_TYPE_SHARE 
                        if stream_type == VideoInputManager.StreamType.SCREENSHARE 
                        else zoom.ZoomSDKRawDataType.RAW_DATA_TYPE_VIDEO)
        
        subscribe_result = self.renderer.subscribe(self.user_id, raw_data_type)
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
            self.video_input_manager.new_frame_callback_for_stream((black_frame, self.stream_type, self.user_id))
            print("sent black frame")
            
        return not self.cleaned_up  # Continue timer if not cleaned up

    def cleanup(self):
        performance_data = self.renderer_delegate.getPerformanceData()
        print("starting cleanup input stream for user", self.user_id)
        if self.black_frame_timer_id is not None:
            GLib.source_remove(self.black_frame_timer_id)
            self.black_frame_timer_id = None
        self.renderer.unSubscribe()
        print("finished cleanup input stream for user", self.user_id)
        self.cleaned_up = True

        print("totalProcessingTimeMicroseconds =", performance_data.totalProcessingTimeMicroseconds)
        print("numCalls =", performance_data.numCalls)
        print("maxProcessingTimeMicroseconds =", performance_data.maxProcessingTimeMicroseconds)
        print("minProcessingTimeMicroseconds =", performance_data.minProcessingTimeMicroseconds)
        print("meanProcessingTimeMicroseconds =", float(performance_data.totalProcessingTimeMicroseconds) / performance_data.numCalls)
        

    def on_raw_video_frame_received_callback(self, data):
        if self.cleaned_up:
            return
            
        # Update FPS calculation
        current_time = time.time()
        self.frame_counter += 1
        
        # Calculate FPS every second
        elapsed_time = current_time - self.fps_start_time
        if elapsed_time >= self.FPS_UPDATE_INTERVAL:
            self.fps = self.frame_counter / elapsed_time
            print(f"Current FPS: {self.fps:.2f}")
            # Reset counters
            self.frame_counter = 0
            self.fps_start_time = current_time

        if not self.video_input_manager.wants_frames_for_user(self.user_id):
            return
        
        #print("GOT SHARE FRAME44 buffer len", len(data.GetBuffer()), "width", data.GetStreamWidth(), "height", data.GetStreamHeight(), "is limited", data.IsLimitedI420())

        

        self.last_frame_time = current_time
        bgr_frame = convert_yuv420_frame_to_bgr(data, data.GetStreamWidth(), data.GetStreamHeight())

        if bgr_frame is None or bgr_frame.size == 0:
            print("Warning: Invalid frame received")
            return

        #print("GOT SHARE FRAME44 buffer len", len(data.GetBuffer()), "width", data.GetStreamWidth(), "height", data.GetStreamHeight(), "is limited", data.IsLimitedI420())
        self.video_input_manager.new_frame_callback_for_stream((bgr_frame, self.stream_type, self.user_id))

    def on_raw_data_status_changed_callback(self, status):
        self.raw_data_status = status
    
class VideoInputManager:

    class StreamType:
        VIDEO = 1
        SCREENSHARE = 2

    class Mode:
        ACTIVE_SPEAKER = 1
        SCREENSHARE_AND_ACTIVE_SPEAKER = 2

    def __init__(self, *, new_frame_callback, wants_any_frames_callback):
        self.new_frame_callback = new_frame_callback
        self.wants_any_frames_callback = wants_any_frames_callback
        self.mode = None
        self.input_streams = []

        self.current_frame = np.zeros((1080, 1920, 3), dtype=np.uint8)  # BGR format
        self.last_speaker_frame = None  # Add this to store the last speaker frame

    def add_input_streams_if_needed(self, streams_info):
        streams_to_remove = [
            input_stream for input_stream in self.input_streams 
            if not any(
                stream_info['user_id'] == input_stream.user_id and 
                stream_info['stream_type'] == input_stream.stream_type 
                for stream_info in streams_info
            )
        ]

        for stream in streams_to_remove:
            stream.cleanup()
            self.input_streams.remove(stream)

        for stream_info in streams_info:
            if any(input_stream.user_id == stream_info['user_id'] and input_stream.stream_type == stream_info['stream_type'] for input_stream in self.input_streams):
                continue

            self.input_streams.append(VideoInputStream(self, stream_info['user_id'], stream_info['stream_type']))

    def cleanup(self):
        for input_stream in self.input_streams:
            input_stream.cleanup()

    def set_mode(self, *, mode, active_speaker_id):
        if mode != VideoInputManager.Mode.ACTIVE_SPEAKER and mode != VideoInputManager.Mode.SCREENSHARE_AND_ACTIVE_SPEAKER:
            raise Exception("Unsupported mode " + str(mode))
        
        self.mode = mode

        if self.mode == VideoInputManager.Mode.ACTIVE_SPEAKER:
            self.active_speaker_id = active_speaker_id
            self.add_input_streams_if_needed([{"stream_type": VideoInputManager.StreamType.VIDEO, "user_id": active_speaker_id}])

        if self.mode == VideoInputManager.Mode.SCREENSHARE_AND_ACTIVE_SPEAKER:
            self.active_speaker_id = active_speaker_id
            self.add_input_streams_if_needed([
                {"stream_type": VideoInputManager.StreamType.VIDEO, "user_id": active_speaker_id}, 
                {"stream_type": VideoInputManager.StreamType.SCREENSHARE, "user_id": active_speaker_id}
            ])

    def wants_frames_for_user(self, user_id):
        if not self.wants_any_frames_callback():
            return False
    
        if self.mode == VideoInputManager.Mode.ACTIVE_SPEAKER and user_id != self.active_speaker_id:
            return False
        
        if self.mode == VideoInputManager.Mode.SCREENSHARE_AND_ACTIVE_SPEAKER and user_id != self.active_speaker_id:
            return False

        return True
    
    def new_frame_callback_for_stream(self, frame):
        bytes, stream_type, user_id = frame
        
        # Handle screenshare frame
        if stream_type == VideoInputManager.StreamType.SCREENSHARE:
            h, w = bytes.shape[:2]
            target_w, target_h = 1920, 1080
            scale = min(target_w/w, target_h/h)
            new_w, new_h = int(w * scale), int(h * scale)
            
            # Only create new black frame if dimensions changed or it doesn't exist
            if not hasattr(self, '_black_frame') or self._black_frame.shape != (1080, 1920, 3):
                self._black_frame = np.zeros((1080, 1920, 3), dtype=np.uint8)
            
            # Just use the existing black frame directly
            self.current_frame = self._black_frame
            # Zero out the frame (faster than creating new one)
            #self.current_frame.fill(0)
            
            x_offset = (target_w - new_w) // 2
            y_offset = (target_h - new_h) // 2
            
            bgr_image_scaled = cv2.resize(bytes, (new_w, new_h), 
                                        interpolation=cv2.INTER_NEAREST)
            
            self.current_frame[y_offset:y_offset+new_h, 
                             x_offset:x_offset+new_w] = bgr_image_scaled
            
            if self.last_speaker_frame is not None:
                self._overlay_speaker_bubble(self.last_speaker_frame)
        
        # Handle speaker video frame
        if stream_type == VideoInputManager.StreamType.VIDEO:
            self.last_speaker_frame = bytes.copy()  # Store the latest speaker frame
            self._overlay_speaker_bubble(self.last_speaker_frame)
        
        # Send the composited frame
        self.new_frame_callback(self.current_frame)

    def _overlay_speaker_bubble(self, speaker_frame):
        # Define speaker bubble dimensions and position
        bubble_width = 160  # Adjust as needed
        bubble_height = 90  # Adjust as needed
        x_offset = self.current_frame.shape[1] - bubble_width - 20  # 20px padding from right
        y_offset = 20  # 20px padding from top
        
        # Only resize if the dimensions don't match
        if speaker_frame.shape[1] != bubble_width or speaker_frame.shape[0] != bubble_height:
            print("resizing speaker frame to bubble size", speaker_frame.shape, "to", bubble_width, bubble_height)
            speaker_bubble = cv2.resize(speaker_frame, (bubble_width, bubble_height))
        else:
            speaker_bubble = speaker_frame
        
        # Overlay the speaker bubble
        self.current_frame[y_offset:y_offset+bubble_height, x_offset:x_offset+bubble_width] = speaker_bubble
