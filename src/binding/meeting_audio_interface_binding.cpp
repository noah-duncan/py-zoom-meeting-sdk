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

void init_meeting_audio_interface_binding(nb::module_ &m) {
    // Audio enums
    nb::enum_<AudioStatus>(m, "AudioStatus")
        .value("Audio_None", Audio_None)
        .value("Audio_Muted", Audio_Muted)
        .value("Audio_UnMuted", Audio_UnMuted)
        .value("Audio_Muted_ByHost", Audio_Muted_ByHost)
        .value("Audio_UnMuted_ByHost", Audio_UnMuted_ByHost)
        .value("Audio_MutedAll_ByHost", Audio_MutedAll_ByHost)
        .value("Audio_UnMutedAll_ByHost", Audio_UnMutedAll_ByHost)
        .export_values();

    nb::enum_<AudioType>(m, "AudioType")
        .value("AUDIOTYPE_NONE", AUDIOTYPE_NONE)
        .value("AUDIOTYPE_VOIP", AUDIOTYPE_VOIP)
        .value("AUDIOTYPE_PHONE", AUDIOTYPE_PHONE)
        .value("AUDIOTYPE_UNKNOWN", AUDIOTYPE_UNKNOWN)
        .export_values();

    // IRequestStartAudioHandler
    nb::class_<IRequestStartAudioHandler>(m, "IRequestStartAudioHandler")
        .def("GetReqFromUserId", &IRequestStartAudioHandler::GetReqFromUserId)
        .def("Ignore", &IRequestStartAudioHandler::Ignore)
        .def("Accept", &IRequestStartAudioHandler::Accept)
        .def("Cancel", &IRequestStartAudioHandler::Cancel);

    // IUserAudioStatus
    nb::class_<IUserAudioStatus>(m, "IUserAudioStatus")
        .def("GetUserId", &IUserAudioStatus::GetUserId)
        .def("GetStatus", &IUserAudioStatus::GetStatus)
        .def("GetAudioType", &IUserAudioStatus::GetAudioType);

    // IMeetingAudioCtrlEvent
    nb::class_<IMeetingAudioCtrlEvent>(m, "IMeetingAudioCtrlEvent")
        .def("onUserAudioStatusChange", &IMeetingAudioCtrlEvent::onUserAudioStatusChange)
        .def("onUserActiveAudioChange", &IMeetingAudioCtrlEvent::onUserActiveAudioChange)
        .def("onHostRequestStartAudio", &IMeetingAudioCtrlEvent::onHostRequestStartAudio)
        .def("onJoin3rdPartyTelephonyAudio", &IMeetingAudioCtrlEvent::onJoin3rdPartyTelephonyAudio)
        .def("onMuteOnEntryStatusChange", &IMeetingAudioCtrlEvent::onMuteOnEntryStatusChange);

    // IMeetingAudioController
    nb::class_<IMeetingAudioController>(m, "IMeetingAudioController")
        .def("SetEvent", &IMeetingAudioController::SetEvent)
        .def("JoinVoip", &IMeetingAudioController::JoinVoip)
        .def("LeaveVoip", &IMeetingAudioController::LeaveVoip)
        .def("MuteAudio", &IMeetingAudioController::MuteAudio, nb::arg("userid"), nb::arg("allowUnmuteBySelf") = true)
        .def("UnMuteAudio", &IMeetingAudioController::UnMuteAudio)
        .def("CanUnMuteBySelf", &IMeetingAudioController::CanUnMuteBySelf)
        .def("CanEnableMuteOnEntry", &IMeetingAudioController::CanEnableMuteOnEntry)
        .def("EnableMuteOnEntry", &IMeetingAudioController::EnableMuteOnEntry)
        .def("IsMuteOnEntryEnabled", &IMeetingAudioController::IsMuteOnEntryEnabled)
        .def("EnablePlayChimeWhenEnterOrExit", &IMeetingAudioController::EnablePlayChimeWhenEnterOrExit)
        .def("StopIncomingAudio", &IMeetingAudioController::StopIncomingAudio)
        .def("IsIncomingAudioStopped", &IMeetingAudioController::IsIncomingAudioStopped)
        .def("Is3rdPartyTelephonyAudioOn", &IMeetingAudioController::Is3rdPartyTelephonyAudioOn);
}