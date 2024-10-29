#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>

#include "zoom_sdk.h"

#include "meeting_service_interface.h"
#include "setting_service_interface.h"
#include "auth_service_interface.h"
#include "meeting_service_components/meeting_ai_companion_interface.h"
#include "meeting_service_components/meeting_recording_interface.h"
#include "meeting_service_components/meeting_audio_interface.h"
#include "meeting_service_components/meeting_reminder_ctrl_interface.h"
#include "meeting_service_components/meeting_breakout_rooms_interface_v2.h"
#include "meeting_service_components/meeting_sharing_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "meeting_service_components/meeting_smart_summary_interface.h"
#include "meeting_service_components/meeting_configuration_interface.h"
#include "meeting_service_components/meeting_video_interface.h"
#include "meeting_service_components/meeting_inmeeting_encryption_interface.h"
#include "meeting_service_components/meeting_participants_ctrl_interface.h"
#include "meeting_service_components/meeting_waiting_room_interface.h"
#include "meeting_service_components/meeting_webinar_interface.h"
#include "meeting_service_components/meeting_raw_archiving_interface.h"





#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_rawdata_audio_helper_interface_binding(nb::module_ &m) {
    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "IZoomSDKAudioRawDataDelegate")
        .def("onMixedAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived)
        .def("onOneWayAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived)
        .def("onShareAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived)
        .def("onOneWayInterpreterAudioRawDataReceived", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate::onOneWayInterpreterAudioRawDataReceived);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper>(m, "IZoomSDKAudioRawDataHelper")
        .def("subscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::subscribe)
        .def("unSubscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::unSubscribe)
        .def("setExternalAudioSource", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::setExternalAudioSource);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent>(m, "IZoomSDKVirtualAudioMicEvent")
        .def("onMicInitialize", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicInitialize)
        .def("onMicStartSend", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicStartSend)
        .def("onMicStopSend", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicStopSend)
        .def("onMicUninitialized", &ZOOM_SDK_NAMESPACE::IZoomSDKVirtualAudioMicEvent::onMicUninitialized);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender>(m, "IZoomSDKAudioRawDataSender")
        .def("send", [](ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataSender& self, nb::bytes data, int sample_rate, ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel channel) -> ZOOM_SDK_NAMESPACE::SDKError {
            return self.send((char*) data.c_str(), data.size(), sample_rate, channel);
        });
}