#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include "zoom_sdk.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "meeting_service_components/meeting_audio_interface.h"
#include "meeting_service_components/meeting_participants_ctrl_interface.h"
#include "meeting_service_components/meeting_video_interface.h"
#include "meeting_service_interface.h"
#include "setting_service_interface.h"
#include "auth_service_interface.h"
#include "events/AuthServiceEvent.h"
#include <iostream>
#include <functional>
#include "Zoom.h"
#include <jwt-cpp/jwt.h>

namespace py = pybind11;


/**
 *  Callback fired atexit()
 */
void onExit() {
    auto* zoom = &Zoom::getInstance();
    zoom->leave();
    zoom->clean();

    cout << "exiting..." << endl;
}

/**
 * Callback fired when a signal is trapped
 * @param signal type of signal
 */
void onSignal(int signal) {
    onExit();
    _Exit(signal);
}


/**
 * Callback for glib event loop
 * @param data event data
 * @return always TRUE
 */
gboolean onTimeout (gpointer data) {
    return TRUE;
}

std::function<void()> onAuth = []() {
        throw std::runtime_error("qwewe");
};

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

SDKError runshuggy(int argc, char** argv) {
    SDKError err{SDKERR_SUCCESS};
    auto* zoom = &Zoom::getInstance();

    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    atexit(onExit);

    // read the CLI and config.ini file
    err = zoom->config(argc, argv);
    if (Zoom::hasError(err, "configure"))
        return err;

    // initialize the Zoom SDK
    err = zoom->init();
    if(Zoom::hasError(err, "initialize"))
        return err;

    // authorize with the Zoom SDK
    err = zoom->auth();
    if (Zoom::hasError(err, "authorize"))
        return err;

    return err;
}

static gboolean quitMainLoop(gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;
    g_main_loop_quit(loop);
    return G_SOURCE_REMOVE;
}

void runloopy() {
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    atexit(onExit);

    GMainLoop* eventLoop;
    eventLoop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(100, onTimeout, eventLoop);
    g_timeout_add_seconds(50, quitMainLoop, eventLoop);
    g_main_loop_run(eventLoop);
}

int mainshuggy(int argc, char **argv) {
    // Run the Meeting Botsdsdss
    SDKError err = runshuggy(argc, argv);

    if (Zoom::hasError(err))
        return err;

    // Use an event loop to receive callbacks
    GMainLoop* eventLoop;
    eventLoop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(100, onTimeout, eventLoop);
    g_main_loop_run(eventLoop);

    return err;
}


void log(const std::string& message) {
    std::cout << message << std::endl;
}

// Function call guard for logging
struct LogGuard {
    std::string funcName;
    LogGuard(const std::string& name) : funcName(name) {
        log("Entering " + funcName);
    }
    ~LogGuard() {
        log("Exiting " + funcName);
    }
};

std::string zcharToString(const zchar_t* str) {
    return str ? std::string(str) : "null";
}

// Helper function to convert SDKUserType to string
std::string SDKUserTypeToString(ZOOM_SDK_NAMESPACE::SDKUserType type) {
    switch (type) {
        case ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER: return "SDK_UT_NORMALUSER";
        case ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN: return "SDK_UT_WITHOUT_LOGIN";
        default: return "UNKNOWN";
    }
}

// Helper function to convert AudioRawdataSamplingRate to string
std::string AudioRawdataSamplingRateToString(ZOOM_SDK_NAMESPACE::AudioRawdataSamplingRate rate) {
    switch (rate) {
        default: return "UNKNOWN";
    }
}

std::string SDKErrorToString(ZOOM_SDK_NAMESPACE::SDKError error) {
    // Implement this function based on the SDK's error codes
    // For example:
    switch (error) {
        case ZOOM_SDK_NAMESPACE::SDKERR_SUCCESS: return "SDKERR_SUCCESS";
        // Add other error codes as needed
        default: return "UNKNOWN_ERROR";
    }
}

// Function to log JoinParam4WithoutLogin
void logJoinParam4WithoutLogin(const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin& param) {
    std::stringstream ss;
    ss << "JoinParam4WithoutLogin:" << std::endl
       << "  meetingNumber: " << param.meetingNumber << std::endl
       << "  vanityID: " << zcharToString(param.vanityID) << std::endl
       << "  userName: " << zcharToString(param.userName) << std::endl
       << "  psw: " << zcharToString(param.psw) << std::endl
       << "  app_privilege_token: " << zcharToString(param.app_privilege_token) << std::endl
       << "  userZAK: " << zcharToString(param.userZAK) << std::endl
       << "  customer_key: " << zcharToString(param.customer_key) << std::endl
       << "  webinarToken: " << zcharToString(param.webinarToken) << std::endl
       << "  isVideoOff: " << (param.isVideoOff ? "true" : "false") << std::endl
       << "  isAudioOff: " << (param.isAudioOff ? "true" : "false") << std::endl
       << "  join_token: " << zcharToString(param.join_token) << std::endl
       << "  isMyVoiceInMix: " << (param.isMyVoiceInMix ? "true" : "false") << std::endl
#if defined(WIN32)
       << "  hDirectShareAppWnd: " << param.hDirectShareAppWnd << std::endl
       << "  isDirectShareDesktop: " << (param.isDirectShareDesktop ? "true" : "false") << std::endl
#endif
       << "  isAudioRawDataStereo: " << (param.isAudioRawDataStereo ? "true" : "false") << std::endl
       << "  eAudioRawdataSamplingRate: " << AudioRawdataSamplingRateToString(param.eAudioRawdataSamplingRate);
    log(ss.str());
}

// Function to log JoinParam4NormalUser
void logJoinParam4NormalUser(const ZOOM_SDK_NAMESPACE::JoinParam4NormalUser& param) {
    std::stringstream ss;
    ss << "JoinParam4NormalUser:" << std::endl
       << "  meetingNumber: " << param.meetingNumber << std::endl
       << "  vanityID: " << zcharToString(param.vanityID) << std::endl
       << "  userName: " << zcharToString(param.userName) << std::endl
       << "  psw: " << zcharToString(param.psw) << std::endl
       << "  app_privilege_token: " << zcharToString(param.app_privilege_token) << std::endl
       << "  customer_key: " << zcharToString(param.customer_key) << std::endl
       << "  webinarToken: " << zcharToString(param.webinarToken) << std::endl
       << "  isVideoOff: " << (param.isVideoOff ? "true" : "false") << std::endl
       << "  isAudioOff: " << (param.isAudioOff ? "true" : "false") << std::endl
       << "  join_token: " << zcharToString(param.join_token) << std::endl
       << "  isMyVoiceInMix: " << (param.isMyVoiceInMix ? "true" : "false") << std::endl
#if defined(WIN32)
       << "  hDirectShareAppWnd: " << param.hDirectShareAppWnd << std::endl
       << "  isDirectShareDesktop: " << (param.isDirectShareDesktop ? "true" : "false") << std::endl
#endif
       << "  isAudioRawDataStereo: " << (param.isAudioRawDataStereo ? "true" : "false") << std::endl
       << "  eAudioRawdataSamplingRate: " << AudioRawdataSamplingRateToString(param.eAudioRawdataSamplingRate);
    log(ss.str());
}

// Function to log JoinParam
void logJoinParam(const ZOOM_SDK_NAMESPACE::JoinParam& param) {
    log("JoinParam:");
    log("  userType: " + SDKUserTypeToString(param.userType));
    if (param.userType == ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER) {
        logJoinParam4NormalUser(param.param.normaluserJoin);
    } else {
        logJoinParam4WithoutLogin(param.param.withoutloginuserJoin);
    }
}


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


    // Binding IMeetingService as an abstract base class
    py::class_<ZOOM_SDK_NAMESPACE::IMeetingService, std::unique_ptr<ZOOM_SDK_NAMESPACE::IMeetingService, py::nodelete>>(m, "IMeetingService")
        .def("GetMeetingStatus", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingStatus)
        .def("Join", [](ZOOM_SDK_NAMESPACE::IMeetingService& self, ZOOM_SDK_NAMESPACE::JoinParam& param) {
            LogGuard guard("IMeetingService::Join");
            logJoinParam(param);
            auto result = self.Join(param);
            log("  Return value: " + SDKErrorToString(result));
            logJoinParam(param);  // Log again after the call in case the SDK modified the param
            return result;
        })
        .def("Start", &ZOOM_SDK_NAMESPACE::IMeetingService::Start)
        .def("Leave", &ZOOM_SDK_NAMESPACE::IMeetingService::Leave)
        .def("GetMeetingStatus", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingStatus);
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


        

    // LG
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
py::class_<IAuthServiceEvent, std::shared_ptr<IAuthServiceEvent>>(m, "IAuthServiceEvent")
    .def("onAuthenticationReturn", &IAuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &IAuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &IAuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &IAuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &IAuthServiceEvent::onZoomAuthIdentityExpired);

py::class_<AuthServiceEvent, IAuthServiceEvent, std::shared_ptr<AuthServiceEvent>>(m, "AuthServiceEvent")
    .def(py::init<std::function<void()>&>())
    .def("onAuthenticationReturn", &AuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &AuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &AuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &AuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &AuthServiceEvent::onZoomAuthIdentityExpired)
    .def("setOnAuth", &AuthServiceEvent::setOnAuth)
    .def("setOnAuthenticationReturn", &AuthServiceEvent::setOnAuthenticationReturn);


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


   py::class_<Zoom, std::unique_ptr<Zoom, py::nodelete>>(m, "Zoom")
        .def("init", &Zoom::init)
        .def("auth", &Zoom::auth)
        .def("config", [](Zoom& self, const std::vector<std::string>& args) {
            std::vector<char*> cargs;
            cargs.reserve(args.size());
            for (const auto& arg : args) {
                cargs.push_back(const_cast<char*>(arg.c_str()));
            }
            return self.config(static_cast<int>(cargs.size()), cargs.data());
        })
        .def("join", &Zoom::join)
        .def("start", &Zoom::start)
        .def("leave", &Zoom::leave)
        .def("clean", &Zoom::clean)
        .def("startRawRecording", &Zoom::startRawRecording)
        .def("stopRawRecording", &Zoom::stopRawRecording)
        .def("isMeetingStart", &Zoom::isMeetingStart)
        .def_static("hasError", &Zoom::hasError);
    m.def("get_zoom_instance", []() -> Zoom& { return Zoom::getInstance(); },
          py::return_value_policy::reference);


    m.def("mainshuggy", [](const std::vector<std::string>& args) -> void { 
                    std::vector<char*> cargs;
            cargs.reserve(args.size());
            for (const auto& arg : args) {
                cargs.push_back(const_cast<char*>(arg.c_str()));
            }
            mainshuggy(static_cast<int>(cargs.size()), cargs.data());
    });
        m.def("runloopy", []() -> void { 
                   runloopy();
    });


    m.def("dothebull", []() -> void { 
        InitParam initParam;

        auto host = "https://zoom.us";

        initParam.strWebDomain = host;
        initParam.strSupportUrl = host;

        initParam.emLanguageID = LANGUAGE_English;

        initParam.enableLogByDefault = true;
        initParam.enableGenerateDump = true;

        auto err = InitSDK(initParam);


        IMeetingService* m_meetingService;
        ISettingService* m_settingService;
        IAuthService* m_authService;

        err = CreateMeetingService(&m_meetingService);
        

        err = CreateSettingService(&m_settingService);
       

        err = CreateAuthService(&m_authService);
        

        ////

        string client_id = "C_yBC775To66EK6MhNin9A";
        string client_secret = "jnLJW1BKXq4AAD7dgtjHPsUwAGYczPQZ";

        function<void()> onAuth = [&]() {
            std::cout << "YOLO" << std::endl;
        };
        


        time_point   m_iat = std::chrono::system_clock::now();
        time_point m_exp = m_iat + std::chrono::hours{24};

         auto   m_jwt = jwt::create()
                    .set_type("JWT")
                    .set_issued_at(m_iat)
                    .set_expires_at(m_exp)
                    .set_payload_claim("appKey", jwt::claim(client_id))
                    .set_payload_claim("tokenExp", jwt::claim(m_exp))
                    .sign(algorithm::hs256{client_secret});


        err = m_authService->SetEvent(new AuthServiceEvent(onAuth));


        AuthContext ctx;
        Log::error("m_jwtzzz " + m_jwt);
        ctx.jwt_token =  m_jwt.c_str();

        m_authService->SDKAuth(ctx);

        GMainLoop* eventLoop;
        eventLoop = g_main_loop_new(NULL, FALSE);
        g_timeout_add(100, onTimeout, eventLoop);
        g_timeout_add_seconds(30, quitMainLoop, eventLoop);
        g_main_loop_run(eventLoop);

        return;
    });
}