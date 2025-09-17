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
using namespace std;

class MeetingRecordingCtrlEventCallbacks : public ZOOM_SDK_NAMESPACE::IMeetingRecordingCtrlEvent {
private:
    function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)> m_onRecordingStatusCallback;
    function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)> m_onCloudRecordingStatusCallback;
    function<void(bool)> m_onRecordPrivilegeChangedCallback;
    function<void(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus)> m_onLocalRecordingPrivilegeRequestStatusCallback;
    function<void(ZOOM_SDK_NAMESPACE::RequestStartCloudRecordingStatus)> m_onRequestCloudRecordingResponseCallback;
    function<void(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler*)> m_onLocalRecordingPrivilegeRequestedCallback;
    function<void(ZOOM_SDK_NAMESPACE::IRequestStartCloudRecordingHandler*)> m_onStartCloudRecordingRequestedCallback;
    function<void(time_t)> m_onCloudRecordingStorageFullCallback;
    function<void(ZOOM_SDK_NAMESPACE::IRequestEnableAndStartSmartRecordingHandler*)> m_onEnableAndStartSmartRecordingRequestedCallback;
    function<void(ZOOM_SDK_NAMESPACE::ISmartRecordingEnableActionHandler*)> m_onSmartRecordingEnableActionCallbackFunc;
    function<void(ZOOM_SDK_NAMESPACE::TranscodingStatus, const zchar_t*)> m_onTranscodingStatusChangedCallback;

public:
    MeetingRecordingCtrlEventCallbacks(
        const function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)>& onRecordingStatusCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)>& onCloudRecordingStatusCallback = nullptr,
        const function<void(bool)>& onRecordPrivilegeChangedCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus)>& onLocalRecordingPrivilegeRequestStatusCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::RequestStartCloudRecordingStatus)>& onRequestCloudRecordingResponseCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler*)>& onLocalRecordingPrivilegeRequestedCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestStartCloudRecordingHandler*)>& onStartCloudRecordingRequestedCallback = nullptr,
        const function<void(time_t)>& onCloudRecordingStorageFullCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestEnableAndStartSmartRecordingHandler*)>& onEnableAndStartSmartRecordingRequestedCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::ISmartRecordingEnableActionHandler*)>& onSmartRecordingEnableActionCallbackFunc = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::TranscodingStatus, const zchar_t*)>& onTranscodingStatusChangedCallback = nullptr
    ) : m_onRecordingStatusCallback(onRecordingStatusCallback),
        m_onCloudRecordingStatusCallback(onCloudRecordingStatusCallback),
        m_onRecordPrivilegeChangedCallback(onRecordPrivilegeChangedCallback),
        m_onLocalRecordingPrivilegeRequestStatusCallback(onLocalRecordingPrivilegeRequestStatusCallback),
        m_onRequestCloudRecordingResponseCallback(onRequestCloudRecordingResponseCallback),
        m_onLocalRecordingPrivilegeRequestedCallback(onLocalRecordingPrivilegeRequestedCallback),
        m_onStartCloudRecordingRequestedCallback(onStartCloudRecordingRequestedCallback),
        m_onCloudRecordingStorageFullCallback(onCloudRecordingStorageFullCallback),
        m_onEnableAndStartSmartRecordingRequestedCallback(onEnableAndStartSmartRecordingRequestedCallback),
        m_onSmartRecordingEnableActionCallbackFunc(onSmartRecordingEnableActionCallbackFunc),
        m_onTranscodingStatusChangedCallback(onTranscodingStatusChangedCallback) {}

    void onRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status) override {
        if (m_onRecordingStatusCallback)
            m_onRecordingStatusCallback(status);
    }

    void onCloudRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status) override {
        if (m_onCloudRecordingStatusCallback)
            m_onCloudRecordingStatusCallback(status);
    }

    void onRecordPrivilegeChanged(bool bCanRec) override {
        if (m_onRecordPrivilegeChangedCallback)
            m_onRecordPrivilegeChangedCallback(bCanRec);
    }

    void onLocalRecordingPrivilegeRequestStatus(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus status) override {
        if (m_onLocalRecordingPrivilegeRequestStatusCallback)
            m_onLocalRecordingPrivilegeRequestStatusCallback(status);
    }

    void onRequestCloudRecordingResponse(ZOOM_SDK_NAMESPACE::RequestStartCloudRecordingStatus status) override {
        if (m_onRequestCloudRecordingResponseCallback)
            m_onRequestCloudRecordingResponseCallback(status);
    }

    void onLocalRecordingPrivilegeRequested(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler* handler) override {
        if (m_onLocalRecordingPrivilegeRequestedCallback)
            m_onLocalRecordingPrivilegeRequestedCallback(handler);
    }

    void onStartCloudRecordingRequested(ZOOM_SDK_NAMESPACE::IRequestStartCloudRecordingHandler* handler) override {
        if (m_onStartCloudRecordingRequestedCallback)
            m_onStartCloudRecordingRequestedCallback(handler);
    }

    void onCloudRecordingStorageFull(time_t gracePeriodDate) override {
        if (m_onCloudRecordingStorageFullCallback)
            m_onCloudRecordingStorageFullCallback(gracePeriodDate);
    }

    void onEnableAndStartSmartRecordingRequested(ZOOM_SDK_NAMESPACE::IRequestEnableAndStartSmartRecordingHandler* handler) override {
        if (m_onEnableAndStartSmartRecordingRequestedCallback)
            m_onEnableAndStartSmartRecordingRequestedCallback(handler);
    }

    void onSmartRecordingEnableActionCallback(ZOOM_SDK_NAMESPACE::ISmartRecordingEnableActionHandler* handler) override {
        if (m_onSmartRecordingEnableActionCallbackFunc)
            m_onSmartRecordingEnableActionCallbackFunc(handler);
    }

    void onTranscodingStatusChanged(ZOOM_SDK_NAMESPACE::TranscodingStatus status, const zchar_t* path) override {
        if (m_onTranscodingStatusChangedCallback)
            m_onTranscodingStatusChangedCallback(status, path);
    }
};

void init_meeting_recording_ctrl_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingRecordingCtrlEventCallbacks, ZOOM_SDK_NAMESPACE::IMeetingRecordingCtrlEvent>(m, "MeetingRecordingCtrlEventCallbacks")
    .def(nb::init<
        const function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)>&,
        const function<void(ZOOM_SDK_NAMESPACE::RecordingStatus)>&,
        const function<void(bool)>&,
        const function<void(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus)>&,
        const function<void(ZOOM_SDK_NAMESPACE::RequestStartCloudRecordingStatus)>&,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler*)>&,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestStartCloudRecordingHandler*)>&,
        const function<void(time_t)>&,
        const function<void(ZOOM_SDK_NAMESPACE::IRequestEnableAndStartSmartRecordingHandler*)>&,
        const function<void(ZOOM_SDK_NAMESPACE::ISmartRecordingEnableActionHandler*)>&,
        const function<void(ZOOM_SDK_NAMESPACE::TranscodingStatus, const zchar_t*)>&
    >(),
        nb::arg("onRecordingStatusCallback") = nullptr,
        nb::arg("onCloudRecordingStatusCallback") = nullptr,
        nb::arg("onRecordPrivilegeChangedCallback") = nullptr,
        nb::arg("onLocalRecordingPrivilegeRequestStatusCallback") = nullptr,
        nb::arg("onRequestCloudRecordingResponseCallback") = nullptr,
        nb::arg("onLocalRecordingPrivilegeRequestedCallback") = nullptr,
        nb::arg("onStartCloudRecordingRequestedCallback") = nullptr,
        nb::arg("onCloudRecordingStorageFullCallback") = nullptr,
        nb::arg("onEnableAndStartSmartRecordingRequestedCallback") = nullptr,
        nb::arg("onSmartRecordingEnableActionCallbackFunc") = nullptr,
        nb::arg("onTranscodingStatusChangedCallback") = nullptr
    );
}