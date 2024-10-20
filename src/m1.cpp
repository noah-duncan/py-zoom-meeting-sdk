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


void printJoinParam(const JoinParam& param) {
    std::cout << "JoinParam contents:" << std::endl;
    std::cout << "User Type: " << (param.userType == SDK_UT_NORMALUSER ? "Normal User" : "Without Login") << std::endl;

    if (param.userType == SDK_UT_NORMALUSER) {
        const auto& np = param.param.normaluserJoin;
        std::cout << "Normal User Join Parameters:" << std::endl;
        std::cout << "  Meeting Number: " << np.meetingNumber << std::endl;
        std::cout << "  Vanity ID: " << (np.vanityID ? np.vanityID : "NULL") << std::endl;
        std::cout << "  User Name: " << (np.userName ? np.userName : "NULL") << std::endl;
        std::cout << "  Password: " << (np.psw ? np.psw : "NULL") << std::endl;
        std::cout << "  App Privilege Token: " << (np.app_privilege_token ? np.app_privilege_token : "NULL") << std::endl;
        std::cout << "  Customer Key: " << (np.customer_key ? np.customer_key : "NULL") << std::endl;
        std::cout << "  Webinar Token: " << (np.webinarToken ? np.webinarToken : "NULL") << std::endl;
        std::cout << "  Video Off: " << (np.isVideoOff ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Off: " << (np.isAudioOff ? "Yes" : "No") << std::endl;
        std::cout << "  Join Token: " << (np.join_token ? np.join_token : "NULL") << std::endl;
        std::cout << "  Is My Voice In Mix: " << (np.isMyVoiceInMix ? "Yes" : "No") << std::endl;
        std::cout << "  Is Audio Raw Data Stereo: " << (np.isAudioRawDataStereo ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Raw Data Sampling Rate: " << np.eAudioRawdataSamplingRate << std::endl;
    } else {
        const auto& wp = param.param.withoutloginuserJoin;
        std::cout << "zzzWithout Login User Join Parameters:" << std::endl;
        std::cout << "  Meeting Number: " << wp.meetingNumber << std::endl;
        std::cout << "  Vanity ID: " << (wp.vanityID ? wp.vanityID : "NULL") << std::endl;
        std::cout << "  User Name: " << (wp.userName ? wp.userName : "NULL") << std::endl;
        std::cout << "  Password: " << (wp.psw ? wp.psw : "NULL") << std::endl;
        std::cout << "  App Privilege Token: " << (wp.app_privilege_token ? wp.app_privilege_token : "NULL") << std::endl;
        std::cout << "  User ZAK: " << (wp.userZAK ? wp.userZAK : "NULL") << std::endl;
        std::cout << "  Customer Key: " << (wp.customer_key ? wp.customer_key : "NULL") << std::endl;
        std::cout << "  Webinar Token: " << (wp.webinarToken ? wp.webinarToken : "NULL") << std::endl;
        std::cout << "  Video Off: " << (wp.isVideoOff ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Off: " << (wp.isAudioOff ? "Yes" : "No") << std::endl;
        std::cout << "  Join Token: " << (wp.join_token ? wp.join_token : "NULL") << std::endl;
        std::cout << "  Is My Voice In Mix: " << (wp.isMyVoiceInMix ? "Yes" : "No") << std::endl;
        std::cout << "  Is Audio Raw Data Stereo: " << (wp.isAudioRawDataStereo ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Rawqq pData Sampling Rate: " << wp.eAudioRawdataSamplingRate << std::endl;
    }
}

void init_m1(nb::module_ &m) {
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

    m.def("CleanUPSDK", &CleanUPSDK, nb::call_guard<nb::gil_scoped_release>());

    nb::enum_<ZOOM_SDK_NAMESPACE::SDKUserType>(m, "SDKUserType")
        .value("SDK_UT_NORMALUSER", ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER)
        .value("SDK_UT_WITHOUT_LOGIN", ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::StartParam4NormalUser>(m, "StartParam4NormalUser")
    .def(nb::init<>())
    .def_rw("meetingNumber", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::meetingNumber)
    .def_rw("vanityID", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::vanityID)
    .def_rw("customer_key", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::customer_key)
    .def_rw("isAudioOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isAudioOff)
    .def_rw("isVideoOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isVideoOff);

    nb::class_<ZOOM_SDK_NAMESPACE::StartParam>(m, "StartParam")
    .def(nb::init<>())
    .def_rw("userType", &ZOOM_SDK_NAMESPACE::StartParam::userType)
    .def_prop_rw("normaluserStart",
        [](ZOOM_SDK_NAMESPACE::StartParam &p) { return p.param.normaluserStart; },
        [](ZOOM_SDK_NAMESPACE::StartParam &p, const ZOOM_SDK_NAMESPACE::StartParam4NormalUser &normalUser) { p.param.normaluserStart = normalUser; }
    );

    nb::class_<ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin>(m, "JoinParam4WithoutLogin")
        .def(nb::init<>())
        .def_rw("meetingNumber", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::meetingNumber)
        .def_rw("userName", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::userName)
        .def_rw("psw", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::psw)
        .def_prop_rw("vanityID",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.vanityID ? std::string(p.vanityID) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.vanityID = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("customer_key",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.customer_key ? std::string(p.customer_key) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.customer_key = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("webinarToken",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.webinarToken ? std::string(p.webinarToken) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.webinarToken = s.empty() ? nullptr : s.c_str(); })
        .def_rw("isVideoOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isVideoOff)
        .def_rw("isAudioOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isAudioOff)
        .def_prop_rw("userZAK",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.userZAK ? std::string(p.userZAK) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.userZAK = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("app_privilege_token",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.app_privilege_token ? std::string(p.app_privilege_token) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.app_privilege_token = s.empty() ? nullptr : s.c_str(); });

    nb::class_<ZOOM_SDK_NAMESPACE::JoinParam>(m, "JoinParam")
    .def(nb::init<>())
    .def_rw("userType", &ZOOM_SDK_NAMESPACE::JoinParam::userType)
    .def_prop_rw("param",
        [](ZOOM_SDK_NAMESPACE::JoinParam &p) -> ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin& { return p.param.withoutloginuserJoin; },
        [](ZOOM_SDK_NAMESPACE::JoinParam &p, const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &jp) { 
            std::cout << "meetingNumber " << jp.meetingNumber << std::endl; 
            std::cout << "mn" << jp.meetingNumber << std::endl;
            p.param.withoutloginuserJoin = jp; 
        }
    );

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


    nb::class_<IMeetingService>(m, "IMeetingService")
    .def("GetMeetingStatus", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingStatus)
    .def("Join", [](ZOOM_SDK_NAMESPACE::IMeetingService& self, ZOOM_SDK_NAMESPACE::JoinParam& param) {
        printJoinParam(param);
        return self.Join(param);
    })
    .def("SetEvent", &ZOOM_SDK_NAMESPACE::IMeetingService::SetEvent)
    .def("Start", &ZOOM_SDK_NAMESPACE::IMeetingService::Start)
    .def("Leave", &ZOOM_SDK_NAMESPACE::IMeetingService::Leave, nb::call_guard<nb::gil_scoped_release>())
    .def("GetMeetingRecordingController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingRecordingController, nb::rv_policy::reference)
    .def("GetMeetingReminderController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingReminderController, nb::rv_policy::reference);
       
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

    nb::class_<ISettingService>(m, "ISettingService")
    .def("GetGeneralSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetGeneralSettings, nb::rv_policy::reference)
    .def("GetAudioSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetAudioSettings, nb::rv_policy::reference)
    .def("GetVideoSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetVideoSettings, nb::rv_policy::reference)
    .def("GetRecordingSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetRecordingSettings, nb::rv_policy::reference)
    .def("GetStatisticSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetStatisticSettings, nb::rv_policy::reference)
    .def("GetShareSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetShareSettings, nb::rv_policy::reference);

    m.def("CreateSettingService", []() -> ZOOM_SDK_NAMESPACE::ISettingService* {
        ZOOM_SDK_NAMESPACE::ISettingService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateSettingService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create setting service");
        }
        return pService;
    }, nb::rv_policy::reference);

    m.def("DestroySettingService", &DestroySettingService);

    nb::class_<IAuthService>(m, "IAuthService")
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IAuthService::SetEvent)
        .def("SDKAuth", &ZOOM_SDK_NAMESPACE::IAuthService::SDKAuth)
        .def("GetAuthResult", &ZOOM_SDK_NAMESPACE::IAuthService::GetAuthResult)
        .def("GetSDKIdentity", &ZOOM_SDK_NAMESPACE::IAuthService::GetSDKIdentity)
        .def("GenerateSSOLoginWebURL", &ZOOM_SDK_NAMESPACE::IAuthService::GenerateSSOLoginWebURL)
        .def("SSOLoginWithWebUriProtocol", &ZOOM_SDK_NAMESPACE::IAuthService::SSOLoginWithWebUriProtocol)
        .def("LogOut", &ZOOM_SDK_NAMESPACE::IAuthService::LogOut)
        .def("GetLoginStatus", &ZOOM_SDK_NAMESPACE::IAuthService::GetLoginStatus);

    m.def("CreateAuthService", []() -> ZOOM_SDK_NAMESPACE::IAuthService* {
        ZOOM_SDK_NAMESPACE::IAuthService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateAuthService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create auth service");
        }
        return pService;
    }, nb::rv_policy::reference);

    m.def("DestroyAuthService", &DestroyAuthService);

    nb::class_<IAuthServiceEvent>(m, "IAuthServiceEvent")
    .def("onAuthenticationReturn", &IAuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &IAuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &IAuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &IAuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &IAuthServiceEvent::onZoomAuthIdentityExpired);

    nb::class_<AuthServiceEvent, IAuthServiceEvent>(m, "AuthServiceEvent")
    .def(nb::init<std::function<void()>&>())
    .def("onAuthenticationReturn", &AuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &AuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &AuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &AuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &AuthServiceEvent::onZoomAuthIdentityExpired)
    .def("setOnAuth", &AuthServiceEvent::setOnAuth)
    .def("setOnAuthenticationReturn", &AuthServiceEvent::setOnAuthenticationReturn);

    nb::class_<AuthContext>(m, "AuthContext")
    .def(nb::init<>())
    .def_rw("jwt_token", &AuthContext::jwt_token);

    nb::class_<IMeetingServiceEvent>(m, "IMeetingServiceEvent")
    .def("onMeetingStatusChanged", &IMeetingServiceEvent::onMeetingStatusChanged)
    .def("onMeetingStatisticsWarningNotification", &IMeetingServiceEvent::onMeetingStatisticsWarningNotification)
    .def("onMeetingParameterNotification", &IMeetingServiceEvent::onMeetingParameterNotification)
    .def("onSuspendParticipantsActivities", &IMeetingServiceEvent::onSuspendParticipantsActivities)
    .def("onAICompanionActiveChangeNotice", &IMeetingServiceEvent::onAICompanionActiveChangeNotice)
    .def("onMeetingTopicChanged", &IMeetingServiceEvent::onMeetingTopicChanged);

    nb::class_<MeetingServiceEvent, IMeetingServiceEvent>(m, "MeetingServiceEvent")
    .def(nb::init<>())
    .def("onMeetingStatusChanged", &MeetingServiceEvent::onMeetingStatusChanged)
    .def("onMeetingParameterNotification", &MeetingServiceEvent::onMeetingParameterNotification)
    .def("onMeetingStatisticsWarningNotification", &MeetingServiceEvent::onMeetingStatisticsWarningNotification)
    .def("onSuspendParticipantsActivities", &MeetingServiceEvent::onSuspendParticipantsActivities)
    .def("onAICompanionActiveChangeNotice", &MeetingServiceEvent::onAICompanionActiveChangeNotice)
    .def("onMeetingTopicChanged", &MeetingServiceEvent::onMeetingTopicChanged)
    .def("setOnMeetingJoin", &MeetingServiceEvent::setOnMeetingJoin)
    .def("setOnMeetingEnd", &MeetingServiceEvent::setOnMeetingEnd)
    .def("setOnMeetingStatusChanged", &MeetingServiceEvent::setOnMeetingStatusChanged);

    nb::enum_<ZOOM_SDK_NAMESPACE::LeaveMeetingCmd>(m, "LeaveMeetingCmd")
    .value("LEAVE_MEETING", ZOOM_SDK_NAMESPACE::LEAVE_MEETING)
    .value("END_MEETING", ZOOM_SDK_NAMESPACE::END_MEETING)
    .export_values();

    nb::enum_<MeetingEndReason>(m, "MeetingEndReason")
    .value("EndMeetingReason_None", EndMeetingReason_None)
    .value("EndMeetingReason_KickByHost", EndMeetingReason_KickByHost)
    .value("EndMeetingReason_EndByHost", EndMeetingReason_EndByHost)
    .value("EndMeetingReason_JBHTimeOut", EndMeetingReason_JBHTimeOut)
    .value("EndMeetingReason_NoAttendee", EndMeetingReason_NoAttendee)
    .value("EndMeetingReason_HostStartAnotherMeeting", EndMeetingReason_HostStartAnotherMeeting)
    .value("EndMeetingReason_FreeMeetingTimeOut", EndMeetingReason_FreeMeetingTimeOut)
    .value("EndMeetingReason_NetworkBroken", EndMeetingReason_NetworkBroken);

    nb::enum_<ZOOM_SDK_NAMESPACE::MeetingStatus>(m, "MeetingStatus")
    .value("MEETING_STATUS_IDLE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_IDLE)
    .value("MEETING_STATUS_CONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_CONNECTING)
    .value("MEETING_STATUS_WAITINGFORHOST", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WAITINGFORHOST)
    .value("MEETING_STATUS_INMEETING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_INMEETING)
    .value("MEETING_STATUS_DISCONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_DISCONNECTING)
    .value("MEETING_STATUS_RECONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_RECONNECTING)
    .value("MEETING_STATUS_FAILED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_FAILED)
    .value("MEETING_STATUS_ENDED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_ENDED)
    .value("MEETING_STATUS_UNKNOWN", ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNKNOWN)
    .value("MEETING_STATUS_LOCKED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_LOCKED)
    .value("MEETING_STATUS_UNLOCKED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNLOCKED)
    .value("MEETING_STATUS_IN_WAITING_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_IN_WAITING_ROOM)
    .value("MEETING_STATUS_WEBINAR_PROMOTE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_PROMOTE)
    .value("MEETING_STATUS_WEBINAR_DEPROMOTE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_DEPROMOTE)
    .value("MEETING_STATUS_JOIN_BREAKOUT_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_JOIN_BREAKOUT_ROOM)
    .value("MEETING_STATUS_LEAVE_BREAKOUT_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_LEAVE_BREAKOUT_ROOM)
    .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::StatisticsWarningType>(m, "StatisticsWarningType")
    .value("Statistics_Warning_None", ZOOM_SDK_NAMESPACE::Statistics_Warning_None)
    .value("Statistics_Warning_Network_Quality_Bad", ZOOM_SDK_NAMESPACE::Statistics_Warning_Network_Quality_Bad)
    .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::MeetingType>(m, "MeetingType")
    .value("MEETING_TYPE_NONE", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NONE)
    .value("MEETING_TYPE_NORMAL", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NORMAL)
    .value("MEETING_TYPE_WEBINAR", ZOOM_SDK_NAMESPACE::MEETING_TYPE_WEBINAR)
    .value("MEETING_TYPE_BREAKOUTROOM", ZOOM_SDK_NAMESPACE::MEETING_TYPE_BREAKOUTROOM)
    .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::MeetingParameter>(m, "MeetingParameter")
    .def(nb::init<>())
    .def_rw("meeting_type", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_type)
    .def_rw("is_view_only", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_view_only)
    .def_rw("is_auto_recording_local", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_local)
    .def_rw("is_auto_recording_cloud", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_cloud)
    .def_rw("meeting_number", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_number)
    .def_rw("meeting_topic", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_topic)
    .def_rw("meeting_host", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_host);

    // Binding for IMeetingRecordingCtrlEvent
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

    // Binding for MeetingRecordingCtrlEvent
    nb::class_<MeetingRecordingCtrlEvent, IMeetingRecordingCtrlEvent>(m, "MeetingRecordingCtrlEvent")
        .def(nb::init<std::function<void(bool)>>())
        .def("onRecordingStatus", &MeetingRecordingCtrlEvent::onRecordingStatus)
        .def("onCloudRecordingStatus", &MeetingRecordingCtrlEvent::onCloudRecordingStatus)
        .def("onRecordPrivilegeChanged", &MeetingRecordingCtrlEvent::onRecordPrivilegeChanged)
        .def("onLocalRecordingPrivilegeRequestStatus", &MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequestStatus)
        .def("onLocalRecordingPrivilegeRequested", &MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequested)
        .def("onCloudRecordingStorageFull", &MeetingRecordingCtrlEvent::onCloudRecordingStorageFull)
        .def("onRequestCloudRecordingResponse", &MeetingRecordingCtrlEvent::onRequestCloudRecordingResponse)
        .def("onStartCloudRecordingRequested", &MeetingRecordingCtrlEvent::onStartCloudRecordingRequested)
        .def("onEnableAndStartSmartRecordingRequested", &MeetingRecordingCtrlEvent::onEnableAndStartSmartRecordingRequested)
        .def("onSmartRecordingEnableActionCallback", &MeetingRecordingCtrlEvent::onSmartRecordingEnableActionCallback)
        .def("onTranscodingStatusChanged", &MeetingRecordingCtrlEvent::onTranscodingStatusChanged);


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

    // Binding for IMeetingReminderEvent
    nb::class_<IMeetingReminderEvent>(m, "IMeetingReminderEvent")
        .def("onReminderNotify", &IMeetingReminderEvent::onReminderNotify)
        .def("onEnableReminderNotify", &IMeetingReminderEvent::onEnableReminderNotify);

    // Binding for MeetingReminderEvent
    nb::class_<MeetingReminderEvent, IMeetingReminderEvent>(m, "MeetingReminderEvent")
        .def(nb::init<>())
        .def("onReminderNotify", &MeetingReminderEvent::onReminderNotify)
        .def("onEnableReminderNotify", &MeetingReminderEvent::onEnableReminderNotify);
        
    // Binding for IMeetingReminderController  
    nb::class_<IMeetingReminderController>(m, "IMeetingReminderController")
        .def("SetEvent", &IMeetingReminderController::SetEvent);

}