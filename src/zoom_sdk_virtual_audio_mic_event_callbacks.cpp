#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"


#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;
using namespace std;

class ZoomSDKVirtualAudioMicEventCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent {
private:
    function<void(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender*)> m_onMicInitializeCallback;
    function<void()> m_onMicStartSendCallback;
    function<void()> m_onMicStopSendCallback;
    function<void()> m_onMicUninitializedCallback;

public:
    ZoomSDKVirtualAudioMicEventCallbacks(
        const function<void(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender*)>& onMicInitializeCallback = nullptr,
        const function<void()>& onMicStartSendCallback = nullptr,
        const function<void()>& onMicStopSendCallback = nullptr,
        const function<void()>& onMicUninitializedCallback = nullptr
    ) : m_onMicInitializeCallback(onMicInitializeCallback),
        m_onMicStartSendCallback(onMicStartSendCallback),
        m_onMicStopSendCallback(onMicStopSendCallback),
        m_onMicUninitializedCallback(onMicUninitializedCallback) {}

    void onMicInitialize(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender* pSender) override {
        if (m_onMicInitializeCallback)
            m_onMicInitializeCallback(pSender);
    }

    void onMicStartSend() override {
        if (m_onMicStartSendCallback)
            m_onMicStartSendCallback();
    }

    void onMicStopSend() override {
        if (m_onMicStopSendCallback)
            m_onMicStopSendCallback();
    }

    void onMicUninitialized() override {
        if (m_onMicUninitializedCallback)
            m_onMicUninitializedCallback();
    }
};

void init_zoom_sdk_virtual_audio_mic_event_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKVirtualAudioMicEventCallbacks, ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent>(m, "ZoomSDKVirtualAudioMicEventCallbacks")
        .def(
            nb::init<
                function<void(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender*)>&,
                function<void()>&,
                function<void()>&,
                function<void()>&
            >(),
            nb::arg("onMicInitializeCallback") = nullptr,
            nb::arg("onMicStartSendCallback") = nullptr,
            nb::arg("onMicStopSendCallback") = nullptr,
            nb::arg("onMicUninitializedCallback") = nullptr
        );
}