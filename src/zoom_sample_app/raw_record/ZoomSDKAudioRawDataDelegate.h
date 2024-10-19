
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include "../util/Log.h"
#include "../util/SocketServer.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKAudioRawDataDelegate : public IZoomSDKAudioRawDataDelegate {
    SocketServer server;

    string m_dir = "out";
    string m_filename = "test.pcm";
    bool m_useMixedAudio;
    bool m_transcribe;

    void writeToFile(const string& path, AudioRawData* data);
public:
    ZoomSDKAudioRawDataDelegate(bool useMixedAudio, bool transcribe);
    void setDir(const string& dir);
    void setFilename(const string& filename);

    void onMixedAudioRawDataReceived(AudioRawData* data) override;
    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override;
    void onShareAudioRawDataReceived(AudioRawData* data) override;

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {};
};



#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
