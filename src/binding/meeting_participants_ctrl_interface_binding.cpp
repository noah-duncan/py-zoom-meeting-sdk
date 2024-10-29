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

void init_meeting_participants_ctrl_interface_binding(nb::module_ &m) {
    nb::class_<ZOOM_SDK_NAMESPACE::IMeetingParticipantsController>(m, "IMeetingParticipantsController")
        .def("SetEvent", &IMeetingParticipantsController::SetEvent)
        .def("GetParticipantsList", &IMeetingParticipantsController::GetParticipantsList)
        .def("GetUserByUserID", &IMeetingParticipantsController::GetUserByUserID, nb::rv_policy::reference)
        .def("GetMySelfUser", &IMeetingParticipantsController::GetMySelfUser, nb::rv_policy::reference)
        .def("LowerAllHands", &IMeetingParticipantsController::LowerAllHands)
        .def("ChangeUserName", &IMeetingParticipantsController::ChangeUserName)
        .def("LowerHand", &IMeetingParticipantsController::LowerHand)
        .def("RaiseHand", &IMeetingParticipantsController::RaiseHand)
        .def("MakeHost", &IMeetingParticipantsController::MakeHost)
        .def("CanbeCohost", &IMeetingParticipantsController::CanbeCohost)
        .def("AssignCoHost", &IMeetingParticipantsController::AssignCoHost)
        .def("RevokeCoHost", &IMeetingParticipantsController::RevokeCoHost)
        .def("ExpelUser", &IMeetingParticipantsController::ExpelUser)
        .def("IsSelfOriginalHost", &IMeetingParticipantsController::IsSelfOriginalHost)
        .def("ReclaimHost", &IMeetingParticipantsController::ReclaimHost)
        .def("CanReclaimHost", &IMeetingParticipantsController::CanReclaimHost)
        .def("ReclaimHostByHostKey", &IMeetingParticipantsController::ReclaimHostByHostKey)
        .def("AllowParticipantsToRename", &IMeetingParticipantsController::AllowParticipantsToRename)
        .def("IsParticipantsRenameAllowed", &IMeetingParticipantsController::IsParticipantsRenameAllowed)
        .def("AllowParticipantsToUnmuteSelf", &IMeetingParticipantsController::AllowParticipantsToUnmuteSelf)
        .def("IsParticipantsUnmuteSelfAllowed", &IMeetingParticipantsController::IsParticipantsUnmuteSelfAllowed)
        .def("AskAllToUnmute", &IMeetingParticipantsController::AskAllToUnmute)
        .def("AllowParticipantsToStartVideo", &IMeetingParticipantsController::AllowParticipantsToStartVideo)
        .def("IsParticipantsStartVideoAllowed", &IMeetingParticipantsController::IsParticipantsStartVideoAllowed)
        .def("AllowParticipantsToShareWhiteBoard", &IMeetingParticipantsController::AllowParticipantsToShareWhiteBoard)
        .def("IsParticipantsShareWhiteBoardAllowed", &IMeetingParticipantsController::IsParticipantsShareWhiteBoardAllowed)
        .def("AllowParticipantsToChat", &IMeetingParticipantsController::AllowParticipantsToChat)
        .def("IsParticipantAllowedToChat", &IMeetingParticipantsController::IsParticipantAllowedToChat)
        .def("IsParticipantRequestLocalRecordingAllowed", &IMeetingParticipantsController::IsParticipantRequestLocalRecordingAllowed)
        .def("AllowParticipantsToRequestLocalRecording", &IMeetingParticipantsController::AllowParticipantsToRequestLocalRecording)
        .def("IsAutoAllowLocalRecordingRequest", &IMeetingParticipantsController::IsAutoAllowLocalRecordingRequest)
        .def("AutoAllowLocalRecordingRequest", &IMeetingParticipantsController::AutoAllowLocalRecordingRequest)
        .def("CanHideParticipantProfilePictures", &IMeetingParticipantsController::CanHideParticipantProfilePictures)
        .def("IsParticipantProfilePicturesHidden", &IMeetingParticipantsController::IsParticipantProfilePicturesHidden)
        .def("HideParticipantProfilePictures", &IMeetingParticipantsController::HideParticipantProfilePictures)
        .def("IsFocusModeEnabled", &IMeetingParticipantsController::IsFocusModeEnabled)
        .def("IsFocusModeOn", &IMeetingParticipantsController::IsFocusModeOn)
        .def("TurnFocusModeOn", &IMeetingParticipantsController::TurnFocusModeOn)
        .def("GetFocusModeShareType", &IMeetingParticipantsController::GetFocusModeShareType)
        .def("SetFocusModeShareType", &IMeetingParticipantsController::SetFocusModeShareType)
        .def("CanEnableParticipantRequestCloudRecording", &IMeetingParticipantsController::CanEnableParticipantRequestCloudRecording)
        .def("IsParticipantRequestCloudRecordingAllowed", &IMeetingParticipantsController::IsParticipantRequestCloudRecordingAllowed)
        .def("AllowParticipantsToRequestCloudRecording", &IMeetingParticipantsController::AllowParticipantsToRequestCloudRecording);


    nb::class_<ZOOM_SDK_NAMESPACE::IUserInfo>(m, "IUserInfo")
        .def("GetUserName", &IUserInfo::GetUserName)
        .def("IsHost", &IUserInfo::IsHost)
        .def("GetUserID", &IUserInfo::GetUserID)
        .def("GetAvatarPath", &IUserInfo::GetAvatarPath)
        .def("GetPersistentId", &IUserInfo::GetPersistentId)
        .def("GetCustomerKey", &IUserInfo::GetCustomerKey)
        .def("IsVideoOn", &IUserInfo::IsVideoOn)
        .def("IsAudioMuted", &IUserInfo::IsAudioMuted)
        .def("GetAudioJoinType", &IUserInfo::GetAudioJoinType)
        .def("IsMySelf", &IUserInfo::IsMySelf)
        .def("IsInWaitingRoom", &IUserInfo::IsInWaitingRoom)
        .def("IsRaiseHand", &IUserInfo::IsRaiseHand)
        .def("GetUserRole", &IUserInfo::GetUserRole)
        .def("IsPurePhoneUser", &IUserInfo::IsPurePhoneUser)
        .def("GetAudioVoiceLevel", &IUserInfo::GetAudioVoiceLevel)
        .def("IsClosedCaptionSender", &IUserInfo::IsClosedCaptionSender)
        .def("IsTalking", &IUserInfo::IsTalking)
        .def("IsH323User", &IUserInfo::IsH323User)
        .def("GetWebinarAttendeeStatus", &IUserInfo::GetWebinarAttendeeStatus)
        .def("GetLocalRecordingStatus", &IUserInfo::GetLocalRecordingStatus)
        .def("IsRawLiveStreaming", &IUserInfo::IsRawLiveStreaming)
        .def("HasRawLiveStreamPrivilege", &IUserInfo::HasRawLiveStreamPrivilege)
        .def("HasCamera", &IUserInfo::HasCamera);
}