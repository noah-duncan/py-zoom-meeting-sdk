#ifndef MEETING_SDK_LINUX_SAMPLE_AUTHSERVICEEVENT_H
#define MEETING_SDK_LINUX_SAMPLE_AUTHSERVICEEVENT_H

#include <iostream>
#include <csignal>
#include <sstream>
#include <functional>
#include "auth_service_interface.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class AuthServiceEvent : public IAuthServiceEvent  {
    function<void()> m_onAuth;
    function<void()> m_onLogout;
    function<void()> m_onZoomIdentityExpired;
    function<void()> m_onZoomAuthIdentityExpired;
    function<void(AuthResult)> m_onAuthenticationReturn;
    function<void(LOGINSTATUS, IAccountInfo*, LoginFailReason)> m_onLoginReturnWithReason;

public:
    AuthServiceEvent(function<void()>& onAuth) : m_onAuth(onAuth) {};
    ~AuthServiceEvent() {};

    /**
     * callback that is triggered when authentication is complete
     * @param result status of the authentication return
     */
    void onAuthenticationReturn(AuthResult result) override;

    /**
     * Callback of login result with fail reason.
     * @param ret Login Status
     * @param pAccountInfo Account Info that is only valid when ret == LOGINRET_SUCCESS
     * @param reason Reason for login failure that is only valid when ret == LOGIN_FAILED
     */
    void onLoginReturnWithReason(LOGINSTATUS ret, IAccountInfo* pAccountInfo, LoginFailReason reason) override {};

    /**
     * Logout result callback.
     */
    void onLogout() override {};

    /**
     * callback used when the Zoom identity has expired
     * when triggered, please re-login or generate a new zoom access token via REST Api.
     */
    void onZoomIdentityExpired() override {};

    /**
     * callback used when Zoom authentication identity will be expired in 10 minutes
     * when triggered please re-auth
     */
    void onZoomAuthIdentityExpired() override {};

    /* Setters for Callbacks */
    void setOnAuth(const function<void()> callback);
    void setOnAuthenticationReturn(const function<void(AuthResult)>& callback);
};


#endif //MEETING_SDK_LINUX_SAMPLE_AUTHSERVICEEVENT_H
