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
#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"
#include "raw_record/ZoomSDKAudioRawDataDelegate.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

void init_auth_service_interface_binding(nb::module_ &m) {
    nb::enum_<ZOOM_SDK_NAMESPACE::LoginType>(m, "LoginType")
        .value("LoginType_Unknown", ZOOM_SDK_NAMESPACE::LoginType_Unknown)
        .value("LoginType_SSO", ZOOM_SDK_NAMESPACE::LoginType_SSO)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::IAccountInfo>(m, "IAccountInfo")
        .def("GetDisplayName", &ZOOM_SDK_NAMESPACE::IAccountInfo::GetDisplayName)
        .def("GetLoginType", &ZOOM_SDK_NAMESPACE::IAccountInfo::GetLoginType);

    nb::enum_<ZOOM_SDK_NAMESPACE::LoginFailReason>(m, "LoginFailReason")
        .value("LoginFail_None", ZOOM_SDK_NAMESPACE::LoginFail_None)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::LOGINSTATUS>(m, "LOGINSTATUS")
        .value("LOGIN_IDLE", ZOOM_SDK_NAMESPACE::LOGIN_IDLE)
        .value("LOGIN_PROCESSING", ZOOM_SDK_NAMESPACE::LOGIN_PROCESSING)
        .value("LOGIN_SUCCESS", ZOOM_SDK_NAMESPACE::LOGIN_SUCCESS)
        .value("LOGIN_FAILED", ZOOM_SDK_NAMESPACE::LOGIN_FAILED)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::AuthResult>(m, "AuthResult")
        .value("AUTHRET_SUCCESS", ZOOM_SDK_NAMESPACE::AUTHRET_SUCCESS)
        .value("AUTHRET_KEYORSECRETEMPTY", ZOOM_SDK_NAMESPACE::AUTHRET_KEYORSECRETEMPTY)
        .value("AUTHRET_KEYORSECRETWRONG", ZOOM_SDK_NAMESPACE::AUTHRET_KEYORSECRETWRONG)
        .value("AUTHRET_ACCOUNTNOTSUPPORT", ZOOM_SDK_NAMESPACE::AUTHRET_ACCOUNTNOTSUPPORT)
        .value("AUTHRET_ACCOUNTNOTENABLESDK", ZOOM_SDK_NAMESPACE::AUTHRET_ACCOUNTNOTENABLESDK)
        .value("AUTHRET_UNKNOWN", ZOOM_SDK_NAMESPACE::AUTHRET_UNKNOWN)
        .value("AUTHRET_SERVICE_BUSY", ZOOM_SDK_NAMESPACE::AUTHRET_SERVICE_BUSY)
        .value("AUTHRET_NONE", ZOOM_SDK_NAMESPACE::AUTHRET_NONE)
        .value("AUTHRET_OVERTIME", ZOOM_SDK_NAMESPACE::AUTHRET_OVERTIME)
        .value("AUTHRET_NETWORKISSUE", ZOOM_SDK_NAMESPACE::AUTHRET_NETWORKISSUE)
        .value("AUTHRET_CLIENT_INCOMPATIBLE", ZOOM_SDK_NAMESPACE::AUTHRET_CLIENT_INCOMPATIBLE)
        .value("AUTHRET_JWTTOKENWRONG", ZOOM_SDK_NAMESPACE::AUTHRET_JWTTOKENWRONG)
        .value("AUTHRET_LIMIT_EXCEEDED_EXCEPTION", ZOOM_SDK_NAMESPACE::AUTHRET_LIMIT_EXCEEDED_EXCEPTION)
        .export_values();   

    nb::class_<IAuthService>(m, "IAuthService")
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IAuthService::SetEvent)
        .def("SDKAuth", &ZOOM_SDK_NAMESPACE::IAuthService::SDKAuth)
        .def("GetAuthResult", &ZOOM_SDK_NAMESPACE::IAuthService::GetAuthResult)
        .def("GetSDKIdentity", &ZOOM_SDK_NAMESPACE::IAuthService::GetSDKIdentity)
        .def("GenerateSSOLoginWebURL", &ZOOM_SDK_NAMESPACE::IAuthService::GenerateSSOLoginWebURL)
        .def("SSOLoginWithWebUriProtocol", &ZOOM_SDK_NAMESPACE::IAuthService::SSOLoginWithWebUriProtocol)
        .def("LogOut", &ZOOM_SDK_NAMESPACE::IAuthService::LogOut)
        .def("GetLoginStatus", &ZOOM_SDK_NAMESPACE::IAuthService::GetLoginStatus);

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