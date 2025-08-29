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
using namespace std;
using namespace ZOOMSDK;

void init_meeting_waiting_room_interface_binding(nb::module_ &m) {
    // Enums
    nb::enum_<ZOOM_SDK_NAMESPACE::WaitingRoomLayoutType>(m, "WaitingRoomLayoutType")
        .value("WaitingRoomLayoutType_Default", ZOOM_SDK_NAMESPACE::WaitingRoomLayoutType_Default)
        .value("WaitingRoomLayoutType_Logo", ZOOM_SDK_NAMESPACE::WaitingRoomLayoutType_Logo)
        .value("WaitingRoomLayoutType_Video", ZOOM_SDK_NAMESPACE::WaitingRoomLayoutType_Video)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::CustomWaitingRoomDataStatus>(m, "CustomWaitingRoomDataStatus")
        .value("CustomWaitingRoomDataStatus_Init", ZOOM_SDK_NAMESPACE::CustomWaitingRoomDataStatus_Init)
        .value("CustomWaitingRoomDataStatus_Downloading", ZOOM_SDK_NAMESPACE::CustomWaitingRoomDataStatus_Downloading)
        .value("CustomWaitingRoomDataStatus_Download_OK", ZOOM_SDK_NAMESPACE::CustomWaitingRoomDataStatus_Download_OK)
        .value("CustomWaitingRoomDataStatus_Download_Failed", ZOOM_SDK_NAMESPACE::CustomWaitingRoomDataStatus_Download_Failed)
        .export_values();

    // Color structs
    nb::class_<ZOOM_SDK_NAMESPACE::WaitingRoomBrandingPageColor>(m, "WaitingRoomBrandingPageColor")
        .def(nb::init<>())
        .def_rw("background_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingPageColor::background_color);

    nb::class_<ZOOM_SDK_NAMESPACE::WaitingRoomBrandingTextColor>(m, "WaitingRoomBrandingTextColor")
        .def(nb::init<>())
        .def_rw("primary_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingTextColor::primary_color)
        .def_rw("secondary_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingTextColor::secondary_color)
        .def_rw("paragraph_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingTextColor::paragraph_color)
        .def_rw("hyperlink_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingTextColor::hyperlink_color);

    nb::class_<ZOOM_SDK_NAMESPACE::WaitingRoomBrandingButtonColor>(m, "WaitingRoomBrandingButtonColor")
        .def(nb::init<>())
        .def_rw("primary_button_color", &ZOOM_SDK_NAMESPACE::WaitingRoomBrandingButtonColor::primary_button_color);

    // Custom waiting room data struct
    nb::class_<ZOOM_SDK_NAMESPACE::CustomWaitingRoomData>(m, "CustomWaitingRoomData")
        .def(nb::init<>())
        .def_rw("title", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::title)
        .def_rw("description", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::description)
        .def_rw("logo_path", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::logo_path)
        .def_rw("video_path", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::video_path)
        .def_rw("image_path", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::image_path)
        .def_rw("type", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::type)
        .def_rw("status", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::status)
        .def_rw("page_color", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::page_color)
        .def_rw("text_color", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::text_color)
        .def_rw("button_color", &ZOOM_SDK_NAMESPACE::CustomWaitingRoomData::button_color);

    // Download handler interface
    nb::class_<ZOOM_SDK_NAMESPACE::IWaitingRoomDataDownloadHandler>(m, "IWaitingRoomDataDownloadHandler")
        .def("Retry", &ZOOM_SDK_NAMESPACE::IWaitingRoomDataDownloadHandler::Retry)
        .def("Ignore", &ZOOM_SDK_NAMESPACE::IWaitingRoomDataDownloadHandler::Ignore);

    // Event callback interface
    nb::class_<ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent>(m, "IMeetingWaitingRoomEvent")
        .def("onWaitingRoomUserJoin", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomUserJoin)
        .def("onWaitingRoomUserLeft", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomUserLeft)
        .def("onWaitingRoomPresetAudioStatusChanged", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomPresetAudioStatusChanged)
        .def("onWaitingRoomPresetVideoStatusChanged", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomPresetVideoStatusChanged)
        .def("onCustomWaitingRoomDataUpdated", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onCustomWaitingRoomDataUpdated)
        .def("onWaitingRoomUserNameChanged", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomUserNameChanged)
        .def("onWaitingRoomEntranceEnabled", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent::onWaitingRoomEntranceEnabled);

    // Main controller interface
    nb::class_<ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController>(m, "IMeetingWaitingRoomController")
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::SetEvent)
        .def("IsSupportWaitingRoom", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::IsSupportWaitingRoom)
        .def("IsWaitingRoomOnEntryFlagOn", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::IsWaitingRoomOnEntryFlagOn)
        .def("EnableWaitingRoomOnEntry", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::EnableWaitingRoomOnEntry)
        .def("GetWaitingRoomLst", [](ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController& self) -> vector<unsigned int> {
            IList<unsigned int>* list = self.GetWaitingRoomLst();
            vector<unsigned int> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, "Returns a list of user IDs in the waiting room")
        .def("GetWaitingRoomUserInfoByID", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::GetWaitingRoomUserInfoByID, nb::rv_policy::reference)
        .def("AdmitToMeeting", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::AdmitToMeeting)
        .def("AdmitAllToMeeting", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::AdmitAllToMeeting)
        .def("PutInWaitingRoom", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::PutInWaitingRoom)
        .def("IsAudioEnabledInWaitingRoom", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::IsAudioEnabledInWaitingRoom)
        .def("IsVideoEnabledInWaitingRoom", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::IsVideoEnabledInWaitingRoom)
        .def("RequestCustomWaitingRoomData", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::RequestCustomWaitingRoomData)
        .def("CanRenameUser", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::CanRenameUser)
        .def("RenameUser", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::RenameUser)
        .def("CanExpelUser", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::CanExpelUser)
        .def("ExpelUser", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::ExpelUser)
        .def("IsWaitingRoomOnEntryLocked", &ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController::IsWaitingRoomOnEntryLocked);
}