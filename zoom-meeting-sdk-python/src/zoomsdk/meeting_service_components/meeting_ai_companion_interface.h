/*!
* \file meeting_ai_companion_interface.h
* \brief Meeting Service AI Companion Interface.
*/
#ifndef _MEETING_AI_COMPANION_INTERFACE_H_
#define _MEETING_AI_COMPANION_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

enum AICompanionFeature
{
	SMART_SUMMARY,    ///<Meeting summary with AI Companion generates summary assets.
	QUERY,            ///<Meeting questions with AI Companion generates transcript assets.
	SMART_RECORDING,  ///<Smart recording with AI Companion generates recording assets.
};

/*! \enum MeetingAICompanionQueryFeedbackType
 * \brief Enumerations of the type for query feedback.
 */
enum MeetingAICompanionQueryFeedbackType
{
	MeetingAICompanionQueryFeedbackType_None = 0,	///<Initialization.
	MeetingAICompanionQueryFeedbackType_Good,		///<Good.
	MeetingAICompanionQueryFeedbackType_Bad			///<Bad.
};

/*! \enum MeetingAICompanionQueryRequestError
 * \brief Enumerations of the type for query request error.
 */
enum MeetingAICompanionQueryRequestError
{
	MeetingAICompanionQueryRequestError_OK = 0,					///<OK.
	MeetingAICompanionQueryRequestError_InvalidParam,			///<InvalidParam.
	MeetingAICompanionQueryRequestError_SendFailed,				///<SendFailed.
	MeetingAICompanionQueryRequestError_WebUnAvailable,			///<WebUnAvailable.
	MeetingAICompanionQueryRequestError_PermissionVerifyFailed,	///<PermissionVerifyFailed.
	MeetingAICompanionQueryRequestError_QueryRateLimitError,	///<QueryRateLimitError.
	MeetingAICompanionQueryRequestError_Timeout,				///<Timeout.
	MeetingAICompanionQueryRequestError_Unknown = 100			///<Unknown.
};

/*! \enum MeetingAICompanionQuerySettingOptions
 * \brief Enumerations of the type for query setting options.
 */
enum MeetingAICompanionQuerySettingOptions
{
	MeetingAICompanionQuerySettingOptions_None = 0,				///<Initialization.
	MeetingAICompanionQuerySettingOptions_WhenQueryStarted,		///<All participants can ask question, and answers are based on the meeting's start until now.
	MeetingAICompanionQuerySettingOptions_WhenParticipantsJoin,	///<All participants can ask question, and answers are based on the current user's joining time until now.
	MeetingAICompanionQuerySettingOptions_OnlyHost				///<Only hosts and users with host privileges assigned before the meeting starts can ask question.
};

/*! \class IMeetingEnableSmartSummaryHandler
	\brief Interface to handle enable smart summary.
*/
class IMeetingEnableSmartSummaryHandler
{
public:
	virtual ~IMeetingEnableSmartSummaryHandler() {};

	/// \brief Enable smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableSmartSummary() = 0;

	/// \brief Determine if this handler is requesting enable smart summary.
	/// \return True means this handler is for requesting enable smart summary. Otherwise not.
	virtual bool IsForRequest() = 0;
};

/*! \class IMeetingStartSmartSummaryHandler
	\brief Interface to handle starting the smart summary.
*/
class IMeetingStartSmartSummaryHandler
{
public:
	virtual ~IMeetingStartSmartSummaryHandler() {};

	/// \brief Start smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartSmartSummary() = 0;

	/// \brief Determine if this handler is for requesting to start the smart summary.
	/// \return True means this handler is requesting to start the smart summary. Otherwise not.
	virtual bool IsForRequest() = 0;
};

/*! \class IMeetingStopSmartSummaryHandler
	\brief Interface to handle stopping the smart summary.
*/
class IMeetingStopSmartSummaryHandler
{
public:
	virtual ~IMeetingStopSmartSummaryHandler() {};

	/// \brief Stop smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopSmartSummary() = 0;
};

/*! \class IMeetingEnableSmartSummaryActionHandler
	\brief The handler to confirm enabling smart summary
*/
class IMeetingEnableSmartSummaryActionHandler
{
public:
	virtual ~IMeetingEnableSmartSummaryActionHandler() {};

	/// \brief Get the title of tip.
	/// \return The title of tip.
	virtual const zchar_t* GetTipTitle() = 0;

	/// \brief Get the string of tip.
	/// \return The string of tip.
	virtual const zchar_t* GetTipString() = 0;

	/// \brief Confirm enabling smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Confirm() = 0;

	/// \brief Cancel enabling smart summary.
/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Cancel() = 0;
};

/*! \class IMeetingApproveStartSmartSummaryHandler
	\brief The handler to approve the smart summary started request
*/
class IMeetingApproveStartSmartSummaryHandler
{
public:
	virtual ~IMeetingApproveStartSmartSummaryHandler() {};

	/// \brief Get the user ID of the requester.
	/// \return The user ID of the requester.
	virtual unsigned int GetSenderUserID() = 0;

	/// \brief Approve request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Approve() = 0;

	/// \brief Decline request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline() = 0;
};

/*! \class IMeetingApproveEnableSmartSummaryHandler
	\brief The handler to approve the smart summary enablement request.
*/
class IMeetingApproveEnableSmartSummaryHandler
{
public:
	virtual ~IMeetingApproveEnableSmartSummaryHandler() {};

	/// \brief Get the user ID of the requester.
	/// \return The user ID of the requester.
	virtual unsigned int GetSenderUserID() = 0;

	/// \brief Continue approve action.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ContinueApprove() = 0;
};

/// \brief Meeting smart summary callback event.
class IMeetingAICompanionSmartSummaryHelperEvent
{
public:
	/// \brief Notify the meting does not support smart summary.
	virtual void onSmartSummaryStateNotSupported() = 0;

	/// \brief Notify the meeting support smart summary but smart summary feature is disabled.
	/// \param handler The handler to enable smart summary.
	virtual void onSmartSummaryStateSupportedButDisabled(IMeetingEnableSmartSummaryHandler* handler) = 0;

	/// \brief Notify the meeting smart summary is not started.
	/// \param handler The handler to start smart summary.
	virtual void onSmartSummaryStateEnabledButNotStarted(IMeetingStartSmartSummaryHandler* handler) = 0;

	/// \brief Notify the meeting smart summary is started.
	/// \param handler The handler to stop smart summary. If the user can not stop smart summary, the handler will be null.
	virtual void onSmartSummaryStateStarted(IMeetingStopSmartSummaryHandler* handler) = 0;

	/// \brief Notify failed to start the smart summary.
	/// \param bTimeout True means timeout. Otherwise no timeout. May be declined by host or cohost.
	virtual void onFailedToStartSmartSummary(bool bTimeout) = 0;

	/// \brief Notify receive request to enable smart summary.
	/// \param handler The handler to handle enable smart summary request.
	virtual void onSmartSummaryEnableRequestReceived(IMeetingApproveEnableSmartSummaryHandler* handler) = 0;

	/// \brief Notify receive request to start smart summary.
	/// \param handler The handler to handle request.
	virtual void onSmartSummaryStartRequestReceived(IMeetingApproveStartSmartSummaryHandler* handler) = 0;

	/// \brief Notify receive smart summary enable action callback.
	/// \param handler The handler to enable smart summary.
	virtual void onSmartSummaryEnableActionCallback(IMeetingEnableSmartSummaryActionHandler* handler) = 0;

	virtual ~IMeetingAICompanionSmartSummaryHelperEvent() {}

};

/// \brief Smart summary helper in meeting.
class IMeetingAICompanionSmartSummaryHelper
{
public:
	virtual ~IMeetingAICompanionSmartSummaryHelper() {}

	/// \brief Set the smart summary callback event handler.
	/// \param event A pointer to the IMeetingAICompanionSmartSummaryHelperEvent that receives the smart summary event. 
	virtual void SetEvent(IMeetingAICompanionSmartSummaryHelperEvent* event) = 0;
};


/*! \class ISmartSummaryPrivilegeHandler
	\brief Interface to handle start smart summary request
	\deprecated This class is marked as deprecated
*/
class ISmartSummaryPrivilegeHandler
{
public:
	virtual ~ISmartSummaryPrivilegeHandler() {};

	/// \brief Agree the start smart summary request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Accept() = 0;

	/// \brief Decline the start smart summary request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline() = 0;

	/// \brief Ignore the start smart summary request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Ignore() = 0;
};
/// \brief Meeting smart summary callback event.
/// \deprecated This class is marked as deprecated
class IMeetingSmartSummaryHelperEvent
{
public:
	/// \brief Callback event when smart summary status changes.
	/// \param isStarted true means smart summary is started, false means not.
	virtual void onSmartSummaryStatusChange(bool isStarted) = 0;

	/// \brief Callback event when a user requests host to start smart summary.
	/// \param senderId The user who requests host to start smart summary.
	/// \param handler The handler to handle the smart summary start request.
	virtual void onSmartSummaryPrivilegeRequested(unsigned int senderId, ISmartSummaryPrivilegeHandler* handler) = 0;

	/// \brief Callback event when the host handle the smart summary request.
	/// \param timeout True means the host not handle the request until timeout.
	/// \param decline True means the host decline the request, false means the host agree the request.
	virtual void onSmartSummaryStartReqResponse(bool timeout, bool decline) = 0;

	/// \brief Callback event when the user need to go to web to enable the smart summary feature.
	/// \remarks Valid for both ZOOM style and user custom interface mode. The callback will only be triggered for Business account. And the smart summary feature will be enabled for the future meeting, but not enable the current meeting if the user enables the setting in web.
	virtual void onEnableSmartSummary() = 0;

	virtual ~IMeetingSmartSummaryHelperEvent() {}

};

/// \brief Meeting smart summary helper interface.
/// \deprecated This class is marked as deprecated
class IMeetingSmartSummaryHelper
{
public:
	virtual ~IMeetingSmartSummaryHelper() {}

	/// \brief Set the smart summary callback event handler.
	/// \param event A pointer to the IMeetingSmartSummaryHelperEvent that receives the smart summary event. 
	virtual void SetEvent(IMeetingSmartSummaryHelperEvent* event) = 0;

	/// \brief Determine if current meeting support smart summary feature.
	/// \return True means the current meeting support smart summary feature, False means not supported.
	virtual bool IsSmartSummarySupported() = 0;

	/// \brief Determine if smart summary feature is enabled in the meeting.
	/// \return True means smart summary feature is enabled.
	virtual bool IsSmartSummaryEnabled() = 0;

	/// \brief Whether the current user can enable the smart summary feature for the account.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanEnableSmartSummaryFeature() = 0;

	/// \brief Enable the smart summary feature for the account.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableSmartSummaryFeature() = 0;

	/// \brief Whether the current user is able to start smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanStartSmartSummary() = 0;

	/// \brief Start smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartSmartSummary() = 0;

	/// \brief Stop smart summary.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopSmartSummary() = 0;

	/// \brief Query whether smart summary is started.
	/// \return True means smart summary is started, false means not.
	virtual bool IsSmartSummaryStarted() = 0;

	/// \brief Whether the current user can request the admin to enable the smart summary feature for the account.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanRequestEnableSmartSummaryFeature() = 0;

	/// \brief Request the admin to enable the smart summary feature for the account.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestEnableSmartSummaryFeature() = 0;

	/// \brief Whether the current user can request host to start the smart summary for the current meeting.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanRequestStartSmartSummary() = 0;

	/// \brief Request the host to start the smart summary for the current meeting.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestStartSmartSummary() = 0;
};


/*! \class IMeetingAICompanionQueryItem
	\brief Interface to get AI companion query item information
*/
class IMeetingAICompanionQueryItem
{
public:
	virtual ~IMeetingAICompanionQueryItem() {}

	/// \brief Get the query question ID.
	virtual const zchar_t* GetQueryID() = 0;

	/// \brief Get the query question content.
	virtual const zchar_t* GetQustionContent() = 0;

	/// \brief Get the query answer content.
	virtual const zchar_t* GetAnswerContent() = 0;

	/// \brief Get the error code.
	virtual MeetingAICompanionQueryRequestError GetErrorCode() = 0;

	/// \brief Get the error message.
	virtual const zchar_t* GetErrorMsg() = 0;

	/// \brief Get the timestamp.
	virtual time_t GetTimeStamp() = 0;

	/// \brief Send feedback of query answer.
	/// \param eFeedbackType The feedback type. For more details, see \link MeetingAICompanionQueryFeedbackType \endlink.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Feedback(MeetingAICompanionQueryFeedbackType eFeedbackType) = 0;
};

/*! \class IMeetingEnableQueryHandler
	\brief Interface to enable AI companion query.
*/
class IMeetingEnableQueryHandler
{
public:
	virtual ~IMeetingEnableQueryHandler() {}

	/// \brief Enable meeting query.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableQuery() = 0;

	/// \brief Determine if this handler for request enable query.
	/// \return True means this handler is for request enable query. Otherwise it returns false means this handler is for enable query directly..
	virtual bool IsForRequest() = 0;
};

/*! \class IMeetingStartQueryHandler
	\brief Interface to start AI companion query.
*/
class IMeetingStartQueryHandler
{
public:
	virtual ~IMeetingStartQueryHandler() {}

	/// \brief Start meeting query.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartMeetingQuery() = 0;

	/// \brief Determine if this handler for request start query.
	/// \return True means this handler is for request start query. Otherwise it returns false means this handler is for start query directly.
	virtual bool IsForRequest() = 0;
};

/*! \class IMeetingApproveEnableQueryHandler
	\brief Interface to appprove enable AI companion query.
*/
class IMeetingApproveEnableQueryHandler
{
public:
	virtual ~IMeetingApproveEnableQueryHandler() {}

	/// \brief Get the user id of requester.
	virtual unsigned int GetSenderUserID() = 0;

	/// \brief Continue approve action.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ContinueApprove() = 0;
};

/*! \class IMeetingApproveStartQueryHandler
	\brief Interface to appprove start AI companion query.
*/
class IMeetingApproveStartQueryHandler
{
public:
	virtual ~IMeetingApproveStartQueryHandler() {}

	/// \brief Get the requester's user ID.
	virtual unsigned int GetSenderUserID() = 0;

	/// \brief Approve the request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Approve() = 0;

	/// \brief Decline the request.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline() = 0;
};

/*! \class IMeetingSendQueryHandler
	\brief Interface to send query.
*/
class IMeetingSendQueryHandler
{
public:
	virtual ~IMeetingSendQueryHandler() {}

	/// \brief Get default query questions.
	/// \return If the function succeeds, it returns the array of questions. Otherwise the function fails and returns nothing.
	virtual IList<const zchar_t*>* GetDefaultQueryQuestions() = 0;

	/// \brief Send query question.
	/// \param question The query question.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SendQueryQuestion(const zchar_t* sQuestion) = 0;

	/// \brief Stop meeting query.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopMeetingQuery() = 0;
};

/*! \class IMeetingEnableQueryActionHandler
	\brief Interface to enable query action.
*/
class IMeetingEnableQueryActionHandler
{
public:
	virtual ~IMeetingEnableQueryActionHandler() {}

	/// \brief Get the title of the tip.
	virtual const zchar_t* GetTipTitle() = 0;

	/// \brief Get the tip string.
	virtual const zchar_t* GetTipString() = 0;

	/// \brief Confirm enable query. The object will be deleted after this interface call.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Confirm() = 0;

	/// \brief Cancel enable query. The object will be deleted after this interface call.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Cancel() = 0;
};

/// \brief Meeting AI companion query callback event.
///
class IMeetingAICompanionQueryHelperEvent
{
public:
	virtual ~IMeetingAICompanionQueryHelperEvent() {}

	/// \brief Callback event that the meeting does not support query.
	virtual void onQueryStateNotSupported() = 0;

	/// \brief Callback event that the meeting supports query but query feature is disabled.
	/// \param pHandler The handler to enable the query. For more details, see \link IMeetingEnableQueryHandler \endlink.
	virtual void onQueryStateSupportedButDisabled(IMeetingEnableQueryHandler* pHandler) = 0;

	/// \brief Callback event that the query is not started.
	/// \param pHandler The handler to start the query. For more details, see \link IMeetingStartQueryHandler \endlink.
	virtual void onQueryStateEnabledButNotStarted(IMeetingStartQueryHandler* pHandler) = 0;

	/// \brief Callback event that the query is started.
	/// \param pHandler The handler to send query. For more details, see \link IMeetingSendQueryHandler \endlink.
	virtual void onQueryStateStarted(IMeetingSendQueryHandler* pHandler) = 0;

	/// \brief Callback event that the query setting is changed.
	/// \param eSetting The selected query setting. For more details, see \link MeetingAICompanionQuerySettingOptions \endlink.
	virtual void onQuerySettingChanged(MeetingAICompanionQuerySettingOptions eSetting) = 0;

	/// \brief Callback event that the query failed to start.
	/// \param bTimeout true means is timeout. Otherwise not.
	virtual void onFailedToStartQuery(bool bTimeout) = 0;

	/// \brief Callback event that receiving request to enable query.
	/// \param pHandler The handler to handle the request. For more details, see \link IMeetingApproveEnableQueryHandler \endlink.
	virtual void onReceiveRequestToEnableQuery(IMeetingApproveEnableQueryHandler* pHandler) = 0;

	/// \brief Callback event that receiving request to start query.
	/// \param pHandler The handler to handle the request. For more details, see \link IMeetingApproveStartQueryHandler \endlink.
	virtual void onReceiveRequestToStartQuery(IMeetingApproveStartQueryHandler* pHandler) = 0;

	/// \brief Callback event that receiving query answer.
	/// \param pHandler The obejct of IMeetingAICompanionQueryItem. For more details, see \link IMeetingAICompanionQueryItem \endlink.
	virtual void onReceiveQueryAnswer(IMeetingAICompanionQueryItem* pQueryItem) = 0;

	/// \brief Callback event that receiving query enable action callback.
	/// \param pHandler The handler to enable the query. For more details, see \link IMeetingEnableQueryActionHandler \endlink.
	virtual void onQueryEnableActionCallback(IMeetingEnableQueryActionHandler* pHandler) = 0;
};


/// \brief Meeting AI companion query helper interface.
///
class IMeetingAICompanionQueryHelper
{
public:
	virtual ~IMeetingAICompanionQueryHelper() {}

	/// \brief Set the AI companion query callback event handler.
	/// \param event A pointer to the IMeetingAICompanionQueryHelperEvent that receives the AI companion query event. 
	virtual void SetEvent(IMeetingAICompanionQueryHelperEvent* event) = 0;

	/// \brief Determine whether the current user can change query setting.
	/// \param bCan True means that it can change the setting. Otherwise it can't change the setting.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanChangeQuerySetting(bool& bCan) = 0;

	/// \brief Change query setting.
	/// \param setting The query setting. For more details, see \link MeetingAICompanionQuerySettingOptions \endlink.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ChangeQuerySettings(MeetingAICompanionQuerySettingOptions setting) = 0;

	/// \brief Get the selected query setting.
	/// \return If the function succeeds, it will return the selected query setting. Otherwise not.
	virtual MeetingAICompanionQuerySettingOptions GetSelectedQuerySetting() = 0;

	/// \brief Determine whether the legal notice for the AI Companion query is available.
	/// \param bAvailable True indicates the legal notice for the AI Companion query is available. Otherwise the legal notice is not available.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError IsAICompanionQueryLegalNoticeAvailable(bool& bAvailable) = 0;

	/// \brief Get the AI Companion query legal notices prompt.
	/// \return If the function succeeds, it will return the AI Companion query legal notices prompt. Otherwise the function fails and returns NULL.
	virtual const zchar_t* GetAICompanionQueryLegalNoticesPrompt() = 0;

	/// \brief Get the AI Companion query legal notices explained.
	/// \return If the function succeeds, it will return the AI Companion query legal notices explained. Otherwise the function fails and returns NULL.
	virtual const zchar_t* GetAICompanionQueryLegalNoticesExplained() = 0;

	/// \brief Determine whether the legal notice for the AI Companion query privacy is available.
	/// \param bAvailable True indicates the legal notice for the AI Companion query privacy is available. Otherwise the legal notice is not available.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError IsAICompanionQueryPrivacyLegalNoticeAvailable(bool& bAvailable) = 0;

	/// \brief Get the AI Companion query privacy legal notices prompt.
	/// \return If the function succeeds, it will return the AI Companion query privacy legal notices prompt. Otherwise the function fails and returns NULL.
	virtual const zchar_t* GetAICompanionQueryPrivacyLegalNoticesPrompt() = 0;
};


class IAICompanionFeatureTurnOnAgainHandler
{
public:
	virtual ~IAICompanionFeatureTurnOnAgainHandler() {};
	/// \brief Get the list of features that the participant turns off.
	virtual IList<AICompanionFeature>* GetFeatureList() = 0;
	/// \brief Get the list of features that the assets are deleted when the feature is turned off by participant.
	virtual IList<AICompanionFeature>* GetAssetsDeletedFeatureList() = 0;
	/// \brief Turn on the auto AI Companion feature which was stopped by the participant before the host or cohost joined meeting.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnOnAgain() = 0;
	/// \brief Agree the auto AI Companion feature turn off status. Keep the AI Companion feature off.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AgreeTurnOff() = 0;
};

/// \brief The handler to handle a user request to turn the AI Companion features on or off.
class IAICompanionFeatureSwitchHandler
{
public:
	virtual ~IAICompanionFeatureSwitchHandler() {};
	/// \brief Get the user ID who requests host to turn the AI Companion features on or off.
	/// \return the request user ID.
	virtual unsigned int GetRequestUserID() = 0;
	/// \brief Turn the AI Companion features on or off.
	/// \return true means turn on the AI Companion features, false means turn off the AI Companion features.
	virtual bool IsTurnOn() = 0;
	/// \brief Agree the request to turn the AI companion features on or off.
	/// \param deleteAssets Specify whether delete the meeting assets when turning off the AI Companion features. 
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Agree(bool deleteAsserts) = 0;
	/// \brief Decline the request to turn the AI companion features on or off.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline() = 0;
};


class IMeetingAICompanionCtrlEvent
{
public:
	virtual ~IMeetingAICompanionCtrlEvent() {}
	/// \brief The callback when the auto start AI Companion feature is turned off by a participant before the host join. Only the host or cohost can receive this callback.
	/// \param handler A pointer to the IAutoAICompanionFeatureTurnOnAgainHandler. For more details, see \link IAutoAICompanionFeatureTurnOnAgainHandler \endlink.
	virtual void onAICompanionFeatureTurnOffByParticipant(IAICompanionFeatureTurnOnAgainHandler* handler) = 0;
	/// \brief The callback when host receives the request to turn the AI Companion features on or off.
	/// \param handler A pointer to the IAICompanionFeatureSwitchHandler. For more details, see \link IAICompanionFeatureSwitchHandler \endlink.
	virtual void onAICompanionFeatureSwitchRequested(IAICompanionFeatureSwitchHandler* handler) = 0;
	/// \brief The callback when the host handles the request to turn the AI Companion features on or off.
	/// \param timeout Specify the host not handle the request until timeout.
	/// \param bAgree Specify the host agrees to the request to turn the AI companion features on or off.
	/// \param bTurnOn Specify the host respond the request of turn on or off.
	virtual void onAICompanionFeatureSwitchRequestResponse(bool bTimeout, bool bAgree, bool bTurnOn) = 0;
	/// \brief The callback when the started AI Companion feature can't be turned off.
	/// \param features Specify the AI Companion features that can't be turned off.
	virtual void onAICompanionFeatureCanNotBeTurnedOff(IList<AICompanionFeature>* features) = 0;
};

/// \brief Meeting AI Companion controller interface.
class IMeetingAICompanionController
{
public:
	/// \brief Configure the meeting AI companion controller callback event handler.
	/// \param pEvent An object pointer to the IMeetingAICompanionCtrlEvent that receives the meeting AI companion callback event. For more details, see \link IMeetingAICompanionCtrlEvent \endlink.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks The SDK use pEvent to transmit the callback event to the user's application. If the function is not called or fails, the user's application is unable to retrieve the callback event.
	virtual SDKError SetEvent(IMeetingAICompanionCtrlEvent* pEvent) = 0;

	/// \brief Get the smart summary helper.
	/// \return If the function succeeds, the return value is a pointer to IMeetingSmartSummaryHelper. Otherwise the function returns NULL.
	/// \deprecated This interface is marked as deprecated, and is replaced by GetMeetingAICompanionSmartSummaryHelper().
	virtual IMeetingSmartSummaryHelper* GetMeetingSmartSummaryHelper() = 0;

	/// \brief Get the AI companion smart summary helper.
	/// \return If the function succeeds, the return value is a pointer to IMeetingAICompanionSmartSummaryHelper. Otherwise the function returns NULL.
	virtual IMeetingAICompanionSmartSummaryHelper* GetMeetingAICompanionSmartSummaryHelper() = 0;

	/// \brief Get the AI companion query helper.
	/// \return If the function succeeds, the return value is a pointer to IMeetingAICompanionQueryHelper. Otherwise the function returns NULL.
	virtual IMeetingAICompanionQueryHelper* GetMeetingAICompanionQueryHelper() = 0;

	// \brief Determine whether the current meeting supports turning off the AI Companion features.
	/// \return True indicates to support.
	virtual bool IsTurnoffAllAICompanionsSupported() = 0;

	// \brief Determine whether the current meeting supports turning on the AI Companion features.
	/// \return True indicates to support.
	virtual bool IsTurnOnAllAICompanionsSupported() = 0;

	/// \brief Determine whether the current user can turn off the AI Companion features. 
	/// \return True indicates the user can turn off the AI Companion features.
	virtual bool CanTurnOffAllAICompanions() = 0;

	/// \brief Determine whether the current user can turn on the AI Companion features. 
	/// \return True indicates the user can turn on the AI Companion features.
	virtual bool CanTurnOnAllAICompanions() = 0;

	/// \brief Turn off the AI Companion features.
	/// \All AI features including smart summary, smart recording and query can be turned off at once.
	/// \param deleteAssets Specify whether delete the meeting assets when turn off the AI Companion features. 
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnOffAllAICompanions(bool bDeleteAssets) = 0;

	/// \brief Turn on the AI Companion features.
	/// \Only smart summary and query can be turned on at once.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnOnAllAICompanions() = 0;

	/// \brief Determine whether the current user can request host to turn off all started AI Companion features.
	/// \Notices: if the current user join the meeting before the host, they can check CanTurnOffAllAICompanions to turn off the AI Companion features by himself/herself.
	/// \All AI features include smart summary, smart recording and query can be requested to turn off at once.
	/// \return True indicates the user can request host to turn off all started AI Companion features.
	virtual bool CanRequestTurnoffAllAICompanions() = 0;

	/// \brief Determine whether the current user can request host to turn on all AI Companion features if they are enabled for the current meeting.
	/// \Only smart summary and query can be requested to turn on at once.
	/// \return True indicates the user can request host to turn on the AI Companion features.
	virtual bool CanRequestTurnOnAllAICompanions() = 0;

	/// \brief request host to turn off all started AI Companion features.
	/// \All AI features include smart summary, smart recording and query can be requested to turn off at once.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestTurnoffAllAICompanions() = 0;

	/// \brief request host to turn on all AI Companion features if they are enabled for the current meeting.
	/// \Only smart summary and query can be requested to turn on at once.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestTurnOnAllAICompanions() = 0;
};

END_ZOOM_SDK_NAMESPACE
#endif