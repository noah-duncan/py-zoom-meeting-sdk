#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"

#include "util/SocketServer.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"


#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

class ZoomSDKAudioRawDataDelegatePassThrough : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    function<void(AudioRawData*, uint32_t)> m_on_one_way_audio_raw_data_received_callback;
    bool send_m_on_one_way_audio_raw_data_received_callback = true;
public:
    ZoomSDKAudioRawDataDelegatePassThrough(){};

    void onMixedAudioRawDataReceived(AudioRawData* data) override {

    };

    void setOnOneWayAudioRawDataReceived(const function<void(AudioRawData*, uint32_t)> & on_one_way_audio_raw_data_received_callback)
    {
        m_on_one_way_audio_raw_data_received_callback = on_one_way_audio_raw_data_received_callback;
    }

    void setSendOnOneWayAudioRawDataReceived(bool send)
    {
        send_m_on_one_way_audio_raw_data_received_callback = false;
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override {
        if (m_on_one_way_audio_raw_data_received_callback && send_m_on_one_way_audio_raw_data_received_callback)
            m_on_one_way_audio_raw_data_received_callback(data, node_id);
    };

    void onShareAudioRawDataReceived(AudioRawData* data) override {

    };

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {};
};

void init_m4(nb::module_ &m) {

    nb::class_<AudioRawData>(m, "AudioRawData")
    .def("GetBuffer", [](AudioRawData& self) -> nb::bytes {
        return nb::bytes(self.GetBuffer(), self.GetBufferLen());
     })
    .def("GetBufferLen", &AudioRawData::GetBufferLen)
    .def("GetSampleRate", &AudioRawData::GetSampleRate)
    .def("GetChannelNum", &AudioRawData::GetChannelNum);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "IZoomSDKAudioRawDataDelegate")
    .def("onMixedAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived)
    .def("onOneWayAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived)
    .def("onShareAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived)
    .def("onOneWayInterpreterAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayInterpreterAudioRawDataReceived);

    nb::class_<ZoomSDKAudioRawDataDelegatePassThrough, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegatePassThrough")
    .def(nb::init<>())
    .def("setOnOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::setOnOneWayAudioRawDataReceived)
    .def("setSendOnOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::setSendOnOneWayAudioRawDataReceived)
    .def("onMixedAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onMixedAudioRawDataReceived)
    .def("onOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onOneWayAudioRawDataReceived)
    .def("onShareAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onShareAudioRawDataReceived)
    .def("onOneWayInterpreterAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onOneWayInterpreterAudioRawDataReceived);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper>(m, "IZoomSDKAudioRawDataHelper")
    .def("subscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::subscribe)
    .def("unSubscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::unSubscribe)
    .def("setExternalAudioSource", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::setExternalAudioSource);

    m.def("GetAudioRawdataHelper", []() -> ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper* {
        return ZOOM_SDK_NAMESPACE::GetAudioRawdataHelper();
    }, nb::rv_policy::take_ownership);
}