/*!
* \file meeting_video_interface.h
* \brief Meeting Service Video Interface
* 
*/
#ifndef _MEETING_VIDEO_INTERFACE_H_
#define _MEETING_VIDEO_INTERFACE_H_
#include "zoom_sdk_def.h"
#if defined(WIN32)
#include "zoom_sdk_util_define.h"
#endif
BEGIN_ZOOM_SDK_NAMESPACE

/*! \enum VideoStatus
    \brief The video status of the user.
    Here are more detailed structural descriptions.
*/
enum VideoStatus
{
	Video_ON, ///<Video is on.
	Video_OFF, ///<Video is off.
	Video_Mute_ByHost, ///<Video is muted by host.
};
/*! \enum VideoConnectionQuality
	\brief The video quality of the user.
	Here are more detailed structural descriptions.
*/
enum VideoConnectionQuality
{
	VideoConnectionQuality_Unknown = 0, //Unknown video quality status.
	VideoConnectionQuality_Bad,  //The video quality is poor.
	VideoConnectionQuality_Normal, //The video quality is normal.
	VideoConnectionQuality_Good, //The video quality is good.
};

typedef struct tagVideoSize
{
	int width;
	int height;
	tagVideoSize()
	{
		memset(this, 0, sizeof(tagVideoSize));   //checked safe
	}
}VideoSize;

/// \brief set video order helper interface.
///
class ISetVideoOrderHelper
{
public:
	/// \brief Prepare to make a new video order.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum
	/// \remarks If the function succeeds, the prepared video order you added by calling \link AddVideoToOrder \endlink will be cleared.
	virtual SDKError SetVideoOrderTransactionBegin() = 0;

	/// \brief Add the assigned user into the prepared video order.
	/// \param userId, the user you want to add into the prepared video order.
	/// \param position, the position you want to place in the prepared video order.
	/// \return true if user is added to the prepared video order successfully.
	/// \remarks The max number of the prepared video order is 49. If you assign many userId with a same order, only the last one will be applied.
	/// \Remark SetVideoOrderTransactionBegin() must be called before this function is called. Otherwise false will be returned.
	virtual bool AddVideoToOrder(unsigned int userId, unsigned int position) = 0;

	/// \brief make a new video order.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum
	/// \Remarks SetVideoOrderTransactionBegin() must be called before this function is called. Otherwise SDKErr_WRONG_USAGE will be returned.
	virtual SDKError SetVideoOrderTransactionCommit() = 0;
};

/// \brief Process after the user receives the requirement from the host to turn on the video.
class IRequestStartVideoHandler
{
public:
	virtual ~IRequestStartVideoHandler(){};
	/// \brief Get the user ID who asks to turn on the video.
	/// \return If the function succeeds, the return value is the user ID. FALSE 0.
	virtual unsigned int GetReqFromUserId() = 0;
	/// \brief Instance to ignore the requirement, return nothing and finally self-destroy.
	virtual SDKError Ignore() = 0;
	/// \brief Instance to accept the requirement, turn on the video and finally self-destroy.
	virtual SDKError Accept() = 0;
	
	/// \brief Ignore the request to enable the video in the meeting and finally the instance self-destroys.
	virtual SDKError Cancel() = 0;
};

/*! \enum CameraControlRequestType
	\brief The camera control request type.
	Here are more detailed structural descriptions.
*/
enum CameraControlRequestType
{
	CameraControlRequestType_Unknown = 0,
	CameraControlRequestType_RequestControl,
	CameraControlRequestType_GiveUpControl,
};

/*! \enum CameraControlRequestResult
	\brief The camera control request result.
	Here are more detailed structural descriptions.
*/
enum CameraControlRequestResult
{
	CameraControlRequestResult_Approve,
	CameraControlRequestResult_Decline,
	CameraControlRequestResult_Revoke,
};

/// \brief Camera control request.
class ICameraControlRequestHandler
{
public:
	virtual ~ICameraControlRequestHandler() {};

	/// \brief Instance to accept the requirement.
	virtual SDKError Approve() = 0;

	/// \brief Instance to decline the requirement and finally self-destroy.
	virtual SDKError Decline() = 0;
};

/// \brief Meeting video controller event callback
///
class IMeetingVideoCtrlEvent
{
public:
	virtual ~IMeetingVideoCtrlEvent() {}

	/// \brief Callback event of the user video status changes.
	/// \param userId The user ID whose video status changes
	/// \param status New video status. For more details, see \link VideoStatus \endlink enum.
	/// \remarks Valid for both normal user and webinar attendee.
	virtual void onUserVideoStatusChange(unsigned int userId, VideoStatus status) = 0;

	/// \brief Callback event for when the video spotlight user list changes.
	/// \Spotlight user means that the view will show only the
	/// \specified user and won't change the view even other users speak.
	/// \param lstSpotlightedUserID spot light user list.
	virtual void onSpotlightedUserListChangeNotification(IList<unsigned int >* lstSpotlightedUserID) = 0;

	/// \brief Callback event of the requirement to turn on the video from the host.
	/// \param handler_ A pointer to the IRequestStartVideoHandler. For more details, see \link IRequestStartVideoHandler \endlink.
	virtual void onHostRequestStartVideo(IRequestStartVideoHandler* handler_) = 0;

	/// \brief Callback event of the active speaker video user changes. 
	/// \param userid The ID of user who becomes the new active speaker.
	virtual void onActiveSpeakerVideoUserChanged(unsigned int userid) = 0;

	/// \brief Callback event of the active video user changes. 
	/// \param userid The ID of user who becomes the new active speaker.
	virtual void onActiveVideoUserChanged(unsigned int userid) = 0;

	/// \brief Callback event of the video order changes.
    /// \param orderList The video order list contains the user ID of listed users.
 	virtual void onHostVideoOrderUpdated(IList<unsigned int >* orderList) = 0;

	/// \brief Callback event of the local video order changes.
	/// \param localOrderList The lcoal video order list contains the user ID of listed users.
	virtual void onLocalVideoOrderUpdated(IList<unsigned int >* localOrderList) = 0;

	/// \brief Notification the status of following host's video order changed.
	/// \param follow Yes means the option of following host's video order is on, otherwise not.
	virtual void onFollowHostVideoOrderChanged(bool bFollow) = 0;

	/// \brief Callback event of the user video quality changes.
	/// \param userId The user ID whose video quality changes
	/// \param quality New video quality. For more details, see \link VideoConnectionQuality \endlink enum.
	virtual void onUserVideoQualityChanged(VideoConnectionQuality quality, unsigned int userid) = 0;

	/// \brief Callback event of video alpha channel mode changes.
	/// \param isAlphaModeOn true means it's in alpha channel mode. Otherwise, it's not.
	virtual void onVideoAlphaChannelStatusChanged(bool isAlphaModeOn) = 0;

	/// \brief Callback for when the current user receives a camera control request.
	/// \This callback will be triggered when another user requests control of the current user's camera.
	/// \param userId The user ID that sent the request
	/// \param requestType The request type. For more details, see \link CameraControlRequestType \endlink enum.
	/// \param pHandler A pointer to the ICameraControlRequestHandler. For more details, see \link ICameraControlRequestHandler \endlink.
	virtual void onCameraControlRequestReceived(unsigned int userId, CameraControlRequestType requestType, ICameraControlRequestHandler* pHandler) = 0;

	/// \brief Callback for when the current user is granted camera control access.
	/// \param userId The user ID that accepted the request
	/// \param isApproved The result of the camera control request.
	virtual void onCameraControlRequestResult(unsigned int userId, CameraControlRequestResult result) = 0;
};

enum PinResult
{
	PinResult_Success = 0,
	PinResult_Fail_NotEnoughUsers,  /// user counts less than 2
	PinResult_Fail_ToMuchPinnedUsers, /// pinned user counts more than 9
	PinResult_Fail_UserCannotBePinned, /// user in view only mode or silent mode or active
	PinResult_Fail_VideoModeDoNotSupport, /// other reasons	
	PinResult_Fail_NoPrivilegeToPin,  /// current user has no privilege to pin
	PinResult_Fail_MeetingDoNotSupport, /// webinar and in view only meeting
	PinResult_Unknown = 100,
};

enum SpotlightResult
{
	SpotResult_Success = 0,
	SpotResult_Fail_NotEnoughUsers,  /// user counts less than 2
	SpotResult_Fail_ToMuchSpotlightedUsers, /// spotlighted user counts is more than 9
	SpotResult_Fail_UserCannotBeSpotlighted, /// user in view only mode or silent mode or active
	SpotResult_Fail_UserWithoutVideo, /// user doesn't turn on video
	SpotResult_Fail_NoPrivilegeToSpotlight,  /// current user has no privilege to spotlight
	SpotResult_Fail_UserNotSpotlighted, ///user is not spotlighted
	SpotResult_Unknown = 100,
};

/// \brief Meeting camera helper interface
///
class IMeetingCameraHelper
{
public:
	virtual ~IMeetingCameraHelper() {}

	/// \brief Gets the current controlled user ID.
	/// \return If the function succeeds, the return value is the user ID. Otherwise, this returns 0.
	virtual unsigned int GetUserId() = 0;

	/// \brief Whether the camera can be controlled or not.
	/// \return true if the user can control camera, false if they can't.
	virtual bool CanControlCamera() = 0;

	/// \brief Request to control remote camera.	
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestControlRemoteCamera() = 0;

	/// \brief Give up control of the remote camera.	
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError GiveUpControlRemoteCamera() = 0;

	/// \brief Turn the camera to the left.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnLeft(unsigned int range = 50) = 0;

	/// \brief Turn the camera to the right.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnRight(unsigned int range = 50) = 0;

	/// \brief Turn the camera up.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnUp(unsigned int range = 50) = 0;

	/// \brief Turn the camera down.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError TurnDown(unsigned int range = 50) = 0;

	/// \brief Zoom the camera in.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ZoomIn(unsigned int range = 50) = 0;

	/// \brief Zoom the camera out.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ZoomOut(unsigned int range = 50) = 0;
};

/// \brief Meeting video controller interface
///
class IMeetingVideoController
{
public:
	/// \brief Set the meeting video controller callback event handler
	/// \param pEvent A pointer to the IRequestStartVideoHandler that receives the video controller event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingVideoCtrlEvent* pEvent) = 0;
	
	/// \brief Turn off the user's own video.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError MuteVideo() = 0;

	/// \brief Turn on the user's own video.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError UnmuteVideo() = 0;

	/// \brief Determine if it is able to spotlight the video of the specified user in the meeting. 
	/// \param userid Specifies the user ID to be determined.
	/// \param [out] result Indicates if it is able to spotlight. It validates only when the return value is SDKErr_Success.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError CanSpotlight(unsigned int userid, SpotlightResult& result) = 0;
	
	/// \brief Determine if it is able to unspotlight the video of the specified user in the meeting. 
	/// \param userid Specifies the user ID to be determined.
	/// \param [out] result Indicates if it is able to unspotlight. It validates only when the return value is SDKErr_Success.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError CanUnSpotlight(unsigned int userid, SpotlightResult& result) = 0;

	/// \brief Spotlight the video of the assigned user to the first view.
	/// \param userid Specifies the user ID to be spotlighted.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError SpotlightVideo(unsigned int userid) = 0;

	/// \brief Unspotlight the video of the assigned user to the first view.
	/// \param userid Specifies the user ID to be unspotlighted.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError UnSpotlightVideo(unsigned int userid) = 0;

	/// \brief Unpin all the videos from the first view.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError UnSpotlightAllVideos() = 0;

	/// \brief Get the list of all the spotlighted user in the meeting.
	/// \return If the function succeeds, the return value is the list of the spotlighted user in the meeting.
	///Otherwise failed, the return value is NULL.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual IList<unsigned int >* GetSpotlightedUserList() = 0;

	/// \brief Query if it is able to demand the specified user to turn on the video.
	/// \param userid Specifies the user ID to query.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError CanAskAttendeeToStartVideo(unsigned int userid) = 0;

	/// \brief Demand the assigned user to turn on the video.
	/// \param userid Specifies the user ID to demand.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError AskAttendeeToStartVideo(unsigned int userid) = 0;

	/// \brief Query if it is able to demand the specified user to turn off the video.
	/// \param userid Specifies the user ID to query.  
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError CanStopAttendeeVideo(unsigned int userid) = 0;

	/// \brief Turn off the video of the assigned user.
	/// \param userid Specifies the user ID to turn off.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError StopAttendeeVideo(unsigned int userid) = 0;

	/// \brief Determine if the following host video order feature is supported.
	/// \return TRUE indicates to support the following host video order feature.
	virtual bool IsSupportFollowHostVideoOrder() = 0;

	/// \brief Enable or disable follow host video order mode.
	/// \param bEnable TRUE indicates to set to enable the follow host video order mode.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableFollowHostVideoOrder(bool bEnable) = 0;

	/// \brief Determine if the follow host video mode is enabled.
	/// \return TRUE indicates to enable the mode. 
	virtual bool IsFollowHostVideoOrderOn() = 0;

	/// \brief Get the video order list.
	/// \return If the function succeeds, the return value the is video order list.
	///Otherwise failed, returns NULL.
	virtual IList<unsigned int >* GetVideoOrderList() = 0;


	/// \brief Determine if the incoming video is stopped.
	/// \return TRUE indicates that the incoming video is stopped. 
	virtual bool IsIncomingVideoStopped() = 0;
#if defined(WIN32)
	/// \brief Determine if it is able to pin the video of the specified user to the first view. 
	/// \param userid Specifies the user ID to be determined.
	/// \param [out] result Indicates if it is able to pin. It validates only when the return value is SDKErr_Success.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError CanPinToFirstView(unsigned int userid, PinResult& result) = 0;

	/// \brief Pin the video of the assigned user to the first view.
	/// \param userid Specifies the user ID to be pinned. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError PinVideoToFirstView(unsigned int userid) = 0;

	/// \brief Unpin the video of the assigned user from the first view.
	/// \param userid Specifies the user ID to be unpinned. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError UnPinVideoFromFirstView(unsigned int userid) = 0;

	/// \brief Unpin all the videos from the first view.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual SDKError UnPinAllVideosFromFirstView() = 0;

	/// \brief Get the list of all the pinned user in the first view.
	/// \return If the function succeeds, the return value is the list of the pinned user in the first view.
	///Otherwise failed, the return value is NULL.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual IList<unsigned int >* GetPinnedUserListFromFirstView() = 0;

	/// \brief Determine if it is able to pin the video of the specified user to the second view. 
	/// \param userid Specifies the user ID to be determined.
	/// \param [out] result Indicates if it is able to pin. It validates only when the return value is SDKErr_Success.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError CanPinToSecondView(unsigned int userid, PinResult& result) = 0;

	/// \brief Pin the video of the assigned user to the second view.
	/// \param userid Specifies the user ID to be pinned. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError PinVideoToSecondView(unsigned int userid) = 0;

	/// \brief Unpin the video of the assigned user from the second view.
	/// \param userid Specifies the user ID to be unpinned. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode. 
	virtual SDKError UnPinVideoFromSecondView(unsigned int userid) = 0;

	/// \brief Get the list of all the pinned user in the second view.
	/// \return If the function succeeds, the return value is the list of the pinned user in the second view.
	///Otherwise failed, the return value is NULL.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual IList<unsigned int >* GetPinnedUserListFromSecondView() = 0;

	/// \brief Display or not the user who does not turn on the video in the video all mode.
	/// \return TRUE indicates to hide, FALSE show.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual SDKError HideOrShowNoVideoUserOnVideoWall(bool bHide) = 0;

	/// \brief Display or not the userself's view.
	/// \return TRUE indicates to hide, FALSE show.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for Zoom style user interface mode.
	virtual SDKError HideOrShowSelfView(bool bHide) = 0;

	/// \brief Get set video order helper interface.
	/// \return If the function succeeds, the return value is a pointer to ISetVideoOrderHelper. Otherwise returns NULL.
	virtual ISetVideoOrderHelper* GetSetVideoOrderHelper() = 0;

	/// \brief Get camera controller interface.
	/// \return If the function succeeds, the return value is a pointer to ICameraController. Otherwise returns NULL.
	virtual ICameraController* GetMyCameraController() = 0;
	
	/// \brief Stop the incoming video.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails and returns an error. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both Zoom style and customize user interface mode.
	virtual SDKError StopIncomingVideo(bool bStop) = 0;
	
	/// \brief Determine if show the last used avatar in the meeting.
	/// \param bShow TRUE indicates to show the last used avatar.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails and returns an error. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ShowAvatar(bool bShow) = 0;

	/// \brief Determine if the meeting is showing the avatar.
	/// \return TRUE indicates the meeting is showing the avatar.
	virtual bool IsShowAvatar() = 0;
#endif

	/// \brief Get camera helper interface.
	/// \return If the function succeeds, the return value is a pointer to IMeetingCameraHelper. Otherwise returns NULL.
	virtual IMeetingCameraHelper* GetMeetingCameraHelper(unsigned int userid) = 0;

	/// \brief Revoke camera control privilege.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RevokeCameraControlPrivilege() = 0;

	/// \brief Determine if alpha channel mode can be enabled. 
	/// \return true means it can be enabled. Otherwise false.
	virtual bool CanEnableAlphaChannelMode() = 0;

	/// \brief Enable or disable video alpha channel mode.
	/// \param enable True indicates to enable alpha channel mode. Otherwise, disable it.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableAlphaChannelMode(bool enable) = 0;

	/// \brief Determine if alpha channel mode is enabled.
	/// \return True indicates alpha channel mode is enabled. Otherwise false.
	virtual bool IsAlphaChannelModeEnabled() = 0;

	/// \brief Get the size of user's video.
	/// \param userid Specifies the user ID. The user id should be 0 when not in meeting.
	/// \return The size of user's video.
	virtual VideoSize GetUserVideoSize(unsigned int userid) = 0;
};
END_ZOOM_SDK_NAMESPACE
#endif