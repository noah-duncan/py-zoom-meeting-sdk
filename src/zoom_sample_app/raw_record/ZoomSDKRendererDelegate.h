
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <future>
#include <glib.h>

#include <X11/Xlib.h>

#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "../util/SocketServer.h"
#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKRendererDelegate : public IZoomSDKRendererDelegate {
    const string c_window = "Face_Detection";
    string m_dir = "out";
    string m_filename = "meeting-video.yuv";

    unsigned int m_frameCount = 0;
    double m_scale=3;
    double m_fx = 1/m_scale;

    SocketServer m_socketServer;

public:
    ZoomSDKRendererDelegate();

    void writeToFile(const string& path, YUVRawDataI420* data);

    void setDir(const string& dir);
    void setFilename(const string& filename);

    void onRawDataFrameReceived(YUVRawDataI420* data) override;
    void onRawDataStatusChanged(RawDataStatus status) override {};
    void onRendererBeDestroyed() override {};
};


#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
