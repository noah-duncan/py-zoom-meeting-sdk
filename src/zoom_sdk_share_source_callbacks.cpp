#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/vector.h>
#include "zoom_sdk.h"
#include "zoom_sdk_def.h"
#include "rawdata/rawdata_video_source_helper_interface.h"
#include "rawdata/rawdata_share_source_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"

#include <iostream>
#include <functional>
#include <memory>
#include <vector>

namespace nb = nanobind;
using namespace ZOOMSDK;
using namespace std;

class ShareSourceCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKShareSource {
private:
    function<void(IZoomSDKShareSender *pSender)> m_onStartSendCallback;
    function<void()> m_onStopSendCallback;

public:
    ShareSourceCallbacks(
        const function<void(IZoomSDKShareSender *pSender)>& onStartSendCallback = nullptr,
        const function<void()>& onStopSendCallback = nullptr
    ) : m_onStartSendCallback(onStartSendCallback),
        m_onStopSendCallback(onStopSendCallback) {}

    void onStartSend(IZoomSDKShareSender *pSender) override {
        if (m_onStartSendCallback)
            m_onStartSendCallback(pSender);
    }

    void onStopSend() override {
        if (m_onStopSendCallback)
            m_onStopSendCallback();
    }
};

class ShareAudioCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSource {
private:
    function<void(IZoomSDKShareAudioSender *pSender)> m_onStartSendAudioCallback;
    function<void()> m_onStopSendAudioCallback;

public:
    ShareAudioCallbacks(
        const function<void(IZoomSDKShareAudioSender  *pSender)>& onStartSendAudioCallback = nullptr,
        const function<void()>& onStopSendAudioCallback = nullptr
    ): m_onStartSendAudioCallback(onStartSendAudioCallback),
        m_onStopSendAudioCallback(onStopSendAudioCallback) {}

    void onStartSendAudio(IZoomSDKShareAudioSender *pSender) override {
        if (m_onStartSendAudioCallback)
            m_onStartSendAudioCallback(pSender);
    }

    void onStopSendAudio() override {
        if (m_onStopSendAudioCallback)
            m_onStopSendAudioCallback();
    }
};


void init_zoom_sdk_share_source_callbacks(nb::module_ &m) {
    nb::class_<ShareSourceCallbacks, IZoomSDKShareSource>(m, "ShareSourceCallbacks")
        .def(nb::init<
            function<void(IZoomSDKShareSender *pSender)>&,
            function<void()>&
        >(),
            nb::arg("onStartSendCallback") = nullptr,
            nb::arg("onStopSendCallback") = nullptr
        );

    nb::class_<ShareAudioCallbacks, IZoomSDKShareAudioSource>(m, "ShareAudioCallbacks")
        .def(nb::init<
            function<void(IZoomSDKShareAudioSender *pSender)>&,
            function<void()>&
        >(),
            nb::arg("onStartSendAudioCallback") = nullptr,
            nb::arg("onStopSendAudioCallback") = nullptr
        );
}