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

void init_zoom_sdk_binding(nb::module_ &m) {
    m.def("InitSDK", [](ZOOM_SDK_NAMESPACE::InitParam& initParam) {
        std::cout << "strWebDomain" << std::endl;
        std::cout << initParam.strWebDomain << std::endl;
        std::cout << "strSupportUrl" << std::endl;
        std::cout << initParam.strSupportUrl << std::endl;
        std::cout << "enableGenerateDump" << std::endl;
        std::cout << initParam.enableGenerateDump << std::endl;
        std::cout << "emLanguageID" << std::endl;
        std::cout << initParam.emLanguageID << std::endl;
        std::cout << "enableLogByDefault" << std::endl;
        std::cout << initParam.enableLogByDefault << std::endl;

        return ZOOM_SDK_NAMESPACE::InitSDK(initParam);
    }, nb::arg("initParam"), "Initialize ZOOM SDK");

    m.def("CleanUPSDK", &ZOOM_SDK_NAMESPACE::CleanUPSDK, nb::call_guard<nb::gil_scoped_release>());

    m.def("CreateMeetingService", []() -> ZOOM_SDK_NAMESPACE::IMeetingService* {
        ZOOM_SDK_NAMESPACE::IMeetingService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateMeetingService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create meeting service");
        }
        return pService;
    }, nb::rv_policy::reference);

    m.def("DestroyMeetingService", [](ZOOM_SDK_NAMESPACE::IMeetingService* pService) {
        return ZOOM_SDK_NAMESPACE::DestroyMeetingService(pService);
    }, "Destroy a meeting service instance");

    m.def("CreateSettingService", []() -> ZOOM_SDK_NAMESPACE::ISettingService* {
        ZOOM_SDK_NAMESPACE::ISettingService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateSettingService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create setting service");
        }
        return pService;
    }, nb::rv_policy::reference);

    m.def("DestroySettingService", &DestroySettingService);

    m.def("CreateAuthService", []() -> ZOOM_SDK_NAMESPACE::IAuthService* {
        ZOOM_SDK_NAMESPACE::IAuthService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateAuthService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create auth service");
        }
        return pService;
    }, nb::rv_policy::reference);

    m.def("DestroyAuthService", &DestroyAuthService);    
}