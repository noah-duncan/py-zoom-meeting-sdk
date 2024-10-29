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

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_setting_service_interface_binding(nb::module_ &m) {
    nb::class_<ISettingService>(m, "ISettingService")
        .def("GetGeneralSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetGeneralSettings, nb::rv_policy::reference)
        .def("GetAudioSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetAudioSettings, nb::rv_policy::reference)
        .def("GetVideoSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetVideoSettings, nb::rv_policy::reference)
        .def("GetRecordingSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetRecordingSettings, nb::rv_policy::reference)
        .def("GetStatisticSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetStatisticSettings, nb::rv_policy::reference)
        .def("GetShareSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetShareSettings, nb::rv_policy::reference);

    nb::class_<IAudioSettingContext>(m, "IAudioSettingContext")
        .def("EnableAutoJoinAudio", &IAudioSettingContext::EnableAutoJoinAudio);
}