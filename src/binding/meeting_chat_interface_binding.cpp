#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "zoom_sdk_raw_data_def.h"

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
            IList<IRichTextStyleItem*>* list = self.GetTextStyleItemList();
            vector<IRichTextStyleItem*> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, nb::rv_policy::reference)
        .def("GetSegmentDetails", [](IChatMsgInfo& self) {
            IList<SegmentDetails>* list = self.GetSegmentDetails();
            vector<SegmentDetails> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, nb::rv_policy::reference);

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
        .def("onChatMessageEditNotification", &IMeetingChatCtrlEvent::onChatMessageEditNotification)
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
        .def("GetAllChatMessageID", [](IMeetingChatController& self) {
            IList<const zchar_t*>* list = self.GetAllChatMessageID();
            vector<const zchar_t*> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, nb::rv_policy::reference)
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
            IList<IRichTextStyleOffset*>* list = self.GetTextStyleOffsetList();
            vector<IRichTextStyleOffset*> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, nb::rv_policy::reference);

    nb::class_<SegmentDetails>(m, "SegmentDetails")
        .def_ro("strContent", &SegmentDetails::strContent)
        .def_ro("boldAttrs", &SegmentDetails::boldAttrs)
        .def_ro("italicAttrs", &SegmentDetails::italicAttrs)
        .def_ro("fontColorAttrs", &SegmentDetails::fontColorAttrs)
        .def_ro("backgroundColorAttrs", &SegmentDetails::backgroundColorAttrs);
}
