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
#include "rawdata/rawdata_share_source_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_rawdata_share_helper_interface_binding(nb::module_ &m) {
    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKShareSourceHelper>(m, "IZoomSDKShareSourceHelper")
        .def("setExternalShareSource", &ZOOM_SDK_NAMESPACE::IZoomSDKShareSourceHelper::setExternalShareSource)
        .def("setSharePureAudioSource", &ZOOM_SDK_NAMESPACE::IZoomSDKShareSourceHelper::setSharePureAudioSource);

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSender>(m, "IZoomSDKShareAudioSender")
        .def("send", [](ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSender& self, nb::bytes data, int sample_rate, ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel channel) -> ZOOM_SDK_NAMESPACE::SDKError {
            return self.sendShareAudio((char*) data.c_str(), data.size(), sample_rate, channel);
        });

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKShareSender>(m, "IZoomSDKShareSender")
        .def("sendShareFrame", [](IZoomSDKShareSender& self, nb::bytes frameBuffer, int width, int height, FrameDataFormat format = FrameDataFormat_I420_FULL) -> SDKError {
            return self.sendShareFrame((char*)frameBuffer.c_str(), width, height, frameBuffer.size(), format);
        });

    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKShareSource>(m, "IZoomSDKShareSource")
        .def("onStartSend", &ZOOM_SDK_NAMESPACE::IZoomSDKShareSource::onStartSend)
        .def("onStopSend", &ZOOM_SDK_NAMESPACE::IZoomSDKShareSource::onStopSend);
    
    nb::class_<ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSource>(m, "IZoomSDKShareAudioSource")
        .def("onStartSendAudio", &ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSource::onStartSendAudio)
        .def("onStopSendAudio", &ZOOM_SDK_NAMESPACE::IZoomSDKShareAudioSource::onStopSendAudio);
}