/*!
* \file auth_service_interface.h
* \brief Authentication Service Interface
* 
*/
#ifndef _AUTH_SERVICE_INTERFACE_H_
#define _AUTH_SERVICE_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

/*! \enum AuthResult
    \brief SDK Authentication Result.
    Here are more detailed structural descriptions.
*/
enum AuthResult
{
	AUTHRET_SUCCESS,///<Authentication is successful. 
	AUTHRET_KEYORSECRETEMPTY,///<The key or secret to authenticate is empty.
	AUTHRET_KEYORSECRETWRONG,///<The key or secret to authenticate is wrong.
	AUTHRET_ACCOUNTNOTSUPPORT,///<The user account does not support.
	AUTHRET_ACCOUNTNOTENABLESDK,///<The user account is not enabled for SDK.
	AUTHRET_UNKNOWN,///<Unknown error.
	AUTHRET_SERVICE_BUSY,///<Service is busy.
	AUTHRET_NONE,///<Initial status.
	AUTHRET_OVERTIME,///<Time out.
	AUTHRET_NETWORKISSUE,///<Network issues.
	AUTHRET_CLIENT_INCOMPATIBLE, ///Account does not support this SDK version
	AUTHRET_JWTTOKENWRONG, ///<The jwt token to authenticate is wrong.
	AUTHRET_LIMIT_EXCEEDED_EXCEPTION, ///<The authentication rate limit is exceeded.
};

/*! \enum LOGINSTATUS
    \brief Login status.
    Here are more detailed structural descriptions.
*/
enum LOGINSTATUS
{
	LOGIN_IDLE,///<Unlogged in.
	LOGIN_PROCESSING,///<In process of login.
	LOGIN_SUCCESS,///<Login successful.
	LOGIN_FAILED,///<Login failed.
};

enum LoginFailReason
{
	LoginFail_None = 0,
	LoginFail_EmailLoginDisable,
	LoginFail_UserNotExist,		
	LoginFail_WrongPassword,	
	LoginFail_AccountLocked,	
	LoginFail_SDKNeedUpdate,	
	LoginFail_TooManyFailedAttempts,	
	LoginFail_SMSCodeError,		
	LoginFail_SMSCodeExpired,	
	LoginFail_PhoneNumberFormatInValid,
	LoginFail_LoginTokenInvalid,
	LoginFail_UserDisagreeLoginDisclaimer,
	LoginFail_Mfa_Required,
	LoginFail_Need_Bitrthday_ask,
	LoginFail_OtherIssue = 100, 
};
#if defined(WIN32)
/**
 * @brief Enumerations of the type for notification service status.
 */
typedef enum
{
	SDK_Notification_Service_None = 0,
	SDK_Notification_Service_Starting,
	SDK_Notification_Service_Started,	
	SDK_Notification_Service_StartFailed,
	SDK_Notification_Service_Closed,
}SDKNotificationServiceStatus;

typedef enum
{
	SDK_Notification_Service_Error_Success = 0,///<Success.
	SDK_Notification_Service_Error_Unknown,///<Unknown error.
	SDK_Notification_Service_Error_Internal_Error,///<Internal error,need retry.
	SDK_Notification_Service_Error_Invalid_Token,///<Invalid token.
	SDK_Notification_Service_Error_Multi_Connect,///<Use same user/resource login again on the same device, the previous login application will receive this error.
	SDK_Notification_Service_Error_Network_Issue,///<Network issue.
	SDK_Notification_Service_Error_Max_Duration, ///<Server disconnects the connection if client stayed connected with server for more than 24 hours. Client need to reconnect/login again.	
}SDKNotificationServiceError;

#endif
/*! \struct tagAuthContext
    \brief SDK Authentication parameter with jwt token.
    Here are more detailed structural descriptions.
*/
typedef struct tagAuthContext
{
	const zchar_t* jwt_token; /*!JWT token. You may generate your JWT token using the online tool https://jwt.io/. **It is highly recommended to generate your JWT token in your backend server.**
								 JWT is generated with three core parts: Header, Payload, and Signature. When combined, these parts are separated by a period to form a token: `aaaaa.bbbbb.cccc`.
								 Please follow this template to compose your payload for SDK initialization:
							     ** Header
							  	 {
							  		"alg": "HS256",
							  		"typ": "JWT"
							  	 }
							     ** Payload
							   	 {
							        "appKey": "string", // Your SDK key
							        "iat": long, // access token issue timestamp
									"exp": long, // access token expire time
									"tokenExp": long // token expire time
							     }
							     ** Signature
							     HMACSHA256(
							  			base64UrlEncode(header) + "." +
										base64UrlEncode(payload),
										"Your SDK secret here"
								)
							  */
	tagAuthContext()
	{
		jwt_token = NULL;
	}

}AuthContext;
/*! \enum LoginType
	\brief User login type.
	Here are more detailed structural descriptions.
*/
enum LoginType
{
	LoginType_Unknown,///<Unknown type.
	LoginType_SSO,///<Login with SSO token.
};

/// \brief Account information interface.
///
class IAccountInfo
{
public:
	/// \brief Get the screen name of user.
	/// \return The return value is the displayed username. If there is no screen name of user, the return value is a string of length ZERO(0).
	virtual const zchar_t* GetDisplayName() = 0;
	/// \brief Get login type.
/// \return The return value is the account login type. For more details, see \link LoginType \endlink enum.
	virtual LoginType GetLoginType() = 0;
	virtual ~IAccountInfo(){};
};

/// \brief Authentication service callback event.
///
class IAuthServiceEvent
{
public:
	virtual ~IAuthServiceEvent() {}

	/// \brief Authentication result callback.
	/// \param ret Authentication result value.  For more details, see \link AuthResult \endlink enum.
	virtual void onAuthenticationReturn(AuthResult ret) = 0;

	/// \brief Callback of login result with fail reason.
	/// \param ret Login status. see \link LOGINSTATUS \endlink enum.
	/// \param pAccountInfo Valid when the ret is LOGINRET_SUCCESS. Otherwise NULL.
	/// \param reason Login fail reason. Valid when the ret is LOGIN_FAILED. Otherwise LoginFail_None. see \link LoginFailReason \endlink enum.
	virtual void onLoginReturnWithReason(LOGINSTATUS ret, IAccountInfo* pAccountInfo, LoginFailReason reason) = 0;
	
	/// \brief Logout result callback.
	virtual void onLogout() = 0;

	/// \brief Zoom identity has expired, please re-login or generate a new zoom access token via REST Api.
	virtual void onZoomIdentityExpired() = 0;

	/// \brief Zoom authentication identity will be expired in 10 minutes, please re-auth.
	virtual void onZoomAuthIdentityExpired() = 0;
#if defined(WIN32)
	/// \brief Notification service status changed callback.
	/// \param status The value of transfer meeting service. For more details, see \link SDKNotificationServiceStatus \endlink.
	/// \param error Connection Notification service fail error code.For more details, see \link SDKNotificationServiceError \endlink enum.
	virtual void onNotificationServiceStatus(SDKNotificationServiceStatus status, SDKNotificationServiceError error) = 0;
#endif
};
#if defined(WIN32)
class IDirectShareServiceHelper;
class IOutlookPluginIntegrationHelper;
class INotificationServiceHelper;
#endif
/// \brief Authentication Service Interface.
///
class IAuthService
{
public:
	/// \brief Set the authentication service callback event handler.
	/// \param pEvent A pointer to receive authentication event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IAuthServiceEvent* pEvent) = 0;

	/// \brief SDK Authentication with jwt token.
	/// \param authContext The parameter to be used for authentication SDK, see \link AuthContext \endlink structure. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SDKAuth(AuthContext& authContext) = 0;

	/// \brief Get authentication status.
	/// \return The return value is authentication status. To get extended error information, see \link AuthResult \endlink enum.
	virtual AuthResult GetAuthResult() = 0;

	/// \brief Get SDK identity.
	/// \return The SDK identity.
	virtual const zchar_t* GetSDKIdentity() = 0;

	/// \brief Get SSO login web url.
	/// \param prefix_of_vanity_url, prefix of vanity url. 
	/// \return SSO login web url
	virtual const zchar_t* GenerateSSOLoginWebURL(const zchar_t* prefix_of_vanity_url) = 0;

	/// \brief Account login.
	/// \param uri_protocol For the parameter to be used for sso account login
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	///You need to call this APIs after IAuthServiceEvent::onAuthenticationReturn() return SDKErr_Success.
	virtual SDKError SSOLoginWithWebUriProtocol(const zchar_t* uri_protocol) = 0;
	
	/// \brief Account logout.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError LogOut() = 0;

	/// \brief Get login account information.
	/// \return If you has logged in your account successfully, the return value is a pointer to IAccountInfo, otherwise is NULL.
	virtual IAccountInfo* GetAccountInfo() = 0;

	/// \brief Get login status.
	/// \return The return value is login status. To get extended error information, see \link LOGINSTATUS \endlink enum.
	virtual LOGINSTATUS GetLoginStatus() = 0;
#if defined(WIN32)
	/// \brief Get direct share service helper interface. 
	/// \return If you logged in your account successfully, the return value is the object pointer IDirectShareServiceHelper. Otherwise is NULL.
	virtual IDirectShareServiceHelper* GetDirectShareServiceHelper() = 0;

	/// \brief Enable or disable auto register notification service. This is enabled by default.
	/// \param [in] bEnable True means enabled, otherwise not. 
	virtual void EnableAutoRegisterNotificationServiceForLogin(bool bEnable) = 0;

	/// \brief Register notification service.
	/// \param accessToken Initialize parameter of notification service.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RegisterNotificationService(const zchar_t* accessToken) = 0;

	/// \brief UnRegister notification service.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError UnregisterNotificationService() = 0;

	/// \brief Get notification service helper interface. 
	/// \return If the function succeeds, the return value is a pointer to INotificationServiceHelper . Otherwise returns NULL.
	virtual INotificationServiceHelper* GetNotificationServiceHelper() = 0;
#endif
};
END_ZOOM_SDK_NAMESPACE
#endif
