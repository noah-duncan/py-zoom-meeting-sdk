#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

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
using namespace std;

class MeetingParticipantsCtrlEventCallbacks : public ZOOM_SDK_NAMESPACE::IMeetingParticipantsCtrlEvent {
private:
    function<void(vector<unsigned int>, const zchar_t*)> m_onUserJoinCallback;
    function<void(vector<unsigned int>, const zchar_t*)> m_onUserLeftCallback;
    function<void(unsigned int)> m_onHostChangeNotificationCallback;
    function<void(bool, unsigned int)> m_onLowOrRaiseHandStatusChangedCallback;
    function<void(vector<unsigned int>)> m_onUserNamesChangedCallback;
    function<void(unsigned int, bool)> m_onCoHostChangeNotificationCallback;
    function<void()> m_onInvalidReclaimHostkeyCallback;
    function<void()> m_onAllHandsLoweredCallback;
    function<void(unsigned int, RecordingStatus)> m_onLocalRecordingStatusChangedCallback;
    function<void(bool)> m_onAllowParticipantsRenameNotificationCallback;
    function<void(bool)> m_onAllowParticipantsUnmuteSelfNotificationCallback;
    function<void(bool)> m_onAllowParticipantsStartVideoNotificationCallback;
    function<void(bool)> m_onAllowParticipantsShareWhiteBoardNotificationCallback;
    function<void(LocalRecordingRequestPrivilegeStatus)> m_onRequestLocalRecordingPrivilegeChangedCallback;
    function<void(bool)> m_onAllowParticipantsRequestCloudRecordingCallback;
    function<void(unsigned int)> m_onInMeetingUserAvatarPathUpdatedCallback;
    function<void(bool)> m_onParticipantProfilePictureStatusChangeCallback;
    function<void(bool)> m_onFocusModeStateChangedCallback;
    function<void(FocusModeShareType)> m_onFocusModeShareTypeChangedCallback;

public:
    MeetingParticipantsCtrlEventCallbacks(
        const function<void(vector<unsigned int>, const zchar_t*)>& onUserJoinCallback = nullptr,
        const function<void(vector<unsigned int>, const zchar_t*)>& onUserLeftCallback = nullptr,
        const function<void(unsigned int)>& onHostChangeNotificationCallback = nullptr,
        const function<void(bool, unsigned int)>& onLowOrRaiseHandStatusChangedCallback = nullptr,
        const function<void(vector<unsigned int>)>& onUserNamesChangedCallback = nullptr,
        const function<void(unsigned int, bool)>& onCoHostChangeNotificationCallback = nullptr,
        const function<void()>& onInvalidReclaimHostkeyCallback = nullptr,
        const function<void()>& onAllHandsLoweredCallback = nullptr,
        const function<void(unsigned int, RecordingStatus)>& onLocalRecordingStatusChangedCallback = nullptr,
        const function<void(bool)>& onAllowParticipantsRenameNotificationCallback = nullptr,
        const function<void(bool)>& onAllowParticipantsUnmuteSelfNotificationCallback = nullptr,
        const function<void(bool)>& onAllowParticipantsStartVideoNotificationCallback = nullptr,
        const function<void(bool)>& onAllowParticipantsShareWhiteBoardNotificationCallback = nullptr,
        const function<void(LocalRecordingRequestPrivilegeStatus)>& onRequestLocalRecordingPrivilegeChangedCallback = nullptr,
        const function<void(bool)>& onAllowParticipantsRequestCloudRecordingCallback = nullptr,
        const function<void(unsigned int)>& onInMeetingUserAvatarPathUpdatedCallback = nullptr,
        const function<void(bool)>& onParticipantProfilePictureStatusChangeCallback = nullptr,
        const function<void(bool)>& onFocusModeStateChangedCallback = nullptr,
        const function<void(FocusModeShareType)>& onFocusModeShareTypeChangedCallback = nullptr
    ) : m_onUserJoinCallback(onUserJoinCallback),
        m_onUserLeftCallback(onUserLeftCallback),
        m_onHostChangeNotificationCallback(onHostChangeNotificationCallback),
        m_onLowOrRaiseHandStatusChangedCallback(onLowOrRaiseHandStatusChangedCallback),
        m_onUserNamesChangedCallback(onUserNamesChangedCallback),
        m_onCoHostChangeNotificationCallback(onCoHostChangeNotificationCallback),
        m_onInvalidReclaimHostkeyCallback(onInvalidReclaimHostkeyCallback),
        m_onAllHandsLoweredCallback(onAllHandsLoweredCallback),
        m_onLocalRecordingStatusChangedCallback(onLocalRecordingStatusChangedCallback),
        m_onAllowParticipantsRenameNotificationCallback(onAllowParticipantsRenameNotificationCallback),
        m_onAllowParticipantsUnmuteSelfNotificationCallback(onAllowParticipantsUnmuteSelfNotificationCallback),
        m_onAllowParticipantsStartVideoNotificationCallback(onAllowParticipantsStartVideoNotificationCallback),
        m_onAllowParticipantsShareWhiteBoardNotificationCallback(onAllowParticipantsShareWhiteBoardNotificationCallback),
        m_onRequestLocalRecordingPrivilegeChangedCallback(onRequestLocalRecordingPrivilegeChangedCallback),
        m_onAllowParticipantsRequestCloudRecordingCallback(onAllowParticipantsRequestCloudRecordingCallback),
        m_onInMeetingUserAvatarPathUpdatedCallback(onInMeetingUserAvatarPathUpdatedCallback),
        m_onParticipantProfilePictureStatusChangeCallback(onParticipantProfilePictureStatusChangeCallback),
        m_onFocusModeStateChangedCallback(onFocusModeStateChangedCallback),
        m_onFocusModeShareTypeChangedCallback(onFocusModeShareTypeChangedCallback) {}

    void onUserJoin(IList<unsigned int>* lstUserID, const zchar_t* strUserList = NULL) override {
        if (m_onUserJoinCallback) {
            vector<unsigned int> result;
            if (lstUserID) {
                int count = lstUserID->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(lstUserID->GetItem(i));
                }
            }
            m_onUserJoinCallback(result, strUserList);
        }
    }

    void onUserLeft(IList<unsigned int>* lstUserID, const zchar_t* strUserList = NULL) override {
        if (m_onUserLeftCallback) {
            vector<unsigned int> result;
            if (lstUserID) {
                int count = lstUserID->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(lstUserID->GetItem(i));
                }
            }
            m_onUserLeftCallback(result, strUserList);
        }
    }

    void onHostChangeNotification(unsigned int userId) override {
        if (m_onHostChangeNotificationCallback)
            m_onHostChangeNotificationCallback(userId);
    }

    void onLowOrRaiseHandStatusChanged(bool bLow, unsigned int userid) override {
        if (m_onLowOrRaiseHandStatusChangedCallback)
            m_onLowOrRaiseHandStatusChangedCallback(bLow, userid);
    }

    void onUserNamesChanged(IList<unsigned int>* lstUserID) override {
        if (m_onUserNamesChangedCallback) {
            vector<unsigned int> result;
            if (lstUserID) {
                int count = lstUserID->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(lstUserID->GetItem(i));
                }
            }
            m_onUserNamesChangedCallback(result);
        }
    }

    void onCoHostChangeNotification(unsigned int userId, bool isCoHost) override {
        if (m_onCoHostChangeNotificationCallback)
            m_onCoHostChangeNotificationCallback(userId, isCoHost);
    }

    void onInvalidReclaimHostkey() override {
        if (m_onInvalidReclaimHostkeyCallback)
            m_onInvalidReclaimHostkeyCallback();
    }

    void onAllHandsLowered() override {
        if (m_onAllHandsLoweredCallback)
            m_onAllHandsLoweredCallback();
    }

    void onLocalRecordingStatusChanged(unsigned int user_id, RecordingStatus status) override {
        if (m_onLocalRecordingStatusChangedCallback)
            m_onLocalRecordingStatusChangedCallback(user_id, status);
    }

    void onAllowParticipantsRenameNotification(bool bAllow) override {
        if (m_onAllowParticipantsRenameNotificationCallback)
            m_onAllowParticipantsRenameNotificationCallback(bAllow);
    }

    void onAllowParticipantsUnmuteSelfNotification(bool bAllow) override {
        if (m_onAllowParticipantsUnmuteSelfNotificationCallback)
            m_onAllowParticipantsUnmuteSelfNotificationCallback(bAllow);
    }

    void onAllowParticipantsStartVideoNotification(bool bAllow) override {
        if (m_onAllowParticipantsStartVideoNotificationCallback)
            m_onAllowParticipantsStartVideoNotificationCallback(bAllow);
    }

    void onAllowParticipantsShareWhiteBoardNotification(bool bAllow) override {
        if (m_onAllowParticipantsShareWhiteBoardNotificationCallback)
            m_onAllowParticipantsShareWhiteBoardNotificationCallback(bAllow);
    }

    void onRequestLocalRecordingPrivilegeChanged(LocalRecordingRequestPrivilegeStatus status) override {
        if (m_onRequestLocalRecordingPrivilegeChangedCallback)
            m_onRequestLocalRecordingPrivilegeChangedCallback(status);
    }

    void onAllowParticipantsRequestCloudRecording(bool bAllow) override {
        if (m_onAllowParticipantsRequestCloudRecordingCallback)
            m_onAllowParticipantsRequestCloudRecordingCallback(bAllow);
    }

    void onInMeetingUserAvatarPathUpdated(unsigned int userID) override {
        if (m_onInMeetingUserAvatarPathUpdatedCallback)
            m_onInMeetingUserAvatarPathUpdatedCallback(userID);
    }

    void onParticipantProfilePictureStatusChange(bool bHidden) override {
        if (m_onParticipantProfilePictureStatusChangeCallback)
            m_onParticipantProfilePictureStatusChangeCallback(bHidden);
    }

    void onFocusModeStateChanged(bool bEnabled) override {
        if (m_onFocusModeStateChangedCallback)
            m_onFocusModeStateChangedCallback(bEnabled);
    }

    void onFocusModeShareTypeChanged(FocusModeShareType type) override {
        if (m_onFocusModeShareTypeChangedCallback)
            m_onFocusModeShareTypeChangedCallback(type);
    }
};

void init_meeting_participants_ctrl_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingParticipantsCtrlEventCallbacks, ZOOM_SDK_NAMESPACE::IMeetingParticipantsCtrlEvent>(m, "MeetingParticipantsCtrlEventCallbacks")
    .def(nb::init<
        const function<void(vector<unsigned int>, const zchar_t*)>&,
        const function<void(vector<unsigned int>, const zchar_t*)>&,
        const function<void(unsigned int)>&,
        const function<void(bool, unsigned int)>&,
        const function<void(vector<unsigned int>)>&,
        const function<void(unsigned int, bool)>&,
        const function<void()>&,
        const function<void()>&,
        const function<void(unsigned int, RecordingStatus)>&,
        const function<void(bool)>&,
        const function<void(bool)>&,
        const function<void(bool)>&,
        const function<void(bool)>&,
        const function<void(LocalRecordingRequestPrivilegeStatus)>&,
        const function<void(bool)>&,
        const function<void(unsigned int)>&,
        const function<void(bool)>&,
        const function<void(bool)>&,
        const function<void(FocusModeShareType)>&
    >(),
        nb::arg("onUserJoinCallback") = nullptr,
        nb::arg("onUserLeftCallback") = nullptr,
        nb::arg("onHostChangeNotificationCallback") = nullptr,
        nb::arg("onLowOrRaiseHandStatusChangedCallback") = nullptr,
        nb::arg("onUserNamesChangedCallback") = nullptr,
        nb::arg("onCoHostChangeNotificationCallback") = nullptr,
        nb::arg("onInvalidReclaimHostkeyCallback") = nullptr,
        nb::arg("onAllHandsLoweredCallback") = nullptr,
        nb::arg("onLocalRecordingStatusChangedCallback") = nullptr,
        nb::arg("onAllowParticipantsRenameNotificationCallback") = nullptr,
        nb::arg("onAllowParticipantsUnmuteSelfNotificationCallback") = nullptr,
        nb::arg("onAllowParticipantsStartVideoNotificationCallback") = nullptr,
        nb::arg("onAllowParticipantsShareWhiteBoardNotificationCallback") = nullptr,
        nb::arg("onRequestLocalRecordingPrivilegeChangedCallback") = nullptr,
        nb::arg("onAllowParticipantsRequestCloudRecordingCallback") = nullptr,
        nb::arg("onInMeetingUserAvatarPathUpdatedCallback") = nullptr,
        nb::arg("onParticipantProfilePictureStatusChangeCallback") = nullptr,
        nb::arg("onFocusModeStateChangedCallback") = nullptr,
        nb::arg("onFocusModeShareTypeChangedCallback") = nullptr
    );
}
