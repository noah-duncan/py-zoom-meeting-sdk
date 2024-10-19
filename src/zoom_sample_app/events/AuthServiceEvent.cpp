#include "AuthServiceEvent.h"

void AuthServiceEvent::onAuthenticationReturn(AuthResult result) {
    if (m_onAuthenticationReturn) {
        m_onAuthenticationReturn(result);
        return;
    }

    stringstream message;
    message << "authentication failed because the ";

    switch (result) {
        case AUTHRET_KEYORSECRETEMPTY:
            message << "key or secret is empty";
            break;
        case AUTHRET_JWTTOKENWRONG:
            message << "JWT is invalid";
            break;
        case AUTHRET_OVERTIME:
            message << "operation timed out";
            break;
        case AUTHRET_SUCCESS:
            if (m_onAuth) m_onAuth();
            else message << "authentication callback was not set";
            break;
        default:
            message << "Zoom SDK encountered an unknown error: " << result;
            break;
    }

    if (result != AUTHRET_SUCCESS) {
        Log::error(message.str());
        abort();
    }

    return;
}

void AuthServiceEvent::setOnAuthenticationReturn(const function<void(AuthResult)>& callback) {
    m_onAuthenticationReturn = callback;
}
void AuthServiceEvent::setOnAuth(const function<void()> callback) {
    m_onAuth = callback;
}