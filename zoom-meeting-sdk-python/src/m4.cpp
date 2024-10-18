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

class ZoomSDKZoomSDKVirtualAudioMicEventPassThrough : public ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent {
private:
    function<void(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender*)> m_on_mic_initialize_callback;
    function<void()> m_on_mic_start_send_callback;
public:
    ZoomSDKZoomSDKVirtualAudioMicEventPassThrough(){};

    void setOnMicInitialize(const function<void(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender*)> & on_mic_initialize_callback)
    {
        m_on_mic_initialize_callback = on_mic_initialize_callback;
    }

    void setOnMicStartSend(const function<void()> & on_mic_start_send_callback)
    {
        m_on_mic_start_send_callback = on_mic_start_send_callback;
    }

    void onMicInitialize(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender* pSender) override {
        if (m_on_mic_initialize_callback)
            m_on_mic_initialize_callback(pSender);
    }

    void onMicStartSend() override {
        if (m_on_mic_start_send_callback)
            m_on_mic_start_send_callback();
    }

    void onMicStopSend() override {

    }

    void onMicUninitialized() override {

    }
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

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent>(m, "IZoomSDKVirtualAudioMicEvent")
    .def("onMicInitialize", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicInitialize)
    .def("onMicStartSend", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicStartSend)
    .def("onMicStopSend", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicStopSend)
    .def("onMicUninitialized", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicUninitialized);

    nb::class_<ZoomSDKZoomSDKVirtualAudioMicEventPassThrough, ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent>(m, "ZoomSDKZoomSDKVirtualAudioMicEventPassThrough")
    .def(nb::init<>())
    .def("setOnMicInitialize", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::setOnMicInitialize)
    .def("setOnMicStartSend", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::setOnMicStartSend)
    .def("onMicInitialize", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::onMicInitialize)
    .def("onMicStartSend", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::onMicStartSend)
    .def("onMicStopSend", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::onMicStopSend)
    .def("onMicUninitialized", &ZoomSDKZoomSDKVirtualAudioMicEventPassThrough::onMicUninitialized);    

    nb::enum_<ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel>(m, "ZoomSDKAudioChannel")
        .value("ZoomSDKAudioChannel_Mono", ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel_Mono)
        .value("ZoomSDKAudioChannel_Stereo", ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel_Stereo)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender>(m, "IZoomSDKAudioRawDataSender")
    .def("send", [](ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender& self, nb::bytes data, int sample_rate, ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel channel) -> ZOOM_SDK_NAMESPACE::SDKError {
        return self.send((char*) data.c_str(), data.size(), sample_rate, channel);
     });

//	virtual SDKError send(char* data, unsigned int data_length, int sample_rate, ZoomSDKAudioChannel channel = ZoomSDKAudioChannel_Mono) = 0;

    //.def("send", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender::send);
}