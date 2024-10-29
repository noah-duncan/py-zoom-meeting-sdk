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

void init_zoom_sdk_def_binding(nb::module_ &m) {

    nb::enum_<ZOOM_SDK_NAMESPACE::SDKError>(m, "SDKError")
        .value("SDKERR_SUCCESS", ZOOM_SDK_NAMESPACE::SDKERR_SUCCESS)
        .value("SDKERR_NO_IMPL", ZOOM_SDK_NAMESPACE::SDKERR_NO_IMPL)
        .value("SDKERR_WRONG_USAGE", ZOOM_SDK_NAMESPACE::SDKERR_WRONG_USAGE)
        .value("SDKERR_INVALID_PARAMETER", ZOOM_SDK_NAMESPACE::SDKERR_INVALID_PARAMETER)
        .value("SDKERR_MODULE_LOAD_FAILED", ZOOM_SDK_NAMESPACE::SDKERR_MODULE_LOAD_FAILED)
        .value("SDKERR_MEMORY_FAILED", ZOOM_SDK_NAMESPACE::SDKERR_MEMORY_FAILED)
        .value("SDKERR_SERVICE_FAILED", ZOOM_SDK_NAMESPACE::SDKERR_SERVICE_FAILED)
        .value("SDKERR_UNINITIALIZE", ZOOM_SDK_NAMESPACE::SDKERR_UNINITIALIZE)
        .value("SDKERR_UNAUTHENTICATION", ZOOM_SDK_NAMESPACE::SDKERR_UNAUTHENTICATION)
        .value("SDKERR_NORECORDINGINPROCESS", ZOOM_SDK_NAMESPACE::SDKERR_NORECORDINGINPROCESS)
        .value("SDKERR_TRANSCODER_NOFOUND", ZOOM_SDK_NAMESPACE::SDKERR_TRANSCODER_NOFOUND)
        .value("SDKERR_VIDEO_NOTREADY", ZOOM_SDK_NAMESPACE::SDKERR_VIDEO_NOTREADY)
        .value("SDKERR_NO_PERMISSION", ZOOM_SDK_NAMESPACE::SDKERR_NO_PERMISSION)
        .value("SDKERR_UNKNOWN", ZOOM_SDK_NAMESPACE::SDKERR_UNKNOWN)
        .value("SDKERR_OTHER_SDK_INSTANCE_RUNNING", ZOOM_SDK_NAMESPACE::SDKERR_OTHER_SDK_INSTANCE_RUNNING)
        .value("SDKERR_INTERNAL_ERROR", ZOOM_SDK_NAMESPACE::SDKERR_INTERNAL_ERROR)
        .value("SDKERR_NO_AUDIODEVICE_ISFOUND", ZOOM_SDK_NAMESPACE::SDKERR_NO_AUDIODEVICE_ISFOUND)
        .value("SDKERR_NO_VIDEODEVICE_ISFOUND", ZOOM_SDK_NAMESPACE::SDKERR_NO_VIDEODEVICE_ISFOUND)
        .value("SDKERR_TOO_FREQUENT_CALL", ZOOM_SDK_NAMESPACE::SDKERR_TOO_FREQUENT_CALL)
        .value("SDKERR_FAIL_ASSIGN_USER_PRIVILEGE", ZOOM_SDK_NAMESPACE::SDKERR_FAIL_ASSIGN_USER_PRIVILEGE)
        .value("SDKERR_MEETING_DONT_SUPPORT_FEATURE", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_DONT_SUPPORT_FEATURE)
        .value("SDKERR_MEETING_NOT_SHARE_SENDER", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_NOT_SHARE_SENDER)
        .value("SDKERR_MEETING_YOU_HAVE_NO_SHARE", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_YOU_HAVE_NO_SHARE)
        .value("SDKERR_MEETING_VIEWTYPE_PARAMETER_IS_WRONG", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_VIEWTYPE_PARAMETER_IS_WRONG)
        .value("SDKERR_MEETING_ANNOTATION_IS_OFF", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_ANNOTATION_IS_OFF)
        .value("SDKERR_SETTING_OS_DONT_SUPPORT", ZOOM_SDK_NAMESPACE::SDKERR_SETTING_OS_DONT_SUPPORT)
        .value("SDKERR_EMAIL_LOGIN_IS_DISABLED", ZOOM_SDK_NAMESPACE::SDKERR_EMAIL_LOGIN_IS_DISABLED)
        .value("SDKERR_HARDWARE_NOT_MEET_FOR_VB", ZOOM_SDK_NAMESPACE::SDKERR_HARDWARE_NOT_MEET_FOR_VB)
        .value("SDKERR_NEED_USER_CONFIRM_RECORD_DISCLAIMER", ZOOM_SDK_NAMESPACE::SDKERR_NEED_USER_CONFIRM_RECORD_DISCLAIMER)
        .value("SDKERR_NO_SHARE_DATA", ZOOM_SDK_NAMESPACE::SDKERR_NO_SHARE_DATA)
        .value("SDKERR_SHARE_CANNOT_SUBSCRIBE_MYSELF", ZOOM_SDK_NAMESPACE::SDKERR_SHARE_CANNOT_SUBSCRIBE_MYSELF)
        .value("SDKERR_NOT_IN_MEETING", ZOOM_SDK_NAMESPACE::SDKERR_NOT_IN_MEETING)
        .value("SDKERR_NOT_JOIN_AUDIO", ZOOM_SDK_NAMESPACE::SDKERR_NOT_JOIN_AUDIO)
        .value("SDKERR_HARDWARE_DONT_SUPPORT", ZOOM_SDK_NAMESPACE::SDKERR_HARDWARE_DONT_SUPPORT)
        .value("SDKERR_DOMAIN_DONT_SUPPORT", ZOOM_SDK_NAMESPACE::SDKERR_DOMAIN_DONT_SUPPORT)
        .value("SDKERR_MEETING_REMOTE_CONTROL_IS_OFF", ZOOM_SDK_NAMESPACE::SDKERR_MEETING_REMOTE_CONTROL_IS_OFF)
        .value("SDKERR_FILETRANSFER_ERROR", ZOOM_SDK_NAMESPACE::SDKERR_FILETRANSFER_ERROR)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::SDK_LANGUAGE_ID>(m, "SDK_LANGUAGE_ID")
        .value("LANGUAGE_Unknown", ZOOM_SDK_NAMESPACE::LANGUAGE_Unknown)
        .value("LANGUAGE_English", ZOOM_SDK_NAMESPACE::LANGUAGE_English)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::InitParam>(m, "InitParam")
        .def(nb::init<>())
        .def_rw("strWebDomain", &ZOOM_SDK_NAMESPACE::InitParam::strWebDomain)
        .def_rw("strBrandingName", &ZOOM_SDK_NAMESPACE::InitParam::strBrandingName)
        .def_rw("strSupportUrl", &ZOOM_SDK_NAMESPACE::InitParam::strSupportUrl)
        .def_rw("emLanguageID", &ZOOM_SDK_NAMESPACE::InitParam::emLanguageID)
        .def_rw("enableGenerateDump", &ZOOM_SDK_NAMESPACE::InitParam::enableGenerateDump)
        .def_rw("enableLogByDefault", &ZOOM_SDK_NAMESPACE::InitParam::enableLogByDefault)
        .def_rw("uiLogFileSize", &ZOOM_SDK_NAMESPACE::InitParam::uiLogFileSize);        

    nb::enum_<ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel>(m, "ZoomSDKAudioChannel")
        .value("ZoomSDKAudioChannel_Mono", ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel_Mono)
        .value("ZoomSDKAudioChannel_Stereo", ZOOM_SDK_NAMESPACE::ZoomSDKAudioChannel_Stereo)
        .export_values();
}