#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "util/SocketServer.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"


#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

class PyIZoomSDKAudioRawDataDelegate : ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
    NB_TRAMPOLINE(ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate, 4);

    void onMixedAudioRawDataReceived(AudioRawData* data_) override {
        NB_OVERRIDE_PURE(onMixedAudioRawDataReceived, data_);
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) override {
        NB_OVERRIDE_PURE(onOneWayAudioRawDataReceived, data_, node_id);
    }

    void onShareAudioRawDataReceived(AudioRawData* data_) override {
        NB_OVERRIDE_PURE(onShareAudioRawDataReceived, data_);
    }

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {
         NB_OVERRIDE_PURE(onOneWayInterpreterAudioRawDataReceived, data_, pLanguageName);
    }

};

class ZoomSDKAudioRawDataDelegatePassThrough : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    function<void(AudioRawData*, uint32_t)> m_on_one_way_audio_raw_data_received_callback;
public:
    ZoomSDKAudioRawDataDelegatePassThrough(){};

    void onMixedAudioRawDataReceived(AudioRawData* data) override {

    };

    void setOnOneWayAudioRawDataReceived(const function<void(AudioRawData*, uint32_t)> & on_one_way_audio_raw_data_received_callback)
    {
        m_on_one_way_audio_raw_data_received_callback = on_one_way_audio_raw_data_received_callback;
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override {
        std::cout << "brolo" << std::endl;
        if (m_on_one_way_audio_raw_data_received_callback)
            m_on_one_way_audio_raw_data_received_callback(data, node_id);
    };
    void onShareAudioRawDataReceived(AudioRawData* data) override {

    };

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {};
};

void init_m4(nb::module_ &m) {
    nb::class_<AudioRawData>(m, "AudioRawData")
    .def("GetBuffer", &AudioRawData::GetBuffer)
    .def("GetBufferLen", &AudioRawData::GetBufferLen);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate, PyIZoomSDKAudioRawDataDelegate>(m, "IZoomSDKAudioRawDataDelegate")
    .def(nb::init<>())
    .def("onMixedAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived)
    .def("onOneWayAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived)
    .def("onShareAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived)
    .def("onOneWayInterpreterAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayInterpreterAudioRawDataReceived);

    nb::class_<ZoomSDKAudioRawDataDelegatePassThrough, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegatePassThrough")
    .def(nb::init<>())
    .def("setOnOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::setOnOneWayAudioRawDataReceived)
    .def("onMixedAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onMixedAudioRawDataReceived)
    .def("onOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onOneWayAudioRawDataReceived)
    .def("onShareAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onShareAudioRawDataReceived)
    .def("onOneWayInterpreterAudioRawDataReceived", &ZoomSDKAudioRawDataDelegatePassThrough::onOneWayInterpreterAudioRawDataReceived);

}