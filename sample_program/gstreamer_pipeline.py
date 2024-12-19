import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib
import time
import os

class GstreamerPipeline:
    def __init__(self, on_new_sample_callback):
        self.on_new_sample_callback = on_new_sample_callback
        self.pipeline = None
        self.appsrc = None
        self.recording_active = False

        self.audio_appsrc = None
        self.audio_recording_active = False

        self.start_time_ns = None  # Will be set on first frame/audio sample

        # Initialize GStreamer
        Gst.init(None)

        self.queue_drops = {f'q{i}': 0 for i in range(1, 8)}
        self.last_reported_drops = {f'q{i}': 0 for i in range(1, 8)}

    def on_new_sample_from_appsink(self, sink):
        """Handle new samples from the appsink"""
        sample = sink.emit('pull-sample')
        if sample:
            buffer = sample.get_buffer()
            data = buffer.extract_dup(0, buffer.get_size())
            self.on_new_sample_callback(data)
            return Gst.FlowReturn.OK
        return Gst.FlowReturn.ERROR
    
    def setup_gstreamer_pipeline(self):
        """Initialize GStreamer pipeline for combined MP4 recording with audio and video"""
        self.start_time_ns = None

# made shit slow             'x264enc tune=zerolatency speed-preset=ultrafast qp-min=17 rc-lookahead=30 subme=8 ref=4 bframes=0 tune=stillimage pass=qual ! '
# made shit fast             'x264enc speed-preset=ultrafast  bframes=0 tune=stillimage pass=qual ! '
# key thing seems to be speed-preset

        reduce_video_resolution_pipeline_str = (
            'appsrc name=video_source do-timestamp=false stream-type=0 format=time ! '
            'queue name=q1 max-size-buffers=1000 max-size-bytes=0 max-size-time=0 ! '
            'videoconvert ! '
            'videoscale method=2 ! video/x-raw,width=1080,height=720 ! '
            'videorate ! '
            'queue name=q2 max-size-buffers=1000 max-size-bytes=0 max-size-time=0 ! '
            'x264enc tune=zerolatency speed-preset=ultrafast qp-min=17 rc-lookahead=30 subme=8 ref=4 bframes=0 tune=stillimage pass=qual ! '
            'queue name=q3 max-size-buffers=1000 max-size-bytes=0 max-size-time=0 ! '
            'mp4mux name=muxer ! queue name=q4 ! appsink name=sink emit-signals=true sync=false drop=false '
            'appsrc name=audio_source do-timestamp=false stream-type=0 format=time ! '
            'queue name=q5 leaky=downstream ! '
            'audioconvert ! '
            'audiorate ! '
            'queue name=q6 leaky=downstream ! '
            'voaacenc bitrate=128000 ! '
            'queue name=q7 leaky=downstream ! '
            'muxer. '
        )
        
        self.pipeline = Gst.parse_launch(reduce_video_resolution_pipeline_str)
        
        # Get both appsrc elements
        self.appsrc = self.pipeline.get_by_name('video_source')
        self.audio_appsrc = self.pipeline.get_by_name('audio_source')
        
        # Configure video appsrc
        video_caps = Gst.Caps.from_string('video/x-raw,format=BGR,width=1728,height=1116,framerate=30/1')
        self.appsrc.set_property('caps', video_caps)
        self.appsrc.set_property('format', Gst.Format.TIME)
        self.appsrc.set_property('is-live', True)
        self.appsrc.set_property('do-timestamp', False)
        self.appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM
        self.appsrc.set_property('block', True)  # This helps with synchronization

        # Configure audio appsrc
        audio_caps = Gst.Caps.from_string(
            'audio/x-raw,format=S16LE,channels=1,rate=32000,layout=interleaved'
        )
        self.audio_appsrc.set_property('caps', audio_caps)
        self.audio_appsrc.set_property('format', Gst.Format.TIME)
        self.audio_appsrc.set_property('is-live', True)
        self.audio_appsrc.set_property('do-timestamp', False)
        self.audio_appsrc.set_property('stream-type', 0)  # GST_APP_STREAM_TYPE_STREAM
        self.audio_appsrc.set_property('block', True)  # This helps with synchronization

        # Set up bus
        bus = self.pipeline.get_bus()
        bus.add_signal_watch()
        bus.connect('message', self.on_pipeline_message)

        # Connect to the sink element
        sink = self.pipeline.get_by_name('sink')
        sink.connect("new-sample", self.on_new_sample_from_appsink)
        
        # Start the pipeline
        self.pipeline.set_state(Gst.State.PLAYING)

        self.recording_active = True
        self.audio_recording_active = True

        # Start statistics monitoring
        self.monitoring_active = True
        GLib.timeout_add_seconds(15, self.monitor_pipeline_stats)

        # Connect drop signals for all queues
        for i in range(1, 8):
            queue = self.pipeline.get_by_name(f'q{i}')
            if queue:
                queue.connect('overrun', self.on_queue_overrun, f'q{i}')

    def on_pipeline_message(self, bus, message):
        """Handle pipeline messages"""
        t = message.type
        if t == Gst.MessageType.ERROR:
            err, debug = message.parse_error()
            print(f"GStreamer Error: {err}, Debug: {debug}")
        elif t == Gst.MessageType.EOS:
            print(f"GStreamer pipeline reached end of stream")

    def monitor_pipeline_stats(self):
        """Periodically print pipeline statistics"""
        if not self.recording_active:
            return False
        
        try:
            # Print dropped buffer counts since last check
            print("\nDropped Buffers Since Last Check:")
            for queue_name in self.queue_drops:
                drops = self.queue_drops[queue_name] - self.last_reported_drops[queue_name]
                if drops > 0:
                    print(f"  {queue_name}: {drops} buffers dropped")
                self.last_reported_drops[queue_name] = self.queue_drops[queue_name]

        except Exception as e:
            print(f"Error getting pipeline stats: {e}")
        
        return True  # Continue timer

    def on_queue_overrun(self, queue, queue_name):
        """Callback for when a queue drops buffers"""
        self.queue_drops[queue_name] += 1
        return True
    
    def on_mixed_audio_raw_data_received_callback(self, data):
        if not self.audio_recording_active or not self.audio_appsrc or not self.recording_active or not self.appsrc:
            return

        try:
            current_time_ns = time.time_ns()
            buffer_bytes = data.GetBuffer()
            buffer = Gst.Buffer.new_wrapped(buffer_bytes)
            
            # Initialize start time if not set
            if self.start_time_ns is None:
                self.start_time_ns = current_time_ns
            
            # Calculate timestamp relative to same start time as video
            buffer.pts = current_time_ns - self.start_time_ns
            
            ret = self.audio_appsrc.emit('push-buffer', buffer)
            if ret != Gst.FlowReturn.OK:
                print(f"Warning: Failed to push audio buffer to pipeline: {ret}")
        except Exception as e:
            print(f"Error processing audio data: {e}")

    def wants_any_video_frames(self):
        if not self.audio_recording_active or not self.audio_appsrc or not self.recording_active or not self.appsrc:
            return False

        return True
    
    def on_new_video_frame(self, frame):
        try:
            current_time_ns = time.time_ns()
                        
            # Initialize start time if not set
            if self.start_time_ns is None:
                self.start_time_ns = current_time_ns

            # Calculate buffer timestamp relative to start time
            buffer_pts = current_time_ns - self.start_time_ns
            
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
            print(f"Error processing video frame: {e}")

    def cleanup(self):
        print("Shutting down GStreamer pipeline...")
        if self.appsrc:
            self.appsrc.emit('end-of-stream')
        if self.audio_appsrc:
            self.audio_appsrc.emit('end-of-stream')
        
        bus = self.pipeline.get_bus()
        msg = bus.timed_pop_filtered(
            Gst.CLOCK_TIME_NONE,
            Gst.MessageType.EOS | Gst.MessageType.ERROR
        )
        
        if msg and msg.type == Gst.MessageType.ERROR:
            err, debug = msg.parse_error()
            print(f"Error during pipeline shutdown: {err}, {debug}")
        
        self.pipeline.set_state(Gst.State.NULL)
        print("GStreamer pipeline shut down")