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

class MeetingAudioCtrlEventCallbacks : public ZOOM_SDK_NAMESPACE::IMeetingAudioCtrlEvent {
private:
    function<void(vector<IUserAudioStatus*>, const zchar_t*)> m_onUserAudioStatusChangeCallback;
    function<void(vector<unsigned int>)> m_onUserActiveAudioChangeCallback;
    function<void(IRequestStartAudioHandler*)> m_onHostRequestStartAudioCallback;
    function<void(const zchar_t*)> m_onJoin3rdPartyTelephonyAudioCallback;
    function<void(bool)> m_onMuteOnEntryStatusChangeCallback;

public:
    MeetingAudioCtrlEventCallbacks(
        const function<void(vector<IUserAudioStatus*>, const zchar_t*)>& onUserAudioStatusChangeCallback = nullptr,
        const function<void(vector<unsigned int>)>& onUserActiveAudioChangeCallback = nullptr,
        const function<void(IRequestStartAudioHandler*)>& onHostRequestStartAudioCallback = nullptr,
        const function<void(const zchar_t*)>& onJoin3rdPartyTelephonyAudioCallback = nullptr,
        const function<void(bool)>& onMuteOnEntryStatusChangeCallback = nullptr
    ) : m_onUserAudioStatusChangeCallback(onUserAudioStatusChangeCallback),
        m_onUserActiveAudioChangeCallback(onUserActiveAudioChangeCallback),
        m_onHostRequestStartAudioCallback(onHostRequestStartAudioCallback),
        m_onJoin3rdPartyTelephonyAudioCallback(onJoin3rdPartyTelephonyAudioCallback),
        m_onMuteOnEntryStatusChangeCallback(onMuteOnEntryStatusChangeCallback) {}

    void onUserAudioStatusChange(IList<IUserAudioStatus*>* lstAudioStatusChange, const zchar_t* strAudioStatusList = NULL) override {
        if (m_onUserAudioStatusChangeCallback) {
            vector<IUserAudioStatus*> result;
            if (lstAudioStatusChange) {
                int count = lstAudioStatusChange->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(lstAudioStatusChange->GetItem(i));
                }
            }
            m_onUserAudioStatusChangeCallback(result, strAudioStatusList);
        }
    }

    void onUserActiveAudioChange(IList<unsigned int>* plstActiveAudio) override {
        if (m_onUserActiveAudioChangeCallback) {
            vector<unsigned int> result;
            if (plstActiveAudio) {
                int count = plstActiveAudio->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(plstActiveAudio->GetItem(i));
                }
            }
            m_onUserActiveAudioChangeCallback(result);
        }
    }

    void onHostRequestStartAudio(IRequestStartAudioHandler* handler_) override {
        if (m_onHostRequestStartAudioCallback)
            m_onHostRequestStartAudioCallback(handler_);
    }

    void onJoin3rdPartyTelephonyAudio(const zchar_t* audioInfo) override {
        if (m_onJoin3rdPartyTelephonyAudioCallback)
            m_onJoin3rdPartyTelephonyAudioCallback(audioInfo);
    }

    void onMuteOnEntryStatusChange(bool bEnabled) override {
        if (m_onMuteOnEntryStatusChangeCallback)
            m_onMuteOnEntryStatusChangeCallback(bEnabled);
    }
};

void init_meeting_audio_ctrl_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingAudioCtrlEventCallbacks, ZOOM_SDK_NAMESPACE::IMeetingAudioCtrlEvent>(m, "MeetingAudioCtrlEventCallbacks")
    .def(nb::init<
        const function<void(vector<IUserAudioStatus*>, const zchar_t*)>&,
        const function<void(vector<unsigned int>)>&,
        const function<void(IRequestStartAudioHandler*)>&,
        const function<void(const zchar_t*)>&,
        const function<void(bool)>&
    >(),
        nb::arg("onUserAudioStatusChangeCallback") = nullptr,
        nb::arg("onUserActiveAudioChangeCallback") = nullptr,
        nb::arg("onHostRequestStartAudioCallback") = nullptr,
        nb::arg("onJoin3rdPartyTelephonyAudioCallback") = nullptr,
        nb::arg("onMuteOnEntryStatusChangeCallback") = nullptr
    );
}