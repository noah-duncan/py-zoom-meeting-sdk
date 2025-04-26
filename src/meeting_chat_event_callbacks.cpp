#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"

namespace nb = nanobind;
using namespace ZOOMSDK;
using namespace std;

class MeetingChatEventCallbacks : public IMeetingChatCtrlEvent {
private:
    function<void(IChatMsgInfo*, const zchar_t*)> m_onChatMsgNotificationCallback;
    function<void(ChatStatus*)> m_onChatStatusChangedNotificationCallback;
    function<void(const zchar_t*, SDKChatMessageDeleteType)> m_onChatMsgDeleteNotificationCallback;
    function<void(IChatMsgInfo*)> m_onChatMessageEditNotificationCallback;
    function<void(bool)> m_onShareMeetingChatStatusChangedCallback;
    function<void(ISDKFileSender*)> m_onFileSendStartCallback;
    function<void(ISDKFileReceiver*)> m_onFileReceivedCallback;
    function<void(SDKFileTransferInfo*)> m_onFileTransferProgressCallback;

public:
    MeetingChatEventCallbacks(
        const function<void(IChatMsgInfo*, const zchar_t*)>& onChatMsgNotificationCallback = nullptr,
        const function<void(ChatStatus*)>& onChatStatusChangedNotificationCallback = nullptr,
        const function<void(const zchar_t*, SDKChatMessageDeleteType)>& onChatMsgDeleteNotificationCallback = nullptr,
        const function<void(IChatMsgInfo*)>& onChatMessageEditNotificationCallback = nullptr,
        const function<void(bool)>& onShareMeetingChatStatusChangedCallback = nullptr,
        const function<void(ISDKFileSender*)>& onFileSendStartCallback = nullptr,
        const function<void(ISDKFileReceiver*)>& onFileReceivedCallback = nullptr,
        const function<void(SDKFileTransferInfo*)>& onFileTransferProgressCallback = nullptr
    ) : m_onChatMsgNotificationCallback(onChatMsgNotificationCallback),
        m_onChatStatusChangedNotificationCallback(onChatStatusChangedNotificationCallback),
        m_onChatMsgDeleteNotificationCallback(onChatMsgDeleteNotificationCallback),
        m_onChatMessageEditNotificationCallback(onChatMessageEditNotificationCallback),
        m_onShareMeetingChatStatusChangedCallback(onShareMeetingChatStatusChangedCallback),
        m_onFileSendStartCallback(onFileSendStartCallback),
        m_onFileReceivedCallback(onFileReceivedCallback),
        m_onFileTransferProgressCallback(onFileTransferProgressCallback) {}

    void onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content = NULL) override {
        if (m_onChatMsgNotificationCallback)
            m_onChatMsgNotificationCallback(chatMsg, content);
    }

    void onChatStatusChangedNotification(ChatStatus* status_) override {
        if (m_onChatStatusChangedNotificationCallback)
            m_onChatStatusChangedNotificationCallback(status_);
    }

    void onChatMsgDeleteNotification(const zchar_t* msgID, SDKChatMessageDeleteType deleteBy) override {
        if (m_onChatMsgDeleteNotificationCallback)
            m_onChatMsgDeleteNotificationCallback(msgID, deleteBy);
    }

    void onChatMessageEditNotification(IChatMsgInfo* chatMsg) override {
        if (m_onChatMessageEditNotificationCallback)
            m_onChatMessageEditNotificationCallback(chatMsg);
    }

    void onShareMeetingChatStatusChanged(bool isStart) override {
        if (m_onShareMeetingChatStatusChangedCallback)
            m_onShareMeetingChatStatusChangedCallback(isStart);
    }

    void onFileSendStart(ISDKFileSender* sender) override {
        if (m_onFileSendStartCallback)
            m_onFileSendStartCallback(sender);
    }

    void onFileReceived(ISDKFileReceiver* receiver) override {
        if (m_onFileReceivedCallback)
            m_onFileReceivedCallback(receiver);
    }

    void onFileTransferProgress(SDKFileTransferInfo* info) override {
        if (m_onFileTransferProgressCallback)
            m_onFileTransferProgressCallback(info);
    }
};

void init_meeting_chat_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingChatEventCallbacks, IMeetingChatCtrlEvent>(m, "MeetingChatEventCallbacks")
        .def(nb::init<
            const function<void(IChatMsgInfo*, const zchar_t*)>&,
            const function<void(ChatStatus*)>&,
            const function<void(const zchar_t*, SDKChatMessageDeleteType)>&,
            const function<void(IChatMsgInfo*)>&,
            const function<void(bool)>&,
            const function<void(ISDKFileSender*)>&,
            const function<void(ISDKFileReceiver*)>&,
            const function<void(SDKFileTransferInfo*)>&
        >(),
            nb::arg("onChatMsgNotificationCallback") = nullptr,
            nb::arg("onChatStatusChangedNotificationCallback") = nullptr,
            nb::arg("onChatMsgDeleteNotificationCallback") = nullptr,
            nb::arg("onChatMessageEditNotificationCallback") = nullptr,
            nb::arg("onShareMeetingChatStatusChangedCallback") = nullptr,
            nb::arg("onFileSendStartCallback") = nullptr,
            nb::arg("onFileReceivedCallback") = nullptr,
            nb::arg("onFileTransferProgressCallback") = nullptr
        );
}
