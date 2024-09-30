#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include "zoom_sdk.h"

#include "meeting_service_interface.h"
#include "setting_service_interface.h"
#include "auth_service_interface.h"
#include "meeting_service_components/meeting_recording_interface.h"
#include "meeting_service_components/meeting_reminder_ctrl_interface.h"
#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"
#include <iostream>
#include <functional>

namespace py = pybind11;

class PyAuthServiceEvent : public ZOOM_SDK_NAMESPACE::IAuthServiceEvent {
public:
    using ZOOM_SDK_NAMESPACE::IAuthServiceEvent::IAuthServiceEvent; // Inherit constructors

    void onAuthenticationReturn(ZOOM_SDK_NAMESPACE::AuthResult ret) override {
        PYBIND11_OVERLOAD_PURE(void, ZOOM_SDK_NAMESPACE::IAuthServiceEvent, onAuthenticationReturn, ret);
    }

    void onLoginReturnWithReason(ZOOM_SDK_NAMESPACE::LOGINSTATUS ret, ZOOM_SDK_NAMESPACE::IAccountInfo* pAccountInfo, ZOOM_SDK_NAMESPACE::LoginFailReason reason) override {
        PYBIND11_OVERLOAD_PURE(void, ZOOM_SDK_NAMESPACE::IAuthServiceEvent, onLoginReturnWithReason, ret, pAccountInfo, reason);
    }

    void onLogout() override {
        PYBIND11_OVERLOAD_PURE(void, ZOOM_SDK_NAMESPACE::IAuthServiceEvent, onLogout);
    }

    void onZoomIdentityExpired() override {
        PYBIND11_OVERLOAD_PURE(void, ZOOM_SDK_NAMESPACE::IAuthServiceEvent, onZoomIdentityExpired);
    }

    void onZoomAuthIdentityExpired() override {
        PYBIND11_OVERLOAD_PURE(void, ZOOM_SDK_NAMESPACE::IAuthServiceEvent, onZoomAuthIdentityExpired);
    }
};

PYBIND11_MODULE(zoom_sdk_python, m) {
    m.doc() = "Python bindings for Zoom SDK";

py::enum_<ZOOM_SDK_NAMESPACE::LoginType>(m, "LoginType")
    .value("LoginType_Unknown", ZOOM_SDK_NAMESPACE::LoginType_Unknown)
    .value("LoginType_SSO", ZOOM_SDK_NAMESPACE::LoginType_SSO)
    .export_values();

py::class_<ZOOM_SDK_NAMESPACE::IAccountInfo>(m, "IAccountInfo")
    .def("GetDisplayName", &ZOOM_SDK_NAMESPACE::IAccountInfo::GetDisplayName)
    .def("GetLoginType", &ZOOM_SDK_NAMESPACE::IAccountInfo::GetLoginType);

py::enum_<ZOOM_SDK_NAMESPACE::LoginFailReason>(m, "LoginFailReason")
    .value("LoginFail_None", ZOOM_SDK_NAMESPACE::LoginFail_None)
    .export_values();

py::enum_<ZOOM_SDK_NAMESPACE::LOGINSTATUS>(m, "LOGINSTATUS")
    .value("LOGIN_IDLE", ZOOM_SDK_NAMESPACE::LOGIN_IDLE)
    .value("LOGIN_PROCESSING", ZOOM_SDK_NAMESPACE::LOGIN_PROCESSING)
    .value("LOGIN_SUCCESS", ZOOM_SDK_NAMESPACE::LOGIN_SUCCESS)
    .value("LOGIN_FAILED", ZOOM_SDK_NAMESPACE::LOGIN_FAILED)
    .export_values();

py::enum_<ZOOM_SDK_NAMESPACE::SDKError>(m, "SDKError")
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

    py::enum_<ZOOM_SDK_NAMESPACE::SDK_LANGUAGE_ID>(m, "SDK_LANGUAGE_ID")
        .value("LANGUAGE_Unknown", ZOOM_SDK_NAMESPACE::LANGUAGE_Unknown)
        .value("LANGUAGE_English", ZOOM_SDK_NAMESPACE::LANGUAGE_English)
        // Add other enum values here
        .export_values();

    py::class_<ZOOM_SDK_NAMESPACE::InitParam>(m, "InitParam")
        .def(py::init<>())
        .def_property("strWebDomain", 
            [](const ZOOM_SDK_NAMESPACE::InitParam &p) { return p.strWebDomain ? py::str(p.strWebDomain) : py::str(""); },
            [](ZOOM_SDK_NAMESPACE::InitParam &p, const std::string &s) {
                static std::string temp;
                temp = s;
                p.strWebDomain = temp.c_str();
            })
        .def_property("strBrandingName", 
            [](const ZOOM_SDK_NAMESPACE::InitParam &p) { return p.strBrandingName ? py::str(p.strBrandingName) : py::str(""); },
            [](ZOOM_SDK_NAMESPACE::InitParam &p, const std::string &s) {
                static std::string temp;
                temp = s;
                p.strBrandingName = temp.c_str();
            })
        .def_property("strSupportUrl", 
            [](const ZOOM_SDK_NAMESPACE::InitParam &p) { return p.strSupportUrl ? py::str(p.strSupportUrl) : py::str(""); },
            [](ZOOM_SDK_NAMESPACE::InitParam &p, const std::string &s) {
                static std::string temp;
                temp = s;
                p.strSupportUrl = temp.c_str();
            })
        .def_readwrite("emLanguageID", &ZOOM_SDK_NAMESPACE::InitParam::emLanguageID)
        .def_readwrite("enableGenerateDump", &ZOOM_SDK_NAMESPACE::InitParam::enableGenerateDump)
        .def_readwrite("enableLogByDefault", &ZOOM_SDK_NAMESPACE::InitParam::enableLogByDefault)
        .def_readwrite("uiLogFileSize", &ZOOM_SDK_NAMESPACE::InitParam::uiLogFileSize);

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
    }, py::arg("initParam"), "Initialize ZOOM SDK");

    py::enum_<ZOOM_SDK_NAMESPACE::SDKUserType>(m, "SDKUserType")
        .value("SDK_UT_NORMALUSER", ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER)
        .value("SDK_UT_WITHOUT_LOGIN", ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN)
        .export_values();

    py::class_<ZOOM_SDK_NAMESPACE::StartParam4NormalUser>(m, "StartParam4NormalUser")
        .def(py::init<>())
        .def_readwrite("meetingNumber", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::meetingNumber)
        .def_property("vanityID",
            [](const ZOOM_SDK_NAMESPACE::StartParam4NormalUser &p) { return p.vanityID ? std::string(p.vanityID) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::StartParam4NormalUser &p, const std::string &s) { p.vanityID = s.empty() ? nullptr : s.c_str(); }
        )
        .def_property("customer_key",
            [](const ZOOM_SDK_NAMESPACE::StartParam4NormalUser &p) { return p.customer_key ? std::string(p.customer_key) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::StartParam4NormalUser &p, const std::string &s) { p.customer_key = s.empty() ? nullptr : s.c_str(); }
        )
        .def_readwrite("isAudioOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isAudioOff)
        .def_readwrite("isVideoOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isVideoOff);

    py::class_<ZOOM_SDK_NAMESPACE::StartParam>(m, "StartParam")
        .def(py::init<>())
        .def_readwrite("userType", &ZOOM_SDK_NAMESPACE::StartParam::userType)
        .def_property("normaluserStart",
            [](ZOOM_SDK_NAMESPACE::StartParam &p) { return p.param.normaluserStart; },
            [](ZOOM_SDK_NAMESPACE::StartParam &p, const ZOOM_SDK_NAMESPACE::StartParam4NormalUser &normalUser) { p.param.normaluserStart = normalUser; }
        );

    py::class_<ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin>(m, "JoinParam4WithoutLogin")
        .def(py::init<>())
        .def_readwrite("meetingNumber", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::meetingNumber)
        .def_property("userName",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &self) -> std::string {
                return self.userName ? self.userName : "";
            },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &self, const std::string &userName) {
                static std::string stored_userName;
                stored_userName = userName;
                self.userName = stored_userName.c_str();
            }
        )
        .def_property("psw",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &self) -> std::string {
                return self.psw ? self.psw : "";
            },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &self, const std::string &psw) {
                static std::string stored_psw;
                stored_psw = psw;
                self.psw = stored_psw.c_str();
            }
        )
        .def_property("vanityID",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.vanityID ? std::string(p.vanityID) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.vanityID = s.empty() ? nullptr : s.c_str(); })
        .def_property("customer_key",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.customer_key ? std::string(p.customer_key) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.customer_key = s.empty() ? nullptr : s.c_str(); })
        .def_property("webinarToken",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.webinarToken ? std::string(p.webinarToken) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.webinarToken = s.empty() ? nullptr : s.c_str(); })
        .def_readwrite("isVideoOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isVideoOff)
        .def_readwrite("isAudioOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isAudioOff)
        .def_property("userZAK",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.userZAK ? std::string(p.userZAK) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.userZAK = s.empty() ? nullptr : s.c_str(); })
        .def_property("app_privilege_token",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.app_privilege_token ? std::string(p.app_privilege_token) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.app_privilege_token = s.empty() ? nullptr : s.c_str(); });

    py::class_<ZOOM_SDK_NAMESPACE::JoinParam>(m, "JoinParam")
        .def(py::init<>())
        .def_readwrite("userType", &ZOOM_SDK_NAMESPACE::JoinParam::userType)
        .def_property("param",
            [](ZOOM_SDK_NAMESPACE::JoinParam &p) -> ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin& {
                return p.param.withoutloginuserJoin;
            },
            [](ZOOM_SDK_NAMESPACE::JoinParam &p, const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &jp) {
                p.param.withoutloginuserJoin = jp;
            }
        );

    py::enum_<ZOOM_SDK_NAMESPACE::AuthResult>(m, "AuthResult")
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

    // Binding IMeetingService as an abstract base class
    py::class_<ZOOM_SDK_NAMESPACE::IMeetingService, std::unique_ptr<ZOOM_SDK_NAMESPACE::IMeetingService, py::nodelete>>(m, "IMeetingService")
        .def("GetMeetingStatus", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingStatus)
        .def("Join", [](ZOOM_SDK_NAMESPACE::IMeetingService& self, ZOOM_SDK_NAMESPACE::JoinParam& param) {
            return self.Join(param);
        })
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IMeetingService::SetEvent)
        .def("Start", &ZOOM_SDK_NAMESPACE::IMeetingService::Start)
        .def("Leave", &ZOOM_SDK_NAMESPACE::IMeetingService::Leave)
        .def("GetMeetingRecordingController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingRecordingController, py::return_value_policy::reference)
        .def("GetMeetingReminderController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingReminderController, py::return_value_policy::reference);
        // Add other methods as needed

    // Binding for CreateMeetingService
    m.def("CreateMeetingService", []() {
        ZOOM_SDK_NAMESPACE::IMeetingService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateMeetingService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create meeting service");
        }
        return std::unique_ptr<ZOOM_SDK_NAMESPACE::IMeetingService, py::nodelete>(pService);
    }, "Create a new meeting service instance");

    // Binding for DestroyMeetingService
    m.def("DestroyMeetingService", [](ZOOM_SDK_NAMESPACE::IMeetingService* pService) {
        return ZOOM_SDK_NAMESPACE::DestroyMeetingService(pService);
    }, "Destroy a meeting service instance");

    py::enum_<ZOOM_SDK_NAMESPACE::MeetingStatus>(m, "MeetingStatus")
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

    py::enum_<ZOOM_SDK_NAMESPACE::StatisticsWarningType>(m, "StatisticsWarningType")
        .value("Statistics_Warning_None", ZOOM_SDK_NAMESPACE::Statistics_Warning_None)
        .value("Statistics_Warning_Network_Quality_Bad", ZOOM_SDK_NAMESPACE::Statistics_Warning_Network_Quality_Bad)
        .export_values();

    py::enum_<ZOOM_SDK_NAMESPACE::MeetingType>(m, "MeetingType")
    .value("MEETING_TYPE_NONE", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NONE)
    .value("MEETING_TYPE_NORMAL", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NORMAL)
    .value("MEETING_TYPE_WEBINAR", ZOOM_SDK_NAMESPACE::MEETING_TYPE_WEBINAR)
    .value("MEETING_TYPE_BREAKOUTROOM", ZOOM_SDK_NAMESPACE::MEETING_TYPE_BREAKOUTROOM)
    .export_values();

    py::class_<ZOOM_SDK_NAMESPACE::MeetingParameter>(m, "MeetingParameter")
        .def(py::init<>())
        .def_readwrite("meeting_type", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_type)
        .def_readwrite("is_view_only", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_view_only)
        .def_readwrite("is_auto_recording_local", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_local)
        .def_readwrite("is_auto_recording_cloud", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_cloud)
        .def_readwrite("meeting_number", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_number)
        .def_property("meeting_topic",
            [](const ZOOM_SDK_NAMESPACE::MeetingParameter &p) { return p.meeting_topic ? py::str(p.meeting_topic) : py::str(""); },
            [](ZOOM_SDK_NAMESPACE::MeetingParameter &p, const std::string &s) {
                static std::string temp;
                temp = s;
                p.meeting_topic = temp.c_str();
            })
        .def_property("meeting_host",
            [](const ZOOM_SDK_NAMESPACE::MeetingParameter &p) { return p.meeting_host ? py::str(p.meeting_host) : py::str(""); },
            [](ZOOM_SDK_NAMESPACE::MeetingParameter &p, const std::string &s) {
                static std::string temp;
                temp = s;
                p.meeting_host = temp.c_str();
            });

    py::class_<IMeetingServiceEvent, std::shared_ptr<IMeetingServiceEvent>>(m, "IMeetingServiceEvent")
        .def("onMeetingStatusChanged", &IMeetingServiceEvent::onMeetingStatusChanged, py::arg("status"), py::arg("iResult") = 0)
        .def("onMeetingStatisticsWarningNotification", &IMeetingServiceEvent::onMeetingStatisticsWarningNotification)
        .def("onMeetingParameterNotification", &IMeetingServiceEvent::onMeetingParameterNotification)
        .def("onSuspendParticipantsActivities", &IMeetingServiceEvent::onSuspendParticipantsActivities)
        .def("onAICompanionActiveChangeNotice", &IMeetingServiceEvent::onAICompanionActiveChangeNotice)
        .def("onMeetingTopicChanged", &IMeetingServiceEvent::onMeetingTopicChanged);

    py::class_<MeetingServiceEvent, ZOOM_SDK_NAMESPACE::IMeetingServiceEvent, std::shared_ptr<MeetingServiceEvent>>(m, "MeetingServiceEvent")
        .def(py::init<>())
        .def("onMeetingStatusChanged", &MeetingServiceEvent::onMeetingStatusChanged)
        .def("onMeetingParameterNotification", &MeetingServiceEvent::onMeetingParameterNotification)
        .def("onMeetingStatisticsWarningNotification", &MeetingServiceEvent::onMeetingStatisticsWarningNotification)
        .def("onSuspendParticipantsActivities", &MeetingServiceEvent::onSuspendParticipantsActivities)
        .def("onAICompanionActiveChangeNotice", &MeetingServiceEvent::onAICompanionActiveChangeNotice)
        .def("onMeetingTopicChanged", &MeetingServiceEvent::onMeetingTopicChanged)
        .def("setOnMeetingJoin", &MeetingServiceEvent::setOnMeetingJoin)
        .def("setOnMeetingEnd", &MeetingServiceEvent::setOnMeetingEnd)
        .def("setOnMeetingStatusChanged", &MeetingServiceEvent::setOnMeetingStatusChanged);

    // Binding ISettingService as an abstract base class
    py::class_<ZOOM_SDK_NAMESPACE::ISettingService, std::unique_ptr<ZOOM_SDK_NAMESPACE::ISettingService, py::nodelete>>(m, "ISettingService")
    .def("GetGeneralSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetGeneralSettings, py::return_value_policy::reference)
    .def("GetAudioSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetAudioSettings, py::return_value_policy::reference)
    .def("GetVideoSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetVideoSettings, py::return_value_policy::reference)
    .def("GetRecordingSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetRecordingSettings, py::return_value_policy::reference)
    .def("GetStatisticSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetStatisticSettings, py::return_value_policy::reference)
    .def("GetShareSettings", &ZOOM_SDK_NAMESPACE::ISettingService::GetShareSettings, py::return_value_policy::reference);

    // Binding for CreateSettingService
    m.def("CreateSettingService", []() {
        ZOOM_SDK_NAMESPACE::ISettingService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateSettingService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create setting service");
        }
        return std::unique_ptr<ZOOM_SDK_NAMESPACE::ISettingService, py::nodelete>(pService);
    }, "Create a new setting service instance");

    // Binding for DestroySettingService
    m.def("DestroySettingService", [](ZOOM_SDK_NAMESPACE::ISettingService* pService) {
        return ZOOM_SDK_NAMESPACE::DestroySettingService(pService);
    }, "Destroy a setting service instance");

    // Binding for AuthContext
    py::class_<ZOOM_SDK_NAMESPACE::AuthContext>(m, "AuthContext")
        .def(py::init<>())
        .def_property("jwt_token",
            [](const ZOOM_SDK_NAMESPACE::AuthContext &self) -> std::string {
                return self.jwt_token ? self.jwt_token : "";
            },
            [](ZOOM_SDK_NAMESPACE::AuthContext &self, const std::string &token) {
                static std::string stored_token;
                stored_token = token;
                self.jwt_token = stored_token.c_str();
            }
        );

   // Binding for IMeetingRecordingCtrlEvent
   py::class_<IMeetingRecordingCtrlEvent, std::shared_ptr<IMeetingRecordingCtrlEvent>>(m, "IMeetingRecordingCtrlEvent")
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
    py::class_<MeetingRecordingCtrlEvent, IMeetingRecordingCtrlEvent, std::shared_ptr<MeetingRecordingCtrlEvent>>(m, "MeetingRecordingCtrlEvent")
        .def(py::init<std::function<void(bool)>>())
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

    py::class_<IMeetingRecordingController, std::shared_ptr<IMeetingRecordingController>>(m, "IMeetingRecordingController")
        .def("SetEvent", &IMeetingRecordingController::SetEvent)
        .def("IsSupportRequestLocalRecordingPrivilege", &IMeetingRecordingController::IsSupportRequestLocalRecordingPrivilege)
        .def("RequestLocalRecordingPrivilege", &IMeetingRecordingController::RequestLocalRecordingPrivilege)
        .def("RequestStartCloudRecording", &IMeetingRecordingController::RequestStartCloudRecording)
        .def("StartRecording", [](IMeetingRecordingController& self) {
            time_t startTimestamp;
            SDKError result = self.StartRecording(startTimestamp);
            return py::make_tuple(result, py::cast(startTimestamp));
        })
        .def("StopRecording", [](IMeetingRecordingController& self) {
            time_t stopTimestamp;
            SDKError result = self.StopRecording(stopTimestamp);
            return py::make_tuple(result, py::cast(stopTimestamp));
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
   
    py::class_<IMeetingReminderEvent, std::shared_ptr<IMeetingReminderEvent>>(m, "IMeetingReminderEvent")
        .def("onReminderNotify", &IMeetingReminderEvent::onReminderNotify)
        .def("onEnableReminderNotify", &IMeetingReminderEvent::onEnableReminderNotify);

    // Binding for IMeetingReminderEvent
    py::class_<MeetingReminderEvent, IMeetingReminderEvent, std::shared_ptr<MeetingReminderEvent>>(m, "MeetingReminderEvent")
        .def(py::init<>())
        .def("onReminderNotify", &MeetingReminderEvent::onReminderNotify)
        .def("onEnableReminderNotify", &MeetingReminderEvent::onEnableReminderNotify);
        
    // Binding for IMeetingReminderController  
    py::class_<IMeetingReminderController, std::shared_ptr<IMeetingReminderController>>(m, "IMeetingReminderController")
        .def("SetEvent", &IMeetingReminderController::SetEvent);

    // Binding for IAuthServiceEvent
    py::class_<IAuthServiceEvent, std::shared_ptr<IAuthServiceEvent>>(m, "IAuthServiceEvent")
        .def("onAuthenticationReturn", &IAuthServiceEvent::onAuthenticationReturn)
        .def("onLoginReturnWithReason", &IAuthServiceEvent::onLoginReturnWithReason)
        .def("onLogout", &IAuthServiceEvent::onLogout)
        .def("onZoomIdentityExpired", &IAuthServiceEvent::onZoomIdentityExpired)
        .def("onZoomAuthIdentityExpired", &IAuthServiceEvent::onZoomAuthIdentityExpired);

    // Binding for AuthServiceEvent (Not in SDK)
    py::class_<AuthServiceEvent, IAuthServiceEvent, std::shared_ptr<AuthServiceEvent>>(m, "AuthServiceEvent")
        .def(py::init<std::function<void()>&>())
        .def("onAuthenticationReturn", &AuthServiceEvent::onAuthenticationReturn)
        .def("onLoginReturnWithReason", &AuthServiceEvent::onLoginReturnWithReason)
        .def("onLogout", &AuthServiceEvent::onLogout)
        .def("onZoomIdentityExpired", &AuthServiceEvent::onZoomIdentityExpired)
        .def("onZoomAuthIdentityExpired", &AuthServiceEvent::onZoomAuthIdentityExpired)
        .def("setOnAuth", &AuthServiceEvent::setOnAuth)
        .def("setOnAuthenticationReturn", &AuthServiceEvent::setOnAuthenticationReturn);

    // Binding for IAuthService
    py::class_<ZOOM_SDK_NAMESPACE::IAuthService>(m, "IAuthService")
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IAuthService::SetEvent)
        .def("SDKAuth", &ZOOM_SDK_NAMESPACE::IAuthService::SDKAuth)
        .def("GetAuthResult", &ZOOM_SDK_NAMESPACE::IAuthService::GetAuthResult)
        .def("GetSDKIdentity", &ZOOM_SDK_NAMESPACE::IAuthService::GetSDKIdentity)
        .def("GenerateSSOLoginWebURL", &ZOOM_SDK_NAMESPACE::IAuthService::GenerateSSOLoginWebURL)
        .def("SSOLoginWithWebUriProtocol", &ZOOM_SDK_NAMESPACE::IAuthService::SSOLoginWithWebUriProtocol)
        .def("LogOut", &ZOOM_SDK_NAMESPACE::IAuthService::LogOut)
        .def("GetLoginStatus", &ZOOM_SDK_NAMESPACE::IAuthService::GetLoginStatus);

    // Binding for CreateAuthService
    m.def("CreateAuthService", []() {
        ZOOM_SDK_NAMESPACE::IAuthService* pService = nullptr;
        ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::CreateAuthService(&pService);
        if (err != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS) {
            throw std::runtime_error("Failed to create auth service");
        }
        return std::unique_ptr<ZOOM_SDK_NAMESPACE::IAuthService, py::nodelete>(pService);
    }, "Create a new auth service instance");

    // Binding for DestroyAuthService 
    m.def("DestroyAuthService", [](ZOOM_SDK_NAMESPACE::IAuthService* pService) {
        return ZOOM_SDK_NAMESPACE::DestroyAuthService(pService);
    }, "Destroy an auth service instance");
}