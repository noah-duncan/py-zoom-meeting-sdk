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

void init_auth_service_interface_binding(nb::module_ &m) {
    nb::enum_<LoginType>(m, "LoginType")
        .value("LoginType_Unknown", LoginType_Unknown)
        .value("LoginType_SSO", LoginType_SSO)
        .export_values();

    nb::class_<IAccountInfo>(m, "IAccountInfo")
        .def("GetDisplayName", &IAccountInfo::GetDisplayName)
        .def("GetLoginType", &IAccountInfo::GetLoginType);

    nb::enum_<LoginFailReason>(m, "LoginFailReason")
        .value("LoginFail_None", LoginFail_None)
        .export_values();

    nb::enum_<LOGINSTATUS>(m, "LOGINSTATUS")
        .value("LOGIN_IDLE", LOGIN_IDLE)
        .value("LOGIN_PROCESSING", LOGIN_PROCESSING)
        .value("LOGIN_SUCCESS", LOGIN_SUCCESS)
        .value("LOGIN_FAILED", LOGIN_FAILED)
        .export_values();

    nb::enum_<AuthResult>(m, "AuthResult")
        .value("AUTHRET_SUCCESS", AUTHRET_SUCCESS)
        .value("AUTHRET_KEYORSECRETEMPTY", AUTHRET_KEYORSECRETEMPTY)
        .value("AUTHRET_KEYORSECRETWRONG", AUTHRET_KEYORSECRETWRONG)
        .value("AUTHRET_ACCOUNTNOTSUPPORT", AUTHRET_ACCOUNTNOTSUPPORT)
        .value("AUTHRET_ACCOUNTNOTENABLESDK", AUTHRET_ACCOUNTNOTENABLESDK)
        .value("AUTHRET_UNKNOWN", AUTHRET_UNKNOWN)
        .value("AUTHRET_SERVICE_BUSY", AUTHRET_SERVICE_BUSY)
        .value("AUTHRET_NONE", AUTHRET_NONE)
        .value("AUTHRET_OVERTIME", AUTHRET_OVERTIME)
        .value("AUTHRET_NETWORKISSUE", AUTHRET_NETWORKISSUE)
        .value("AUTHRET_CLIENT_INCOMPATIBLE", AUTHRET_CLIENT_INCOMPATIBLE)
        .value("AUTHRET_JWTTOKENWRONG", AUTHRET_JWTTOKENWRONG)
        .value("AUTHRET_LIMIT_EXCEEDED_EXCEPTION", AUTHRET_LIMIT_EXCEEDED_EXCEPTION)
        .export_values();   

    nb::class_<IAuthService>(m, "IAuthService")
        .def("SetEvent", &IAuthService::SetEvent)
        .def("SDKAuth", &IAuthService::SDKAuth)
        .def("GetAuthResult", &IAuthService::GetAuthResult)
        .def("GetSDKIdentity", &IAuthService::GetSDKIdentity)
        .def("GenerateSSOLoginWebURL", &IAuthService::GenerateSSOLoginWebURL)
        .def("SSOLoginWithWebUriProtocol", &IAuthService::SSOLoginWithWebUriProtocol)
        .def("LogOut", &IAuthService::LogOut)
        .def("GetLoginStatus", &IAuthService::GetLoginStatus);

    nb::class_<IAuthServiceEvent>(m, "IAuthServiceEvent")
    .def("onAuthenticationReturn", &IAuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &IAuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &IAuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &IAuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &IAuthServiceEvent::onZoomAuthIdentityExpired);

    nb::class_<AuthContext>(m, "AuthContext")
        .def(nb::init<>())
        .def_rw("jwt_token", &AuthContext::jwt_token);      
}