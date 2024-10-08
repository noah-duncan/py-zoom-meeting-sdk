import zoom_meeting_sdk_python as zoom

# class RawAudioDelegate(zoom.IZoomSDKAudioRawDataDelegateConstructorOnly):
#     def __init__(self):
#         print("garlic")
#         self.use_mixed_audio = False
#         self.transcribe = False

#         self.directory = "out"
#         self.filename = "test.pcm"

#     def write_to_file(self, path, data):
#         print("cu")

#     def onMixedAudioRawDataReceived(self, data):
#         pass
#         #print("bud")

#     def onOneWayAudioRawDataReceived(self, data, node_id):
#         print("received from ", node_id)
        

#     def onShareAudioRawDataReceived(self, data):
#         print("must")

#     def onOneWayInterpreterAudioRawDataReceived(self, data, language_name):
#         print("bust")


# class ShihTzu(zoom.Dog):
#      def bark(self):
#              return "ggg"

"""
cd zoom-meeting-sdk-python/
pip install nanobind scikit-build-core[pyproject]
pip install --no-build-isolation -ve .
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/python-zoom-linux-sdk/lib/zoomsdk
"""