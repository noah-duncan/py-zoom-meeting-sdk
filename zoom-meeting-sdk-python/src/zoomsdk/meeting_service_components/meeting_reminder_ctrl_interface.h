/*!
* \file meeting_reminder_ctrl_interface.h
* \brief Meeting Service Phone Interface.
* Valid for both ZOOM style and user custom interface mode.
*/
#ifndef _MEETING_REMINDER_CTRL_INTERFACE_H_
#define _MEETING_REMINDER_CTRL_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

/*! \enum MeetingReminderType
	\brief The type for disclaimer dialog.
	Here are more detailed structural descriptions.
*/
enum MeetingReminderType
{
	TYPE_LOGIN_REQUIRED,///Reminder type of login.
	TYPE_START_OR_JOIN_MEETING,///<Reminder type of start or join meeting.
	TYPE_RECORD_REMINDER,///<Reminder type of record reminder.
	TYPE_RECORD_DISCLAIMER,///<Reminder type of record disclaimer
	TYPE_LIVE_STREAM_DISCLAIMER,///<Reminder type of live stream disclaimer.
	TYPE_ARCHIVE_DISCLAIMER,///<Reminder type of archive disclaimer.
	TYPE_WEBINAR_AS_PANELIST_JOIN,///<Reminder type of join webinar as panelist.
	TYPE_TERMS_OF_SERVICE, ///Reminder type of Terms of service or privacy statement changed.
	TYPE_SMART_SUMMARY_DISCLAIMER, ///<Reminder type of smart summary disclaimer.
	TYPE_SMART_SUMMARY_ENABLE_REQUEST_REMINDER, ///<Reminder type of smart summary enable request. This type is marked as deprecated. Replaced with callback \link IMeetingAICompanionSmartSummaryHelperEvent::onSmartSummaryEnableActionCallback \endlink
	TYPE_QUERY_DISCLAIMER, ///<Reminder type of query disclaimer. 
	TYPE_QUERY_ENABLE_REQUEST_REMINDER, ///<Reminder type of query enable request. This type is marked as deprecated.
	TYPE_ENABLE_SMART_SUMMARY_REMINDER,///<Reminder type of enable smart summary. This type is marked as deprecated. Replaced with callback \link IMeetingAICompanionSmartSummaryHelperEvent::onSmartSummaryEnableActionCallback \endlink
	TYPE_WEBINAR_ATTENDEE_PROMOTE_REMINDER,///<Reminder type of webinar promote attendee.
	TYPE_JOIN_PRIVATE_MODE_MEETING_REMINDER,///<Reminder type of joining a meeting with private mode.
	TYPE_SMART_RECORDING_ENABLE_REQUEST_REMINDER,///<Reminder type to enable smart recording request. This type is marked as deprecated.
	TYPE_ENABLE_SMART_RECORDING_REMINDER,///<Reminder type to enable smart recording. This type is marked as deprecated.
	TYPE_AI_COMPANION_PLUS_DISCLAIMER,///<Reminder type of AICompanionPlus disclaimer.
	TYPE_CLOSED_CAPTION_DISCLAIMER,///<Reminder type of Close Caption disclaimer.
	TYPE_MULTI_DISCLAIMER,///<Reminder type of disclaimers combination. 
	TYPE_JOIN_MEETING_CONNECTOR_AS_GUEST_REMINDER,///<Reminder type for a join meeting connector with guest mode.
	TYPE_COMMON_DISCLAIMER,///<Reminder type of common disclaimer.
};

/*! \enum ActionType
	\brief The type of the action which user should take.
	Here are more detailed structural descriptions.
*/
enum ActionType
{
	ACTION_TYPE_NONE,///<Need no more action.
	ACTION_TYPE_NEED_SIGN_IN,///<Need to sign in.
	ACTION_TYPE_NEED_SWITCH_ACCOUNT,///<Need to switch account.
};

#if (defined WIN32 )
/*! \struct tagWndPosition
	\brief The position of the window. The coordinate of position is that of monitor when the parent window is null. If the the parent window is not null, the position coordinate is that of the parent window.
	Here are more detailed structural descriptions.
*/
typedef struct tagMultiReminderUIConfig
{
	int left;///<Specifies the X-axis coordinate of the top-left corner of the multi-reminder window in the parent window.
	int top;///<Specifies the Y-axis coordinate of the top-left corner of the multi-reminder window in the parent window.
	HWND hParent;///<Specifies the window handle of the parent window. If the value is NULL, the position coordinate is the monitor coordinate.
	unsigned long background_color;///<Specifies the background color of the multi-reminder window.
	tagMultiReminderUIConfig()
	{
		left = 0;
		top = 0;
		hParent = NULL;
		background_color = 0xffffff;
	}
}MultiReminderUIConfig;
#endif

/// \brief the interface of reminder dialog content.
class IMeetingReminderContent
{
public:
	virtual ~IMeetingReminderContent() {};
	/// \brief Get the type of the reminder.
	virtual MeetingReminderType  GetType() = 0;
	/// \brief Get the title of the reminder dialog content.
	virtual const zchar_t* GetTitle() = 0;
	/// \brief Get the detail content of the reminder dialog content.
	virtual const zchar_t* GetContent() = 0;
	/// \brief Determine whether block the user join or stay in the meeting
	/// \return True indicates block the user join or stay in the meeting. Otherwise False.
	virtual bool  IsBlocking() = 0;
	/// \brief Get the type of the action which user should take after receiving this reminder content.
	virtual ActionType GetActionType() = 0;
	/// \brief Get a List of reminder's type.
	/// \return List of the reminder's type. 
	virtual IList<MeetingReminderType >* GetMultiReminderTypes() = 0;

};

/// \brief the interface to handle the reminder dialog.
class IMeetingReminderHandler
{
public:
	virtual ~IMeetingReminderHandler() {};
	/// \brief Ignore the reminder.
	virtual SDKError  Ignore() = 0;
	/// \brief Accept the reminder.
	virtual SDKError  Accept() = 0;
	/// \brief Decline the reminder.
	virtual SDKError  Decline() = 0;
	/// \brief Set not show the disclaimer in subsequent meetings.
	virtual SDKError  SetHideFeatureDisclaimers() = 0;
};

/*! \enum FeatureEnableOption
	\brief The option for meeting feature.
*/
enum FeatureEnableOption
{
	EnableOption_None,///do not enable.
	EnableOption_Once,///enable for this meeting.
	EnableOption_Always,///enable for this and all future meetings on this account.
};

/// \brief if the current user can control web setting, they will get this handler when an attendee requests to start the smart recording feature or start feature by itself.
class IMeetingEnableReminderHandler
{
public:
	virtual ~IMeetingEnableReminderHandler() {};

	/// \brief Set the option indicating which meetings feature will be enabled for.
	/// \param option Specify the option. 	
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEnableOption(FeatureEnableOption option) = 0;

	/// \brief Start the feature.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Start() = 0;

	/// \brief Decline the reminder.
	/// \param bDeclineAll TRUE means decline all reminders,and participants cannot send requests again until the host changes the setting. FALSE means that the host declines only this specific request, not all requests.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline(bool bDeclineAll) = 0;

	/// \brief Ignore the reminder.
	virtual SDKError Ignore() = 0;
};

/// \brief Callback event to enable showing the reminder dialog.
class IMeetingReminderEvent
{
public:
	virtual ~IMeetingReminderEvent() {}

	/// \brief Callback event of the reminder dialog show.
	/// \param content The detail content in the reminder dialog. For more details, see \link IMeetingReminderContent \endlink enum.
	/// \param handle A pointer to the IMeetingReminderHandler. For more details, see \link IMeetingReminderHandler \endlink.
	virtual void onReminderNotify(IMeetingReminderContent* content, IMeetingReminderHandler* handle) = 0;

	/// \brief Callback event of the enable reminder dialog show.
	/// \param content The detail content in the reminder dialog. For more details, see \link IMeetingEnableReminderHandler \endlink enum.
	/// \param handle A pointer to the IMeetingReminderHandler. For more details, see \link IMeetingEnableReminderHandler \endlink.
	virtual void onEnableReminderNotify(IMeetingReminderContent* content, IMeetingEnableReminderHandler* handle) = 0;
};
/// \brief Meeting reminder dialog interface.
///
class IMeetingReminderController
{
public:
	/// \brief Set meeting reminder controller callback event handler
	/// \param pEvent A pointer to the IMeetingReminderEvent that receives reminder callback event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingReminderEvent* pEvent) = 0;

#if (defined WIN32 )
	/// \brief Set the customized config of multi-reminder disclaimer. 
	/// \param config the customized config of multi-reminder disclaimer, see \link MultiReminderUIConfig \endlink 
	/// \remarks Valid for user custom interface mode only.	
	virtual void SetMultiReminderDisclaimerUIConfig(MultiReminderUIConfig config) = 0;

	/// \brief Update the position and size of multi-reminder disclaimer window when its parent window moves or changes size.
	/// \remark This interface should be invoked when the OnSize and OnMove messages of the parent window recieved. Valid for user custom interface mode only.
	virtual void UpdateMultiReminderDisclaimerUI() = 0;
#endif
};
END_ZOOM_SDK_NAMESPACE
#endif