import zoom_meeting_sdk_python as zoom

class RawAudioDelegate(zoom.IZoomSDKAudioRawDataDelegate):
    def __init__(self):
        self.use_mixed_audio = False
        self.transcribe = False

        self.directory = "out"
        self.filename = "test.pcm"

    def write_to_file(self, path, data):
        print("cu")

    def onMixedAudioRawDataReceived(self, data):
        print("bud")

    def onOneWayAudioRawDataReceived(self, data, node_id):
        print("received from ", node_id)
        

    def onShareAudioRawDataReceived(self, data):
        print("must")

    def onOneWayInterpreterAudioRawDataReceived(self, data, language_name):
        print("bust")