#include "ZoomSDKRendererDelegate.h"


ZoomSDKRendererDelegate::ZoomSDKRendererDelegate() {
    // For X11 Forwarding
    XInitThreads();

    m_socketServer.start();
}

void ZoomSDKRendererDelegate::onRawDataFrameReceived(YUVRawDataI420 *data)
{
    auto res = async(launch::async, [&]{
    });
}

void ZoomSDKRendererDelegate::writeToFile(const string &path, YUVRawDataI420 *data)
{

	std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::app);
	if (!file.is_open())
        return Log::error("failed to open video output file: " + path);

	file.write(data->GetBuffer(), data->GetBufferLen());

	file.close();
	file.flush();
}

void ZoomSDKRendererDelegate::setDir(const string &dir)
{
    m_dir = dir;
}

void ZoomSDKRendererDelegate::setFilename(const string &filename)
{
    m_filename = filename;
}
