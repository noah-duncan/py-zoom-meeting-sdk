/*!
* \file meeting_participants_ctrl_interface.h
* \brief Meeting Participants Controller Interface.
* 
*/
#ifndef _MEETING_ParticipantsCtrl_INTERFACE_H_
#define _MEETING_ParticipantsCtrl_INTERFACE_H_
#include "zoom_sdk_def.h"
#include "meeting_service_components/meeting_recording_interface.h"
#if defined(WIN32)
#include "meeting_service_components/meeting_emoji_reaction_interface.h"
#endif
BEGIN_ZOOM_SDK_NAMESPACE
/*! \enum UserRole
    \brief Role of user.
    Here are more detailed structural descriptions.
*/
enum UserRole
{
	USERROLE_NONE,///<For initialization.
	USERROLE_HOST,///<Role of the host.
	USERROLE_COHOST,///<Role of co-host.
	USERROLE_PANELIST,///<Role of the panelist, valid only in webinar.
	USERROLE_BREAKOUTROOM_MODERATOR,///<Host role in breakout room.
	USERROLE_ATTENDEE,///<Role of attendee.
};

/*! \struct tagWebinarAttendeeStatus
    \brief Status of webinar attendee.
    Here are more detailed structural descriptions.
*/
typedef struct tagWebinarAttendeeStatus
{
	bool allow_talk;///<TRUE indicates that it is able to talk.
	tagWebinarAttendeeStatus()
	{
		allow_talk = false;
	}
}WebinarAttendeeStatus;

/*! \enum FocusModeShareType
	\brief Type of focus mode.
	Here are more detailed structural descriptions.
*/
enum FocusModeShareType
{
	FocusModeShareType_None,
	FocusModeShareType_HostOnly,
	FocusModeShareType_AllParticipants,
};

/// \brief User information interface.
///
class IUserInfo
{
public:
	/// \brief Get the username matched with the current user information.
	/// \return If the function succeeds, the return value is the username.
	///Otherwise failed, the return value is NULL.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual const zchar_t* GetUserName() = 0;

	/// \brief Determine whether the member corresponding with the current information is the host or not.
	/// \return TRUE indicates the host.
	virtual bool IsHost() = 0;

	/// \brief Get the user ID matched with the current user information.
	/// \return If the function succeeds, the return value is the user ID.
	///Otherwise the function fails, and the return value is ZERO(0).
	/// \remarks Valid for both normal user and webinar attendee.
	virtual unsigned int GetUserID() = 0;

	/// \brief Get the avatar file path matched with the current user information.
	/// \return If the function succeeds, the return value is the avatar file path.
	///Otherwise failed, the return value is NULL.
	virtual const zchar_t* GetAvatarPath() = 0;

	/// \brief Get the user persistent id matched with the current user information.
	/// \return If the function succeeds, the return value is the user persistent id.
	///Otherwise failed, the return value is NULL.
	virtual const zchar_t* GetPersistentId() = 0;

	/// \brief Get the customer_key matched with the current user information.
	///If you assign a customer_key for a user in the start/join meeting parameter, the value you assigned will be returned.
	///Otherwise a empty string will be returned.
	/// \return If the function succeeds, the return value is the customer_key.
	///Otherwise failed, the return value is NULL.
	virtual const zchar_t* GetCustomerKey() = 0;

	/// \brief Determine the video status of the user specified by the current information.
	/// \return TRUE indicates that the video is turned on.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual bool IsVideoOn() = 0;

	/// \brief Determine the audio status of the user specified by the current information.
	/// \return TRUE indicates that the audio status is muted.
	virtual bool IsAudioMuted() = 0;

	/// \brief Get the audio type of the user specified by the current information when joins the meeting.
	/// \return The type of audio when the user joins the meeting. For more details, see \link AudioType \endlink enum.
	virtual AudioType GetAudioJoinType() = 0;

	/// \brief Determine whether the current information corresponds to the user himself or not.
	/// \return TRUE indicates that the current information corresponds to the user himself.
	virtual bool IsMySelf() = 0;

	/// \brief Determine whether the user specified by the current information is in the waiting room or not.
	/// \return TRUE indicates that the specified user is in the waiting room.
	virtual bool IsInWaitingRoom() = 0;

	/// \brief Determine whether the user specified by the current information raises hand or not.
	/// \return TRUE indicates that the user raises hand.
	virtual bool IsRaiseHand() = 0;

	/// \brief Get the type of role of the user specified by the current information.
	/// \return The role of the user. For more details, see \link UserRole \endlink enum.
	virtual UserRole GetUserRole() = 0;

	/// \brief Determine whether the user corresponding to the current information joins the meeting by telephone or not.
	/// \return TRUE indicates that the user joins the meeting by telephone.
	virtual bool IsPurePhoneUser() = 0;

	/// \brief Get the Mic level of the user corresponding to the current information.
	/// \return The mic level of the user.
	virtual int GetAudioVoiceLevel() = 0;

	/// \brief Determine whether the user corresponding to the current information is the sender of Closed Caption or not.
	/// \return TRUE indicates that the user is the sender of Closed Caption.
	virtual bool IsClosedCaptionSender() = 0;

	/// \brief Determine whether the user specified by the current information is talking or not.
	/// \return TRUE indicates that the specified user is talking.
	virtual bool IsTalking() = 0;
	
	/// \brief Determine whether the user specified by the current information is H323 user or not.
	/// \return TRUE indicates that the specified user is H323 user.
	virtual bool IsH323User() = 0;

	/// \brief Get the webinar status of the user specified by the current information.
	/// \return The status of the specified user. For more details, see \link WebinarAttendeeStatus \endlink structure.
	virtual WebinarAttendeeStatus* GetWebinarAttendeeStatus() = 0;
#if defined(WIN32)
	/// \brief Determine whether the user specified by the current information is a interpreter or not.
	/// \return TRUE indicates that the specified user is a interpreter, otherwise not.
	virtual bool IsInterpreter() = 0;

	/// \brief Determine whether the user specified by the current information is a sign language interpreter or not.
	/// \return TRUE indicates that the specified user is a sign language  interpreter, otherwise false.
	virtual bool IsSignLanguageInterpreter() = 0;

	/// \brief Get the active language, if the user is a interpreter.
	/// \return If success, the return value is the active language abbreviation, Otherwise the return value is ZERO(0).
	virtual const zchar_t* GetInterpreterActiveLanguage() = 0;

	/// \brief Get the emoji feedback type of the user.
	/// \return The emoji feedback type of the user. For more details, see \link SDKEmojiFeedbackType \endlink enum.
	virtual SDKEmojiFeedbackType GetEmojiFeedbackType() = 0;
#endif
	/// \brief Get the local recording status.
	/// \return The status of the local recording status. For more details, see \link RecordingStatus \endlink structure
	virtual RecordingStatus GetLocalRecordingStatus() = 0;

	/// \brief Determine whether the user has started a raw live stream.
	/// \return TRUE indicates that the specified user has started a raw live stream, otherwise false.
	virtual bool IsRawLiveStreaming() = 0;

	/// \brief Determine whether the user has raw live stream privilege.
	/// \return TRUE indicates that the specified user has raw live stream privilege, otherwise false.
	virtual bool HasRawLiveStreamPrivilege() = 0;

	/// \brief Query if the participant has a camera.
	/// \return TRUE means the user has a camera, otherwise false.
	virtual bool HasCamera() = 0;

	virtual ~IUserInfo(){};
};

/// \brief Meeting Participants Controller Callback Event.
///
class IMeetingParticipantsCtrlEvent
{
public:
	virtual ~IMeetingParticipantsCtrlEvent() {}

	/// \brief Callback event of notification of users who are in the meeting.
	/// \param lstUserID List of user IDs. 
	/// \param strUserList List of users in JSON format. This function is currently invalid, hereby only for reservations.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual void onUserJoin(IList<unsigned int >* lstUserID, const zchar_t* strUserList = NULL) = 0;

	/// \brief Callback event of notification of user who leaves the meeting.
	/// \param lstUserID List of the user ID who leaves the meeting.
	/// \param strUserList List of the users in JSON format. This function is currently invalid, hereby only for reservations.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual void onUserLeft(IList<unsigned int >* lstUserID, const zchar_t* strUserList = NULL) = 0;

	/// \brief Callback event of notification of the new host. 
	/// \param userId Specify the ID of the new host. 
	virtual void onHostChangeNotification(unsigned int userId) = 0;

	/// \brief Callback event of changing the state of the hand.
	/// \param bLow TRUE indicates to put down the hand, FALSE indicates to raise the hand. 
	/// \param userid Specify the user ID whose status changes.
	virtual void onLowOrRaiseHandStatusChanged(bool bLow, unsigned int userid) = 0;

	/// \brief Callback event of changing the screen name. 
	/// \param userId list Specify the users ID whose status changes.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual void onUserNamesChanged(IList<unsigned int>* lstUserID) = 0;

	/// \brief Callback event of changing the co-host.
	/// \param userId Specify the user ID whose status changes. 
	/// \param isCoHost TRUE indicates that the specified user is co-host.
	virtual void onCoHostChangeNotification(unsigned int userId, bool isCoHost) = 0;

	/// \brief Callback event of invalid host key.
	virtual void onInvalidReclaimHostkey() = 0;

	/// \brief Callback event of the host calls the lower all hands interface, the host/cohost/panelist will receive this callback.
	virtual void onAllHandsLowered() = 0;

	/// \brief Callback event that the status of local recording changes.
	/// \param userId Specify the user ID whose status changes. 
	/// \param status Value of recording status. For more details, see \link RecordingStatus \endlink enum.
	virtual void onLocalRecordingStatusChanged(unsigned int user_id, RecordingStatus status) = 0;

	/// \brief Callback event that lets participants rename themself.
	/// \param bAllow True allow. If false, participants may not rename themselves.
	virtual void onAllowParticipantsRenameNotification(bool bAllow) = 0;

	/// \brief Callback event that lets participants unmute themself.
	/// \param bAllow True allow. If false, participants may not rename themselves.
	virtual void onAllowParticipantsUnmuteSelfNotification(bool bAllow) = 0;

	/// \brief Callback event that lets participants start a video.
	/// \param bAllow True allow. If false, disallow.
	virtual void onAllowParticipantsStartVideoNotification(bool bAllow) = 0;

	/// \brief Callback event that lets participants share a new whiteboard.
	/// \param bAllow True allow. If false, participants may not share new whiteboard.
	virtual void onAllowParticipantsShareWhiteBoardNotification(bool bAllow) = 0;

	/// \brief Callback event that the request local recording privilege changes.
	/// \param status Value of request local recording privilege status. For more details, see \link LocalRecordingRequestPrivilegeStatus \endlink enum.
	virtual void onRequestLocalRecordingPrivilegeChanged(LocalRecordingRequestPrivilegeStatus status) = 0;

	/// \brief Callback event that lets participants request that the host starts cloud recording.
	/// \param bAllow True allow. If false, disallow.
	virtual void onAllowParticipantsRequestCloudRecording(bool bAllow) = 0;

	/// \brief Callback event that the user avatar path is updated in the meeting.
	/// \param userID Specify the user ID whose avatar updated. 
	virtual void onInMeetingUserAvatarPathUpdated(unsigned int userID) = 0;

	/// \brief Callback event that participant profile status change.
	/// \param bHide true means hide participant profile picture, false means show participant profile picture. 
	virtual void onParticipantProfilePictureStatusChange(bool bHidden) = 0;

	/// \brief Callback event that focus mode changed by host or co-host.
	/// \param bEnabled True means the focus mode change to on. Otherwise off.
	virtual void onFocusModeStateChanged(bool bEnabled) = 0;

	/// \brief Callback event that that focus mode share type changed by host or co-host.
	/// \param type Share type change.
	virtual void onFocusModeShareTypeChanged(FocusModeShareType type) = 0;
};

/// \brief Meeting waiting room controller interface
///
class IMeetingParticipantsController
{
public:
	/// \brief Set the participants controller callback event handler.
	/// \param pEvent A pointer to the IParticipantsControllerEvent that receives the participants event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingParticipantsCtrlEvent* pEvent) = 0;

	/// \brief Get the list of all the panelists in the meeting.
	/// \return If the function succeeds, the return value is the list of the panelists in the meeting.
	///Otherwise the function fails, and the return value is NULL.
	/// \remarks Valid for both ZOOM style and user custom interface mode. Valid for both normal user and webinar attendee.
	virtual IList<unsigned int >* GetParticipantsList() = 0;

	/// \brief Get the information of specified user.
	/// \param userid Specify the user ID for which you want to get the information. 
	/// \return If the function succeeds, the return value is a pointer to the IUserInfo. For more details, see \link IUserInfo \endlink.
	///Otherwise the function fails, and the return value is NULL.
	/// \remarks Valid for both ZOOM style and user custom interface mode. Valid for both normal user and webinar attendee.
	virtual IUserInfo* GetUserByUserID(unsigned int userid) = 0;

	/// \brief Get the information of current user.
	/// \return If the function succeeds, the return value is a pointer to the IUserInfo. For more details, see \link IUserInfo \endlink.
	///Otherwise failed, the return value is NULL.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual IUserInfo* GetMySelfUser() = 0;

	/// \brief Cancel all hands raised.
	/// \param forWebinarAttendees is true, the SDK sends the lower all hands command only to webinar attendees.
	/// \forWebinarAttendees is false, the SDK sends the lower all hands command to anyone who is not a webinar attendee, such as the webinar host/cohost/panelist or everyone in a regular meeting.. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError LowerAllHands(bool forWebinarAttendees) = 0;

	/// \brief Change the screen name of specified user. Only the host or co-host can change the others' name.
	/// \param userid Specify the user ID whose name needed to be changed. 
	/// \param userName Specify a new screen name for the user.
	/// \param bSaveUserName Save the screen name to join the meeting next time.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError ChangeUserName(const unsigned int userid, const zchar_t* userName, bool bSaveUserName) = 0;

	/// \brief Cancel the hands raised of specified user.
	/// \param userid Specify the user ID to put down the hands.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError LowerHand(unsigned int userid) = 0;

	/// \brief Raise hands in the meeting.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError RaiseHand() = 0;

	/// \brief Set the specified user as the host.
	/// \param userid Specify the user ID to be the host.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError MakeHost(unsigned int userid) = 0;

	/// \brief Determine if it is able to change the specified user role as the co-host.
	/// \param userid Specify the user ID.
	/// \return If the specified user can be the co-host, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError CanbeCohost(unsigned int userid) = 0;

	/// \brief Set the specified user as the co-host.
	/// \param userid Specify the user ID who is to be the co-host.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError AssignCoHost(unsigned int userid) = 0;

	/// \brief Get back the co-host role from the specified user.
	/// \param userid Specify the user ID to get back the co-host.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError RevokeCoHost(unsigned int userid) = 0;

	/// \brief Expel the specified user.
	/// \param userid Specify the ID of user to be expelled.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError ExpelUser(unsigned int userid) = 0;

	/// \brief Check whether myself is original host.
	/// \return true means that myself is original host, otherwise not.
	virtual bool IsSelfOriginalHost() = 0;

	/// \brief Reclaim the role of the host.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual SDKError ReclaimHost() = 0;

	/// \brief Determine if the user has the right to reclaim the host role.
	/// \param [out] bCanReclaimHost TRUE indicates to have the right to reclaim the host role.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError CanReclaimHost(bool& bCanReclaimHost) = 0;

	/// \brief Reclaim role of host via host_key.
	/// \param host_key The key to get the role of host.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError ReclaimHostByHostKey(const zchar_t* host_key) = 0;

	virtual SDKError AllowParticipantsToRename(bool bAllow) = 0;

	virtual bool IsParticipantsRenameAllowed() = 0;

	virtual SDKError AllowParticipantsToUnmuteSelf(bool bAllow) = 0;

	virtual bool IsParticipantsUnmuteSelfAllowed() = 0;

	virtual SDKError AskAllToUnmute() = 0;

	/// \brief Allowing the regular attendees to start video, it can only be used in regular meeetings(no bo).
	/// \param bAllow TRUE indicates Allowing the regular attendees to start video. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AllowParticipantsToStartVideo(bool bAllow) = 0;

	/// \brief Check whether the current meeting allows participants to start video, it can only be used in regular meeetings(no bo).
	/// \return If allows participants to start video, the return value is true.
	virtual bool IsParticipantsStartVideoAllowed() = 0;

	/// \brief Allowing the regular attendees to share whiteboard, it can only be used in regular meeetings(no bo).
	/// \param bAllow TRUE indicates Allowing the regular attendees to share whiteboard. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AllowParticipantsToShareWhiteBoard(bool bAllow) = 0;

	/// \brief Check whether the current meeting allows participants to share whiteboard, it can only be used in regular meeetings(no bo).
	/// \return If allows participants to share whiteboard, the return value is true.
	virtual bool IsParticipantsShareWhiteBoardAllowed() = 0;

	/// \brief Allowing the regular attendees to use chat, it can only be used in regular meeetings(no webinar or bo).
	/// \param bAllow TRUE indicates Allowing the regular attendees to use chat. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual SDKError AllowParticipantsToChat(bool bAllow) = 0;

	/// \brief  Check whether the current meeting allows participants to chat, it can only be used in regular meeetings(no webinar or bo)..
	/// \return If allows participants to chat, the return value is true.
	/// \remarks Valid for both ZOOM style and user custom interface mode..
	virtual bool IsParticipantAllowedToChat() = 0;

	/// \brief Check whether the current meeting allows participants to send local recording privilege request, it can only be used in regular meeetings(no webinar or bo).
	/// \return If allows participants to send request, the return value is true.
	virtual bool IsParticipantRequestLocalRecordingAllowed() = 0;

	/// \brief Allowing the regular attendees to send local recording privilege request, it can only be used in regular meeetings(no bo).
	/// \param bAllow TRUE indicates Allowing the regular attendees to send local recording privilege request. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AllowParticipantsToRequestLocalRecording(bool bAllow) = 0;

	/// \brief Check whether the current meeting auto grant participants local recording privilege request, it can only be used in regular meeetings(no webinar or bo).
	/// \return If auto grant participants local recording privilege request, the return value is true.
	virtual bool IsAutoAllowLocalRecordingRequest() = 0;

	/// \brief Auto grant or deny the regular attendee's local recording privilege request, it can only be used in regular meeetings(no bo).
	/// \param bAllow TRUE indicates Auto grant or deny the regular attendee's local recording privilege request. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AutoAllowLocalRecordingRequest(bool bAllow) = 0;

	/// \brief Determine if the current user can hide participant profile pictures.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanHideParticipantProfilePictures() = 0;

	/// \brief Check whether the current meeting hides participant pictures.
	/// \return If participants profile pictures be hidden, the return value is true.
	virtual bool IsParticipantProfilePicturesHidden() = 0;

	/// \brief Hide/Show participant profile pictures.
	/// \param bHide TRUE indicates Hide participant profile pictures. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError HideParticipantProfilePictures(bool bHide) = 0;

	/// \brief Determine if the focus mode enabled or not by web portal.
	/// \return True means focus mode enabled. Otherwise not.
	virtual bool IsFocusModeEnabled() = 0;

	/// \brief Determine if the focus mode on or off.
	/// \return True means focus mode on. Otherwise off.
	virtual bool IsFocusModeOn() = 0;

	/// \brief Turn focus mode on or off. Focus mode on means Participants will only be able to see hosts' videos and shared content, and videos of spotlighted participants.
	/// \param turnOn True means to turen on, false means to turn off.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnFocusModeOn(bool turnOn) = 0;

	/// \brief Get focus mode share type indicating who can see the shared content which is controlled by host or co-host.
	/// \return The current focus mode share type.
	virtual FocusModeShareType GetFocusModeShareType() = 0;

	/// \brief Set the focus mode type indicating who can see the shared content which is controlled by host or co-host.
	/// \param shareType The type of focus mode share type.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetFocusModeShareType(FocusModeShareType shareType) = 0;

	/// \brief Determine if the current user can enable participant request clould recording.
	/// \return true means the current user can enable participant request clould recording.
	virtual bool CanEnableParticipantRequestCloudRecording() = 0;

	/// \brief Check whether the current meeting allows participants to send cloud recording privilege request, This can only be used in regular meeetings and webinar(no breakout rooms).
	/// \return If allows participants to send request, the return value is true.
	virtual bool IsParticipantRequestCloudRecordingAllowed() = 0;

	/// \brief Toggle whether attendees can requests for the host to start a cloud recording. This can only be used in regular meeetings and webinar(no breakout rooms).
	/// \param bAllow TRUE indicates that participants are allowed to send cloud recording privilege requests.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AllowParticipantsToRequestCloudRecording(bool bAllow) = 0;
};
END_ZOOM_SDK_NAMESPACE
#endif