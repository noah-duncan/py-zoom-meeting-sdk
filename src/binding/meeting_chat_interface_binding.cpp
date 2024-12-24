#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

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


#include "zoom_sdk.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "../ilist_caster.h"
namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

void init_meeting_chat_interface_binding(nb::module_ &m) {
    nb::enum_<SDKChatMessageType>(m, "SDKChatMessageType")
        .value("To_None", SDKChatMessageType_To_None)
        .value("To_All", SDKChatMessageType_To_All)
        .value("To_All_Panelist", SDKChatMessageType_To_All_Panelist)
        .value("To_Individual_Panelist", SDKChatMessageType_To_Individual_Panelist)
        .value("To_Individual", SDKChatMessageType_To_Individual)
        .value("To_WaitingRoomUsers", SDKChatMessageType_To_WaitingRoomUsers);

    nb::enum_<RichTextStyle>(m, "RichTextStyle")
        .value("None", TextStyle_None)
        .value("Bold", TextStyle_Bold)
        .value("Italic", TextStyle_Italic)
        .value("Strikethrough", TextStyle_Strikethrough)
        .value("BulletedList", TextStyle_BulletedList)
        .value("NumberedList", TextStyle_NumberedList)
        .value("Underline", TextStyle_Underline)
        .value("FontSize", TextStyle_FontSize)
        .value("FontColor", TextStyle_FontColor)
        .value("BackgroundColor", TextStyle_BackgroundColor)
        .value("Indent", TextStyle_Indent)
        .value("Paragraph", TextStyle_Paragraph)
        .value("Quote", TextStyle_Quote)
        .value("InsertLink", TextStyle_InsertLink);

    // Structs
    nb::class_<InsertLinkAttrs>(m, "InsertLinkAttrs")
        .def(nb::init<>())
        .def_rw("insertLinkUrl", &InsertLinkAttrs::insertLinkUrl);

    nb::class_<FontSizeAttrs>(m, "FontSizeAttrs")
        .def(nb::init<>())
        .def_rw("fontSize", &FontSizeAttrs::fontSize);

    nb::class_<FontColorAttrs>(m, "FontColorAttrs")
        .def(nb::init<>())
        .def_rw("red", &FontColorAttrs::red)
        .def_rw("green", &FontColorAttrs::green)
        .def_rw("blue", &FontColorAttrs::blue);

    nb::class_<BackgroundColorAttrs>(m, "BackgroundColorAttrs")
        .def(nb::init<>())
        .def_rw("red", &BackgroundColorAttrs::red)
        .def_rw("green", &BackgroundColorAttrs::green)
        .def_rw("blue", &BackgroundColorAttrs::blue);

    nb::class_<ParagraphAttrs>(m, "ParagraphAttrs")
        .def(nb::init<>())
        .def_rw("strParagraph", &ParagraphAttrs::strParagraph);

    nb::class_<IndentAttrs>(m, "IndentAttrs")
        .def(nb::init<>())
        .def_rw("indent", &IndentAttrs::indent);


    nb::class_<IChatMsgInfo>(m, "IChatMsgInfo")
        .def("GetMessageID", &IChatMsgInfo::GetMessageID)
        .def("GetSenderUserId", &IChatMsgInfo::GetSenderUserId)
        .def("GetSenderDisplayName", &IChatMsgInfo::GetSenderDisplayName)
        .def("GetReceiverUserId", &IChatMsgInfo::GetReceiverUserId)
        .def("GetReceiverDisplayName", &IChatMsgInfo::GetReceiverDisplayName)
        .def("GetContent", &IChatMsgInfo::GetContent)
        .def("GetTimeStamp", &IChatMsgInfo::GetTimeStamp)
        .def("IsChatToAll", &IChatMsgInfo::IsChatToAll)
        .def("IsChatToAllPanelist", &IChatMsgInfo::IsChatToAllPanelist)
        .def("IsChatToWaitingroom", &IChatMsgInfo::IsChatToWaitingroom)
        .def("GetChatMessageType", &IChatMsgInfo::GetChatMessageType)
        .def("IsComment", &IChatMsgInfo::IsComment)
        .def("IsThread", &IChatMsgInfo::IsThread)
        .def("GetThreadID", &IChatMsgInfo::GetThreadID)
        .def("GetTextStyleItemList", [](IChatMsgInfo& self) {
            auto list = self.GetTextStyleItemList();
            if (!list) {
                return static_cast<IList<IRichTextStyleItem*>*>(nullptr);
            }
            return list;
        }, nb::rv_policy::reference)
        .def("GetSegmentDetails", &IChatMsgInfo::GetSegmentDetails);

    nb::class_<IChatMsgInfoBuilder>(m, "IChatMsgInfoBuilder")
        .def("SetContent", &IChatMsgInfoBuilder::SetContent)
        .def("SetReceiver", &IChatMsgInfoBuilder::SetReceiver)
        .def("SetThreadId", &IChatMsgInfoBuilder::SetThreadId)
        .def("SetMessageType", &IChatMsgInfoBuilder::SetMessageType)
        .def("SetQuotePosition", &IChatMsgInfoBuilder::SetQuotePosition)
        .def("UnsetQuotePosition", &IChatMsgInfoBuilder::UnsetQuotePosition)
        .def("SetInsertLink", &IChatMsgInfoBuilder::SetInsertLink)
        .def("UnsetInsertLink", &IChatMsgInfoBuilder::UnsetInsertLink)
        .def("SetFontSize", &IChatMsgInfoBuilder::SetFontSize)
        .def("UnsetFontSize", &IChatMsgInfoBuilder::UnsetFontSize)
        .def("SetItalic", &IChatMsgInfoBuilder::SetItalic)
        .def("UnsetItalic", &IChatMsgInfoBuilder::UnsetItalic)
        .def("SetBold", &IChatMsgInfoBuilder::SetBold)
        .def("UnsetBold", &IChatMsgInfoBuilder::UnsetBold)
        .def("SetStrikethrough", &IChatMsgInfoBuilder::SetStrikethrough)
        .def("UnsetStrikethrough", &IChatMsgInfoBuilder::UnsetStrikethrough)
        .def("SetBulletedList", &IChatMsgInfoBuilder::SetBulletedList)
        .def("UnsetBulletedList", &IChatMsgInfoBuilder::UnsetBulletedList)
        .def("SetNumberedList", &IChatMsgInfoBuilder::SetNumberedList)
        .def("UnsetNumberedList", &IChatMsgInfoBuilder::UnsetNumberedList)
        .def("SetUnderline", &IChatMsgInfoBuilder::SetUnderline)
        .def("UnsetUnderline", &IChatMsgInfoBuilder::UnsetUnderline)
        .def("SetFontColor", &IChatMsgInfoBuilder::SetFontColor)
        .def("UnsetFontColor", &IChatMsgInfoBuilder::UnsetFontColor)
        .def("SetBackgroundColor", &IChatMsgInfoBuilder::SetBackgroundColor)
        .def("UnsetBackgroundColor", &IChatMsgInfoBuilder::UnsetBackgroundColor)
        .def("IncreaseIndent", &IChatMsgInfoBuilder::IncreaseIndent)
        .def("DecreaseIndent", &IChatMsgInfoBuilder::DecreaseIndent)
        .def("SetParagraph", &IChatMsgInfoBuilder::SetParagraph)
        .def("UnsetParagraph", &IChatMsgInfoBuilder::UnsetParagraph)
        .def("ClearStyles", &IChatMsgInfoBuilder::ClearStyles)
        .def("Clear", &IChatMsgInfoBuilder::Clear)
        .def("Build", &IChatMsgInfoBuilder::Build, nb::rv_policy::reference);

    nb::class_<IMeetingChatCtrlEvent>(m, "IMeetingChatCtrlEvent")
        .def("onChatMsgNotification", &IMeetingChatCtrlEvent::onChatMsgNotification)
        .def("onChatStatusChangedNotification", &IMeetingChatCtrlEvent::onChatStatusChangedNotification)
        .def("onChatMsgDeleteNotification", &IMeetingChatCtrlEvent::onChatMsgDeleteNotification)
        .def("onShareMeetingChatStatusChanged", &IMeetingChatCtrlEvent::onShareMeetingChatStatusChanged)
        .def("onFileSendStart", &IMeetingChatCtrlEvent::onFileSendStart)
        .def("onFileReceived", &IMeetingChatCtrlEvent::onFileReceived)
        .def("onFileTransferProgress", &IMeetingChatCtrlEvent::onFileTransferProgress);

    nb::class_<NormalMeetingChatStatus>(m, "NormalMeetingChatStatus")
        .def(nb::init<>())
        .def_rw("can_chat", &NormalMeetingChatStatus::can_chat)
        .def_rw("can_chat_to_all", &NormalMeetingChatStatus::can_chat_to_all)
        .def_rw("can_chat_to_individual", &NormalMeetingChatStatus::can_chat_to_individual)
        .def_rw("is_only_can_chat_to_host", &NormalMeetingChatStatus::is_only_can_chat_to_host);

    nb::class_<WebinarAttendeeChatStatus>(m, "WebinarAttendeeChatStatus")
        .def(nb::init<>())
        .def_rw("can_chat", &WebinarAttendeeChatStatus::can_chat)
        .def_rw("can_chat_to_all_panellist_and_attendee", &WebinarAttendeeChatStatus::can_chat_to_all_panellist_and_attendee)
        .def_rw("can_chat_to_all_panellist", &WebinarAttendeeChatStatus::can_chat_to_all_panellist);

    nb::class_<WebinarOtherUserRoleChatStatus>(m, "WebinarOtherUserRoleChatStatus")
        .def(nb::init<>())
        .def_rw("can_chat_to_all_panellist", &WebinarOtherUserRoleChatStatus::can_chat_to_all_panellist)
        .def_rw("can_chat_to_all_panellist_and_attendee", &WebinarOtherUserRoleChatStatus::can_chat_to_all_panellist_and_attendee)
        .def_rw("can_chat_to_individual", &WebinarOtherUserRoleChatStatus::can_chat_to_individual);

    nb::class_<ChatStatus>(m, "ChatStatus")
        .def(nb::init<>())
        .def_rw("is_chat_off", &ChatStatus::is_chat_off)
        .def_rw("is_webinar_attendee", &ChatStatus::is_webinar_attendee)
        .def_rw("is_webinar_meeting", &ChatStatus::is_webinar_meeting)
        .def_prop_rw("normal_meeting_status",
            [](const ChatStatus& self) -> NormalMeetingChatStatus { return self.ut.normal_meeting_status; },
            [](ChatStatus& self, const NormalMeetingChatStatus& status) { self.ut.normal_meeting_status = status; })
        .def_prop_rw("webinar_attendee_status",
            [](const ChatStatus& self) -> WebinarAttendeeChatStatus { return self.ut.webinar_attendee_status; },
            [](ChatStatus& self, const WebinarAttendeeChatStatus& status) { self.ut.webinar_attendee_status = status; })
        .def_prop_rw("webinar_other_status",
            [](const ChatStatus& self) -> WebinarOtherUserRoleChatStatus { return self.ut.webinar_other_status; },
            [](ChatStatus& self, const WebinarOtherUserRoleChatStatus& status) { self.ut.webinar_other_status = status; });

    nb::class_<IMeetingChatController>(m, "IMeetingChatController")
        .def("SetEvent", &IMeetingChatController::SetEvent)
        .def("GetChatStatus", &IMeetingChatController::GetChatStatus, nb::rv_policy::reference)
        .def("SetParticipantsChatPrivilege", &IMeetingChatController::SetParticipantsChatPrivilege)
        .def("IsMeetingChatLegalNoticeAvailable", &IMeetingChatController::IsMeetingChatLegalNoticeAvailable)
        .def("getChatLegalNoticesPrompt", &IMeetingChatController::getChatLegalNoticesPrompt)
        .def("getChatLegalNoticesExplained", &IMeetingChatController::getChatLegalNoticesExplained)
        .def("IsShareMeetingChatLegalNoticeAvailable", &IMeetingChatController::IsShareMeetingChatLegalNoticeAvailable)
        .def("GetShareMeetingChatStartedLegalNoticeContent", &IMeetingChatController::GetShareMeetingChatStartedLegalNoticeContent)
        .def("GetShareMeetingChatStoppedLegalNoticeContent", &IMeetingChatController::GetShareMeetingChatStoppedLegalNoticeContent)
        .def("IsChatMessageCanBeDeleted", &IMeetingChatController::IsChatMessageCanBeDeleted)
        .def("DeleteChatMessage", &IMeetingChatController::DeleteChatMessage)
        .def("GetChatMessageById", &IMeetingChatController::GetChatMessageById, nb::rv_policy::reference)
        .def("GetChatMessageBuilder", &IMeetingChatController::GetChatMessageBuilder, nb::rv_policy::reference)
        .def("SendChatMsgTo", &IMeetingChatController::SendChatMsgTo)
        .def("IsFileTransferEnabled", &IMeetingChatController::IsFileTransferEnabled)
        .def("TransferFile", &IMeetingChatController::TransferFile)
        .def("TransferFileToAll", &IMeetingChatController::TransferFileToAll)
        .def("GetTransferFileTypeAllowList", &IMeetingChatController::GetTransferFileTypeAllowList)
        .def("GetMaxTransferFileSizeBytes", &IMeetingChatController::GetMaxTransferFileSizeBytes);

    nb::class_<IRichTextStyleOffset>(m, "IRichTextStyleOffset")
        .def("GetPositionStart", &IRichTextStyleOffset::GetPositionStart)
        .def("GetPositionEnd", &IRichTextStyleOffset::GetPositionEnd)
        .def("GetReserve", &IRichTextStyleOffset::GetReserve);

    nb::class_<IRichTextStyleItem>(m, "IRichTextStyleItem")
        .def("GetTextStyle", &IRichTextStyleItem::GetTextStyle)
        .def("GetTextStyleOffsetList", [](IRichTextStyleItem& self) {
            auto list = self.GetTextStyleOffsetList();
            if (!list) {
                return static_cast<IList<IRichTextStyleOffset*>*>(nullptr);
            }
            return list;
        }, nb::rv_policy::reference);

    nb::class_<SegmentDetails>(m, "SegmentDetails")
        .def_ro("strContent", &SegmentDetails::strContent)
        .def_ro("boldAttrs", &SegmentDetails::boldAttrs)
        .def_ro("italicAttrs", &SegmentDetails::italicAttrs)
        .def_ro("fontColorAttrs", &SegmentDetails::fontColorAttrs)
        .def_ro("backgroundColorAttrs", &SegmentDetails::backgroundColorAttrs);

            nb::class_<IMeetingParticipantsCtrlEvent>(m, "IMeetingParticipantsCtrlEvent")
        .def("onHostChangeNotification", &IMeetingParticipantsCtrlEvent::onHostChangeNotification)
        .def("onLowOrRaiseHandStatusChanged", &IMeetingParticipantsCtrlEvent::onLowOrRaiseHandStatusChanged)
        .def("onUserNamesChanged", &IMeetingParticipantsCtrlEvent::onUserNamesChanged)
        .def("onCoHostChangeNotification", &IMeetingParticipantsCtrlEvent::onCoHostChangeNotification)
        .def("onInvalidReclaimHostkey", &IMeetingParticipantsCtrlEvent::onInvalidReclaimHostkey)
        .def("onAllHandsLowered", &IMeetingParticipantsCtrlEvent::onAllHandsLowered)
        .def("onLocalRecordingStatusChanged", &IMeetingParticipantsCtrlEvent::onLocalRecordingStatusChanged)
        .def("onAllowParticipantsRenameNotification", &IMeetingParticipantsCtrlEvent::onAllowParticipantsRenameNotification)
        .def("onAllowParticipantsUnmuteSelfNotification", &IMeetingParticipantsCtrlEvent::onAllowParticipantsUnmuteSelfNotification)
        .def("onAllowParticipantsStartVideoNotification", &IMeetingParticipantsCtrlEvent::onAllowParticipantsStartVideoNotification)
        .def("onAllowParticipantsShareWhiteBoardNotification", &IMeetingParticipantsCtrlEvent::onAllowParticipantsShareWhiteBoardNotification)
        .def("onRequestLocalRecordingPrivilegeChanged", &IMeetingParticipantsCtrlEvent::onRequestLocalRecordingPrivilegeChanged)
        .def("onAllowParticipantsRequestCloudRecording", &IMeetingParticipantsCtrlEvent::onAllowParticipantsRequestCloudRecording)
        .def("onInMeetingUserAvatarPathUpdated", &IMeetingParticipantsCtrlEvent::onInMeetingUserAvatarPathUpdated)
        .def("onParticipantProfilePictureStatusChange", &IMeetingParticipantsCtrlEvent::onParticipantProfilePictureStatusChange)
        .def("onFocusModeStateChanged", &IMeetingParticipantsCtrlEvent::onFocusModeStateChanged)
        .def("onFocusModeShareTypeChanged", &IMeetingParticipantsCtrlEvent::onFocusModeShareTypeChanged);

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
        .def("HasCamera", &IUserInfo::HasCamera)
        .def("IsProductionStudioUser", &IUserInfo::IsProductionStudioUser)
        .def("GetProductionStudioParent", &IUserInfo::GetProductionStudioParent);
}
