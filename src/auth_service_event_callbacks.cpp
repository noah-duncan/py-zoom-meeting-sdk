#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"

#include "auth_service_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

class AuthServiceEventCallbacks : public ZOOM_SDK_NAMESPACE::IAuthServiceEvent {
private:
    function<void(ZOOM_SDK_NAMESPACE::AuthResult)> m_onAuthenticationReturnCallback;
    function<void(ZOOM_SDK_NAMESPACE::LOGINSTATUS ret, ZOOM_SDK_NAMESPACE::IAccountInfo* pAccountInfo, ZOOM_SDK_NAMESPACE::LoginFailReason reason)> m_onLoginReturnWithReasonCallback;
    function<void()> m_onLogoutCallback;
    function<void()> m_onZoomIdentityExpiredCallback;
    function<void()> m_onZoomAuthIdentityExpiredCallback;
public:
    AuthServiceEventCallbacks(
        const function<void(ZOOM_SDK_NAMESPACE::AuthResult)> & onAuthenticationReturnCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::LOGINSTATUS ret, ZOOM_SDK_NAMESPACE::IAccountInfo* pAccountInfo, ZOOM_SDK_NAMESPACE::LoginFailReason reason)> & onLoginReturnWithReasonCallback = nullptr,
        const function<void()> & onLogoutCallback = nullptr,
        const function<void()> & onZoomIdentityExpiredCallback = nullptr,
        const function<void()> & onZoomAuthIdentityExpiredCallback = nullptr
    ) : m_onAuthenticationReturnCallback(onAuthenticationReturnCallback),
        m_onLoginReturnWithReasonCallback(onLoginReturnWithReasonCallback),
        m_onLogoutCallback(onLogoutCallback),
        m_onZoomIdentityExpiredCallback(onZoomIdentityExpiredCallback),
        m_onZoomAuthIdentityExpiredCallback(onZoomAuthIdentityExpiredCallback) {}

    void onAuthenticationReturn(ZOOM_SDK_NAMESPACE::AuthResult ret) override {
        if (m_onAuthenticationReturnCallback)
            m_onAuthenticationReturnCallback(ret);
    }

    void onLoginReturnWithReason(ZOOM_SDK_NAMESPACE::LOGINSTATUS ret, ZOOM_SDK_NAMESPACE::IAccountInfo* pAccountInfo, ZOOM_SDK_NAMESPACE::LoginFailReason reason) override {
        if (m_onLoginReturnWithReasonCallback)
            m_onLoginReturnWithReasonCallback(ret, pAccountInfo, reason);
    }

    void onLogout() override {
        if (m_onLogoutCallback)
            m_onLogoutCallback();
    }

    void onZoomIdentityExpired() override {
        if (m_onZoomIdentityExpiredCallback)
            m_onZoomIdentityExpiredCallback();
    }

    void onZoomAuthIdentityExpired() override {
        if (m_onZoomAuthIdentityExpiredCallback)
            m_onZoomAuthIdentityExpiredCallback();
    }
};

void init_auth_service_event_callbacks(nb::module_ &m) {
    nb::class_<AuthServiceEventCallbacks, ZOOM_SDK_NAMESPACE::IAuthServiceEvent>(m, "AuthServiceEventCallbacks")
    .def(
        nb::init<
            function<void(ZOOM_SDK_NAMESPACE::AuthResult)>&,
            function<void(ZOOM_SDK_NAMESPACE::LOGINSTATUS,ZOOM_SDK_NAMESPACE::IAccountInfo*,ZOOM_SDK_NAMESPACE::LoginFailReason)>&,
            function<void()>&,
            function<void()>&,
            function<void()>&
        >(),
        nb::arg("onAuthenticationReturnCallback") = nullptr,
        nb::arg("onLoginReturnWithReasonCallback") = nullptr,
        nb::arg("onLogoutCallback") = nullptr,
        nb::arg("onZoomIdentityExpiredCallback") = nullptr,
        nb::arg("onZoomAuthIdentityExpiredCallback") = nullptr
    );

    /*
    .def("onAuthenticationReturn", &AuthServiceEventCallbacks::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &AuthServiceEventCallbacks::onLoginReturnWithReason)
    .def("onLogout", &AuthServiceEventCallbacks::onLogout)
    .def("onZoomIdentityExpired", &AuthServiceEventCallbacks::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &AuthServiceEventCallbacks::onZoomAuthIdentityExpired);
    */
}