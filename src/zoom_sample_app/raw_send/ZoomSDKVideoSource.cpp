
#include "ZoomSDKVideoSource.h"

ZoomSDKVideoSource::ZoomSDKVideoSource() {}

bool ZoomSDKVideoSource::isReady() {
    return m_isReady;
}

IZoomSDKVideoSender *ZoomSDKVideoSource::getSender() const {
    return m_videoSender;
}

void ZoomSDKVideoSource::onInitialize(IZoomSDKVideoSender *sender,IList <VideoSourceCapability> *support_cap_list,
                                      VideoSourceCapability& suggest_cap){
    m_videoSender = sender;
    Log::success("onInitialize");

}

void ZoomSDKVideoSource::onPropertyChange(IList <VideoSourceCapability> *support_cap_list,
                                          VideoSourceCapability suggest_cap) {
    m_width = suggest_cap.width;
    m_height = suggest_cap.height;
}

void ZoomSDKVideoSource::onStartSend() {
    Log::info("sender is ready");
    m_isReady = true;
}

void ZoomSDKVideoSource::onStopSend() {
    Log::info("sender stopped");
    m_isReady = false;
}

void ZoomSDKVideoSource::onUninitialized() {
    m_videoSender = nullptr;
}

void ZoomSDKVideoSource::setWidth(const unsigned int& width) {
    m_width = width;
}

void ZoomSDKVideoSource::setHeight(const unsigned int& height) {
    m_height = height;
}