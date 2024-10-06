#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

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
#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"
#include "raw_record/ZoomSDKAudioRawDataDelegate.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

//	virtual SDKError subscribe(IZoomSDKAudioRawDataDelegate* pDelegate, bool bWithInterpreters = false) = 0;

void init_m2(nb::module_ &m) {
    nb::class_<IZoomSDKAudioRawDataHelper>(m, "IZoomSDKAudioRawDataHelper")
    .def("subscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::subscribe)
    .def("subscribe2", [](IZoomSDKAudioRawDataHelper & self, IZoomSDKAudioRawDataDelegate& pDelegate, bool bWithInterpreters = false) -> SDKError {
        return self.subscribe(&pDelegate, bWithInterpreters);
    }, nb::rv_policy::take_ownership)
    .def("unSubscribe", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::unSubscribe)
    .def("setExternalAudioSource", &ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper::setExternalAudioSource);

    m.def("GetAudioRawdataHelper", []() -> ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper* {
        return GetAudioRawdataHelper();
    }, nb::rv_policy::take_ownership);
/*
    // Not part of SDK
    nb::class_<ZoomSDKAudioRawDataDelegate, IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegate")
    .def(nb::init<bool, bool>())
    .def("setDir", &ZoomSDKAudioRawDataDelegate::setDir)
    .def("setFilename", &ZoomSDKAudioRawDataDelegate::setFilename)
    .def("onMixedAudioRawDataReceived", &ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived)
    .def("onOneWayAudioRawDataReceived", &ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived)
    .def("onShareAudioRawDataReceived", &ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived)
    .def("onOneWayInterpreterAudioRawDataReceived", &ZoomSDKAudioRawDataDelegate::onOneWayInterpreterAudioRawDataReceived);
*/
    nb::class_<IAudioSettingContext>(m, "IAudioSettingContext")
    .def("EnableAutoJoinAudio", &IAudioSettingContext::EnableAutoJoinAudio);
}