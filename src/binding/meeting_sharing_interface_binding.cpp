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
#include "rawdata/rawdata_video_source_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_meeting_sharing_interface_binding(nb::module_ &m) {
    // Sharing enums
    nb::enum_<ShareSettingType>(m, "ShareSettingType")
        .value("LOCK_SHARE", ShareSettingType_LOCK_SHARE)
        .value("HOST_GRAB", ShareSettingType_HOST_GRAB)
        .value("ANYONE_GRAB", ShareSettingType_ANYONE_GRAB)
        .value("MULTI_SHARE", ShareSettingType_MULTI_SHARE)
        .export_values();

    nb::enum_<AudioShareMode>(m, "AudioShareMode")
        .value("Mono", AudioShareMode_Mono)
        .value("Stereo", AudioShareMode_Stereo)
        .export_values();

    nb::enum_<MultiShareOption>(m, "MultiShareOption")
        .value("Enable_Multi_Share", Enable_Multi_Share)
        .value("Enable_Only_HOST_Start_Share", Enable_Only_HOST_Start_Share)
        .value("Enable_Only_HOST_Grab_Share", Enable_Only_HOST_Grab_Share)
        .value("Enable_All_Grab_Share", Enable_All_Grab_Share)
        .export_values();

    nb::enum_<ZoomSDKVideoFileSharePlayError>(m, "VideoFileSharePlayError")
        .value("None", ZoomSDKVideoFileSharePlayError_None)
        .value("Not_Supported", ZoomSDKVideoFileSharePlayError_Not_Supported)
        .value("Resolution_Too_High", ZoomSDKVideoFileSharePlayError_Resolution_Too_High)
        .value("Open_Fail", ZoomSDKVideoFileSharePlayError_Open_Fail)
        .value("Play_Fail", ZoomSDKVideoFileSharePlayError_Play_Fail)
        .value("Seek_Fail", ZoomSDKVideoFileSharePlayError_Seek_Fail)
        .export_values();

    // IMeetingShareController
    nb::class_<IMeetingShareController>(m, "IMeetingShareController")
        .def("SetEvent", &IMeetingShareController::SetEvent)
        .def("StopShare", &IMeetingShareController::StopShare)
        .def("LockShare", &IMeetingShareController::LockShare)
        .def("PauseCurrentSharing", &IMeetingShareController::PauseCurrentSharing)
        .def("ResumeCurrentSharing", &IMeetingShareController::ResumeCurrentSharing)
        .def("GetViewableSharingUserList", [](IMeetingShareController& self) {
            IList<unsigned int>* list = self.GetViewableSharingUserList();
            std::vector<unsigned int> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, "Returns a vector of user IDs who are currently sharing")
        .def("GetSharingSourceInfoList", [](IMeetingShareController& self, unsigned int userID) {
            IList<ZoomSDKSharingSourceInfo>* list = self.GetSharingSourceInfoList(userID);
            std::vector<ZoomSDKSharingSourceInfo> result;
            if (list) {
                int count = list->GetCount();
                result.reserve(count);
                for (int i = 0; i < count; i++) {
                    result.push_back(list->GetItem(i));
                }
            }
            return result;
        }, "Returns a vector of sharing source information for the specified user")
        .def("IsDesktopSharingEnabled", &IMeetingShareController::IsDesktopSharingEnabled)
        .def("IsShareLocked", &IMeetingShareController::IsShareLocked)
        .def("IsSupportEnableShareComputerSound", &IMeetingShareController::IsSupportEnableShareComputerSound)
        .def("IsSupportEnableOptimizeForFullScreenVideoClip", [](IMeetingShareController& self) {
            return self.IsSupportEnableOptimizeForFullScreenVideoClip();
        }, "Determine if the current meeting supports sharing with optimize video")
        .def("IsSupportEnableOptimizeForFullScreenVideoClipWithStatus", [](IMeetingShareController& self) {
            bool bCurEnableOrNot = false;
            bool result = self.IsSupportEnableOptimizeForFullScreenVideoClip(bCurEnableOrNot);
            return std::make_tuple(result, bCurEnableOrNot);
        }, "Determine if optimization for video is supported and get current status (deprecated)")
        .def("EnableShareComputerSound", &IMeetingShareController::EnableShareComputerSound)
        .def("EnableShareComputerSoundWhenSharing", &IMeetingShareController::EnableShareComputerSoundWhenSharing)
        .def("SetAudioShareMode", &IMeetingShareController::SetAudioShareMode)
        .def("GetAudioShareMode", &IMeetingShareController::GetAudioShareMode)
        .def("EnableOptimizeForFullScreenVideoClip", &IMeetingShareController::EnableOptimizeForFullScreenVideoClip)
        .def("EnableOptimizeForFullScreenVideoClipWhenSharing", &IMeetingShareController::EnableOptimizeForFullScreenVideoClipWhenSharing)
        .def("SetMultiShareSettingOptions", &IMeetingShareController::SetMultiShareSettingOptions)
        .def("GetMultiShareSettingOptions", &IMeetingShareController::GetMultiShareSettingOptions)
        .def("CanSwitchToShareNextCamera", &IMeetingShareController::CanSwitchToShareNextCamera)
        .def("SwitchToShareNextCamera", &IMeetingShareController::SwitchToShareNextCamera)
        .def("CanShareVideoFile", &IMeetingShareController::CanShareVideoFile)
        .def("CanStartShare", [](IMeetingShareController& self) {
            CannotShareReasonType reason;
            bool result = self.CanStartShare(reason);
            return std::make_tuple(result, reason);
        }, "Returns a tuple of (can_share: bool, reason: CannotShareReasonType)");

    nb::class_<IMeetingShareCtrlEvent>(m, "IMeetingShareCtrlEvent")
        .def("onSharingStatus", &IMeetingShareCtrlEvent::onSharingStatus)
        .def("onLockShareStatus", &IMeetingShareCtrlEvent::onLockShareStatus)
        .def("onShareContentNotification", &IMeetingShareCtrlEvent::onShareContentNotification)
        .def("onMultiShareSwitchToSingleShareNeedConfirm", &IMeetingShareCtrlEvent::onMultiShareSwitchToSingleShareNeedConfirm)
        .def("onShareSettingTypeChangedNotification", &IMeetingShareCtrlEvent::onShareSettingTypeChangedNotification)
        .def("onSharedVideoEnded", &IMeetingShareCtrlEvent::onSharedVideoEnded)
        .def("onVideoFileSharePlayError", &IMeetingShareCtrlEvent::onVideoFileSharePlayError)
        .def("onFailedToStartShare", &IMeetingShareCtrlEvent::onFailedToStartShare)
        .def("onOptimizingShareForVideoClipStatusChanged", &IMeetingShareCtrlEvent::onOptimizingShareForVideoClipStatusChanged);

    // Add ZoomSDKSharingSourceInfo struct binding
    nb::class_<ZoomSDKSharingSourceInfo>(m, "SharingSourceInfo")
        .def_rw("userid", &ZoomSDKSharingSourceInfo::userid)
        .def_rw("shareSourceID", &ZoomSDKSharingSourceInfo::shareSourceID)
        .def_rw("status", &ZoomSDKSharingSourceInfo::status)
        .def_rw("isShowingInFirstView", &ZoomSDKSharingSourceInfo::isShowingInFirstView)
        .def_rw("isShowingInSecondView", &ZoomSDKSharingSourceInfo::isShowingInSecondView)
        .def_rw("isCanBeRemoteControl", &ZoomSDKSharingSourceInfo::isCanBeRemoteControl)
        .def_rw("bEnableOptimizingVideoSharing", &ZoomSDKSharingSourceInfo::bEnableOptimizingVideoSharing)
        .def_rw("contentType", &ZoomSDKSharingSourceInfo::contentType)
        .def_rw("hwndSharedApp", &ZoomSDKSharingSourceInfo::hwndSharedApp)
        .def_rw("monitorID", &ZoomSDKSharingSourceInfo::monitorID);
}