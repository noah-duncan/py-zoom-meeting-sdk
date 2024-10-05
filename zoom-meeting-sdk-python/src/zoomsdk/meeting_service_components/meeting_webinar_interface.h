/*!
* \file meeting_webinar_interface.h
* \brief Meeting Service Webinar Interface.
* 
*/

#ifndef _MEETING_WEBINAR_INTERFACE_H_
#define _MEETING_WEBINAR_INTERFACE_H_
#include "zoom_sdk_def.h"
BEGIN_ZOOM_SDK_NAMESPACE

/// \brief Webinar callback event.
///
class IMeetingWebinarCtrlEvent
{
public:
	virtual ~IMeetingWebinarCtrlEvent() {}

	/// \brief Callback to promote attendees to panelist.
	/// \param result If the promotion is successful, the result is zero(0). Otherwise it is an error code.
	virtual void onPromptAttendee2PanelistResult(int result) = 0;

	/// \brief Callback to demote attendees to panelist.
	/// \param result If the demotion is successful, the result is zero(0), otherwise an error code.
	virtual void onDepromptPanelist2AttendeeResult(int result) = 0;

	/// \brief Callback to enable the panelist to start the video.
	virtual void onAllowPanelistStartVideoNotification() = 0;

	/// \brief Callback to disable the panelist to start the video.
	virtual void onDisallowPanelistStartVideoNotification() = 0;

	/// \brief Callback event that attendees are required to enable the mic in the view-only mode of webinar.
	virtual void onSelfAllowTalkNotification() = 0;

	/// \brief Callback event that attendees are required to turn off the mic in the view-only mode of webinar.
	virtual void onSelfDisallowTalkNotification() = 0;

	/// \brief Callback to enable the attendees to chat. Available only for the host and the co-host.
	virtual void onAllowAttendeeChatNotification() = 0;

	/// \brief Callback to disable the attendees to chat. Available only for the host and the co-host.
	virtual void onDisallowAttendeeChatNotification() = 0;
	
	/// \brief Callback to emoji Reactions Status Changed.
	/// \param can_reaction If the Reaction is allow, the result is true, False not.
	virtual void onAllowWebinarReactionStatusChanged(bool can_reaction) = 0;

	/// \brief Callback to attendee raise hand Status Changed.
	/// \param can_raiseHand If the raise hand is allow, the result is true, False not.
	virtual void onAllowAttendeeRaiseHandStatusChanged(bool can_raiseHand) = 0;

	/// \brief Callback to attendee view the participant count status Changed.
	/// \param can_viewParticipantCount If attendee view the participant count is allow, the result is true, False not.
	virtual void onAllowAttendeeViewTheParticipantCountStatusChanged(bool can_viewParticipantCount) = 0;
  
	/// \brief Attendee will receive this callback if his audio status changes.
	/// \param userid The ID of the user whose audio status changes.
	/// \param can_talk True indicates that it is able to use the audio. False not.  
	/// \param is_muted TRUE indicates muted, FALSE not. This parameter works only when the value of can_talk is TRUE.
	virtual void onAttendeeAudioStatusNotification(unsigned int userid, bool can_talk, bool is_muted) = 0;

	/// \brief When attendee agree or decline the promote invitation, host will receive this callback.
	/// \param agree, if attendee agree return true, otherwise false.
	/// \param userid, The attendee user id.
	virtual void onAttendeePromoteConfirmResult(bool agree, unsigned int userid) = 0;
};

/*! \struct tagWebinarMeetingStatus
    \brief Webinar Meeting Status.
    Here are more detailed structural descriptions.
*/
typedef struct tagWebinarMeetingStatus
{
	bool allow_panellist_start_video;///<TRUE indicates that the panelist is able to turn on the video. FALSE not.
	bool allow_attendee_chat;///<TRUE indicates that the attendee is able to chat. FALSE not.
	bool allow_emoji_reaction;///<TRUE indicates that the attendee is able to emojireaction. FALSE not.
	bool allow_attendee_raise_hand;///<TRUE indicates that the attendee is able to raise hand. FALSE not.
	bool allow_attendee_view_participant_count;///<TRUE indicates that the attendee is able to view participant count. FALSE not.
	tagWebinarMeetingStatus()
	{
		Reset();
	}

	void Reset()
	{
		allow_panellist_start_video = false;
		allow_attendee_chat = false;
		allow_emoji_reaction = false;
		allow_attendee_raise_hand = false;
		allow_attendee_view_participant_count = false;
	}
}WebinarMeetingStatus;

/**
 * @brief Enumerations of the panelist chat privilege.
 */
enum SDKPanelistChatPrivilege
{
	SDKPanelistChatPrivilege_PanelistOnly = 1,	/// Allow panelists only to chat with each other.
	SDKPanelistChatPrivilege_All = 2			/// Allow panelist to chat with everyone.
};
#if defined(WIN32)
/**
 * @brief Enumerations of the attendee view display mode.
 */
enum  SDKAttendeeViewMode
{
	SDKAttendeeViewMode_None,              /// attendee view display mode is invaild
	SDKAttendeeViewMode_FollowHost,        /// follow host
	SDKAttendeeViewMode_Speaker,           /// always view active speaker
	SDKAttendeeViewMode_Gallery,           /// always view gallery
	SDKAttendeeViewMode_Standard,          /// attendee can manually switch between gallery and active speaker
	SDKAttendeeViewMode_SidebysideSpeaker, /// attendee view sharing side by side speaker
	SDKAttendeeViewMode_SidebysideGallery  /// attendee view sharing side by side gallery
};

/*! \struct tagWebinarLegalNoticesExplained
	\brief Webinar Legal notices explained.
	Here are more detailed structural descriptions.
*/
typedef struct tagWebinarLegalNoticesExplainedInfo
{
	const zchar_t* explained_content;
	const zchar_t* url_register_account_owner;
	const zchar_t* url_register_terms;
	const zchar_t* url_register_privacy_policy;
	tagWebinarLegalNoticesExplainedInfo()
	{
		Reset();
	}

	void Reset()
	{
		explained_content = nullptr;
		url_register_account_owner = nullptr;
		url_register_terms = nullptr;
		url_register_privacy_policy = nullptr;
	}
}WebinarLegalNoticesExplainedInfo;
#endif
/// \brief Webinar controller interface
///
class IMeetingWebinarController
{
public:
	/// \brief Set webinar controller callback event handler.
	/// \param pEvent A pointer to the IMeetingWebinarCtrlEvent that receives the webinar callback event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingWebinarCtrlEvent* pEvent) = 0;

	/// \brief Promote the attendee to panelist. Available only for the meeting host.
	/// \param userid Specifies the user ID to promote.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onPromptAttendee2PanelistResult() callback event.
	virtual SDKError PromptAttendee2Panelist(unsigned int userid) = 0;

	/// \brief Demote the panelist to attendee. Available only for the host.
	/// \param userid Specifies the user ID to demote.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onDepromptPanelist2AttendeeResult() callback event.
	virtual SDKError DepromptPanelist2Attendee(unsigned int userid) = 0;

	/// \brief Query if the webinar supports the user to use the audio device.
	/// \return If it supports, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError IsSupportAttendeeTalk() = 0;

	/// \brief The attendee is permitted to use the audio device.
	/// \param userid Specifies the permitted user ID.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowAttendeeChatNotification() callback event. Available only for the host.
	virtual SDKError AllowAttendeeTalk(unsigned int userid) = 0;

	/// \brief Forbid the attendee to use the audio device.
	/// \param userid Specifies the forbidden user ID.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onDisallowAttendeeChatNotification() callback event. Available only for the host.
	virtual SDKError DisallowAttendeeTalk(unsigned int userid) = 0;

	/// \brief The panelist is permitted to start the video.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowPanelistStartVideoNotification() callback event. Available only for the host.
	virtual SDKError AllowPanelistStartVideo() = 0;

	/// \brief Forbid the panelist to start video.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onDisallowPanelistStartVideoNotification() callback event. Available only for the host.
	virtual SDKError DisallowPanelistStartVideo() = 0;

	/// \brief Permitted to use emoji reactions.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowWebinarReactionStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError AllowWebinarEmojiReaction() = 0;

	/// \brief Forbid to use emoji reactions.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowWebinarReactionStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError DisallowWebinarEmojiReaction() = 0;

	/// \brief Determine if current webinar support emoji reaction.
	/// \return True means the current webinar supports emoji reactions. Otherwise false.
	virtual bool IsWebinarEmojiReactionSupported() = 0;

	/// \brief The attendee is permitted to use the raise hand.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowAttendeeRaiseHandStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError AllowAttendeeRaiseHand() = 0;

	/// \brief Forbid the attendee to use the raise hand.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowAttendeeRaiseHandStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError DisallowAttendeeRaiseHand() = 0;

	/// \brief The attendee is permitted to view the participant count.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowAttendeeViewTheParticipantCountStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError AllowAttendeeViewTheParticipantCount() = 0;

	/// \brief Forbid the attendee to view the participant count.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If the function succeeds, the user will receive the IMeetingWebinarCtrlEvent::onAllowAttendeeViewTheParticipantCountStatusChanged(bool) callback event. Available only for the host.
	virtual SDKError DisallowAttendeeViewTheParticipantCount() = 0;

	/// \brief Get the participant count.
    /// \return The count of participant.
	virtual int GetParticipantCount() = 0;

	/// \brief Get the webinar status.
	/// \return The status of webinar. For more details, see \link WebinarMeetingStatus \endlink.
	virtual WebinarMeetingStatus* GetWebinarMeetingStatus() = 0;

	/// \brief Set the chat privilege of the panelist.
	/// \param privilege The chat priviledge of the panelist.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetPanelistChatPrivilege(SDKPanelistChatPrivilege privilege) = 0;

	/// \brief Get the chat privilege of the panelist.
	/// \param privilege The chat priviledge of the panelist. It validates only when the return value is SDKErr_Success. 
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError GetPanelistChatPrivilege(SDKPanelistChatPrivilege& privilege) = 0;

#if defined(WIN32)
	/// \brief Set the view mode of the attendee. Available only for zoom ui.
    /// \param mode The view mode of the attendee.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS..
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetAttendeeViewMode(SDKAttendeeViewMode mode) = 0;

	/// \brief Get the view mode of the attendee.Available only for zoom ui.
    /// \return If the function succeeds, it will return the attendee's view mode. For more details, see \link ZoomSDKAttendeeViewMode \endlink.
	virtual SDKAttendeeViewMode GetAttendeeViewMode() = 0;

	/// \brief Get the webinar legal notices prompt.
	/// \return The webinar legal notices prompt.
	virtual const zchar_t* getWebinarLegalNoticesPrompt() = 0;
	
	/// \brief Get the webinar legal notices explained.
	/// \return The webinar legal notices explained.
	virtual bool getWebinarLegalNoticesExplained(WebinarLegalNoticesExplainedInfo& explained_info) = 0;
#endif
};

END_ZOOM_SDK_NAMESPACE
#endif