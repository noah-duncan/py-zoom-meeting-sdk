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
#include "rawdata/rawdata_video_source_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_meeting_video_interface_binding(nb::module_ &m) {
    // Video enums
    nb::enum_<VideoStatus>(m, "VideoStatus")
        .value("Video_ON", Video_ON)
        .value("Video_OFF", Video_OFF)
        .value("Video_Mute_ByHost", Video_Mute_ByHost)
        .export_values();

    nb::enum_<VideoConnectionQuality>(m, "VideoConnectionQuality")
        .value("VideoConnectionQuality_Unknown", VideoConnectionQuality_Unknown)
        .value("VideoConnectionQuality_Bad", VideoConnectionQuality_Bad)
        .value("VideoConnectionQuality_Normal", VideoConnectionQuality_Normal)
        .value("VideoConnectionQuality_Good", VideoConnectionQuality_Good)
        .export_values();

    nb::enum_<PinResult>(m, "PinResult")
        .value("PinResult_Success", PinResult_Success)
        .value("PinResult_Fail_NotEnoughUsers", PinResult_Fail_NotEnoughUsers)
        .value("PinResult_Fail_ToMuchPinnedUsers", PinResult_Fail_ToMuchPinnedUsers)
        .value("PinResult_Fail_UserCannotBePinned", PinResult_Fail_UserCannotBePinned)
        .value("PinResult_Fail_VideoModeDoNotSupport", PinResult_Fail_VideoModeDoNotSupport)
        .value("PinResult_Fail_NoPrivilegeToPin", PinResult_Fail_NoPrivilegeToPin)
        .value("PinResult_Fail_MeetingDoNotSupport", PinResult_Fail_MeetingDoNotSupport)
        .value("PinResult_Unknown", PinResult_Unknown)
        .export_values();

    nb::enum_<SpotlightResult>(m, "SpotlightResult")
        .value("SpotResult_Success", SpotResult_Success)
        .value("SpotResult_Fail_NotEnoughUsers", SpotResult_Fail_NotEnoughUsers)
        .value("SpotResult_Fail_ToMuchSpotlightedUsers", SpotResult_Fail_ToMuchSpotlightedUsers)
        .value("SpotResult_Fail_UserCannotBeSpotlighted", SpotResult_Fail_UserCannotBeSpotlighted)
        .value("SpotResult_Fail_UserWithoutVideo", SpotResult_Fail_UserWithoutVideo)
        .value("SpotResult_Fail_NoPrivilegeToSpotlight", SpotResult_Fail_NoPrivilegeToSpotlight)
        .value("SpotResult_Fail_UserNotSpotlighted", SpotResult_Fail_UserNotSpotlighted)
        .value("SpotResult_Unknown", SpotResult_Unknown)
        .export_values();

    // IMeetingVideoController
    nb::class_<IMeetingVideoController>(m, "IMeetingVideoController")
        .def("SetEvent", &IMeetingVideoController::SetEvent)
        .def("MuteVideo", &IMeetingVideoController::MuteVideo)
        .def("UnmuteVideo", &IMeetingVideoController::UnmuteVideo)
        .def("CanSpotlight", &IMeetingVideoController::CanSpotlight)
        .def("CanUnSpotlight", &IMeetingVideoController::CanUnSpotlight)
        .def("SpotlightVideo", &IMeetingVideoController::SpotlightVideo)
        .def("UnSpotlightVideo", &IMeetingVideoController::UnSpotlightVideo)
        .def("UnSpotlightAllVideos", &IMeetingVideoController::UnSpotlightAllVideos)
        .def("GetSpotlightedUserList", &IMeetingVideoController::GetSpotlightedUserList)
        .def("CanAskAttendeeToStartVideo", &IMeetingVideoController::CanAskAttendeeToStartVideo)
        .def("AskAttendeeToStartVideo", &IMeetingVideoController::AskAttendeeToStartVideo)
        .def("CanStopAttendeeVideo", &IMeetingVideoController::CanStopAttendeeVideo)
        .def("StopAttendeeVideo", &IMeetingVideoController::StopAttendeeVideo)
        .def("IsSupportFollowHostVideoOrder", &IMeetingVideoController::IsSupportFollowHostVideoOrder)
        .def("EnableFollowHostVideoOrder", &IMeetingVideoController::EnableFollowHostVideoOrder)
        .def("IsFollowHostVideoOrderOn", &IMeetingVideoController::IsFollowHostVideoOrderOn)
        .def("GetVideoOrderList", &IMeetingVideoController::GetVideoOrderList)
        .def("IsIncomingVideoStopped", &IMeetingVideoController::IsIncomingVideoStopped)
        .def("GetMeetingCameraHelper", &IMeetingVideoController::GetMeetingCameraHelper)
        .def("RevokeCameraControlPrivilege", &IMeetingVideoController::RevokeCameraControlPrivilege)
        .def("CanEnableAlphaChannelMode", &IMeetingVideoController::CanEnableAlphaChannelMode)
        .def("EnableAlphaChannelMode", &IMeetingVideoController::EnableAlphaChannelMode)
        .def("IsAlphaChannelModeEnabled", &IMeetingVideoController::IsAlphaChannelModeEnabled)
        .def("GetUserVideoSize", &IMeetingVideoController::GetUserVideoSize);

    // IMeetingVideoCtrlEvent
    nb::class_<IMeetingVideoCtrlEvent>(m, "IMeetingVideoCtrlEvent")
        .def("onUserVideoStatusChange", &IMeetingVideoCtrlEvent::onUserVideoStatusChange)
        .def("onSpotlightedUserListChangeNotification", &IMeetingVideoCtrlEvent::onSpotlightedUserListChangeNotification)
        .def("onHostRequestStartVideo", &IMeetingVideoCtrlEvent::onHostRequestStartVideo)
        .def("onActiveSpeakerVideoUserChanged", &IMeetingVideoCtrlEvent::onActiveSpeakerVideoUserChanged)
        .def("onActiveVideoUserChanged", &IMeetingVideoCtrlEvent::onActiveVideoUserChanged)
        .def("onHostVideoOrderUpdated", &IMeetingVideoCtrlEvent::onHostVideoOrderUpdated)
        .def("onLocalVideoOrderUpdated", &IMeetingVideoCtrlEvent::onLocalVideoOrderUpdated)
        .def("onFollowHostVideoOrderChanged", &IMeetingVideoCtrlEvent::onFollowHostVideoOrderChanged)
        .def("onUserVideoQualityChanged", &IMeetingVideoCtrlEvent::onUserVideoQualityChanged)
        .def("onVideoAlphaChannelStatusChanged", &IMeetingVideoCtrlEvent::onVideoAlphaChannelStatusChanged)
        .def("onCameraControlRequestReceived", &IMeetingVideoCtrlEvent::onCameraControlRequestReceived)
        .def("onCameraControlRequestResult", &IMeetingVideoCtrlEvent::onCameraControlRequestResult);
}