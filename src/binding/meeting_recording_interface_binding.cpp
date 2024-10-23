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

void init_meeting_recording_interface_binding(nb::module_ &m) {
   nb::class_<IMeetingRecordingCtrlEvent>(m, "IMeetingRecordingCtrlEvent")
        .def("onRecordingStatus", &IMeetingRecordingCtrlEvent::onRecordingStatus)
        .def("onCloudRecordingStatus", &IMeetingRecordingCtrlEvent::onCloudRecordingStatus)
        .def("onRecordPrivilegeChanged", &IMeetingRecordingCtrlEvent::onRecordPrivilegeChanged)
        .def("onLocalRecordingPrivilegeRequestStatus", &IMeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequestStatus)
        .def("onRequestCloudRecordingResponse", &IMeetingRecordingCtrlEvent::onRequestCloudRecordingResponse)
        .def("onLocalRecordingPrivilegeRequested", &IMeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequested)
        .def("onStartCloudRecordingRequested", &IMeetingRecordingCtrlEvent::onStartCloudRecordingRequested)
        .def("onCloudRecordingStorageFull", &IMeetingRecordingCtrlEvent::onCloudRecordingStorageFull)
        .def("onEnableAndStartSmartRecordingRequested", &IMeetingRecordingCtrlEvent::onEnableAndStartSmartRecordingRequested)
        .def("onSmartRecordingEnableActionCallback", &IMeetingRecordingCtrlEvent::onSmartRecordingEnableActionCallback)
        .def("onTranscodingStatusChanged", &IMeetingRecordingCtrlEvent::onTranscodingStatusChanged);

    nb::class_<IMeetingRecordingController>(m, "IMeetingRecordingController")
        .def("SetEvent", &IMeetingRecordingController::SetEvent)
        .def("IsSupportRequestLocalRecordingPrivilege", &IMeetingRecordingController::IsSupportRequestLocalRecordingPrivilege)
        .def("RequestLocalRecordingPrivilege", &IMeetingRecordingController::RequestLocalRecordingPrivilege)
        .def("RequestStartCloudRecording", &IMeetingRecordingController::RequestStartCloudRecording)
        .def("StartRecording", [](IMeetingRecordingController& self) {
            time_t startTimestamp;
            SDKError result = self.StartRecording(startTimestamp);
            return nb::make_tuple(result, nb::cast(startTimestamp));
        })
        .def("StopRecording", [](IMeetingRecordingController& self) {
            time_t stopTimestamp;
            SDKError result = self.StopRecording(stopTimestamp);
            return nb::make_tuple(result, nb::cast(stopTimestamp));
        })
        .def("CanStartRecording", &IMeetingRecordingController::CanStartRecording)
        .def("IsSmartRecordingEnabled", &IMeetingRecordingController::IsSmartRecordingEnabled)
        .def("CanEnableSmartRecordingFeature", &IMeetingRecordingController::CanEnableSmartRecordingFeature)
        .def("EnableSmartRecording", &IMeetingRecordingController::EnableSmartRecording)
        .def("CanAllowDisAllowLocalRecording", &IMeetingRecordingController::CanAllowDisAllowLocalRecording)
        .def("StartCloudRecording", &IMeetingRecordingController::StartCloudRecording)
        .def("StopCloudRecording", &IMeetingRecordingController::StopCloudRecording)
        .def("IsSupportLocalRecording", &IMeetingRecordingController::IsSupportLocalRecording)
        .def("AllowLocalRecording", &IMeetingRecordingController::AllowLocalRecording)
        .def("DisAllowLocalRecording", &IMeetingRecordingController::DisAllowLocalRecording)
        .def("PauseRecording", &IMeetingRecordingController::PauseRecording)
        .def("ResumeRecording", &IMeetingRecordingController::ResumeRecording)
        .def("PauseCloudRecording", &IMeetingRecordingController::PauseCloudRecording)
        .def("ResumeCloudRecording", &IMeetingRecordingController::ResumeCloudRecording)
        .def("CanStartRawRecording", &IMeetingRecordingController::CanStartRawRecording)
        .def("StartRawRecording", &IMeetingRecordingController::StartRawRecording)
        .def("StopRawRecording", &IMeetingRecordingController::StopRawRecording)
        .def("GetCloudRecordingStatus", &IMeetingRecordingController::GetCloudRecordingStatus)
        .def("SubscribeLocalrecordingResource", &IMeetingRecordingController::SubscribeLocalrecordingResource)
        .def("UnSubscribeLocalrecordingResource", &IMeetingRecordingController::UnSubscribeLocalrecordingResource);

}