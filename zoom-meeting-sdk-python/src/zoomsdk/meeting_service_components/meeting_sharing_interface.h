/*!
* \file meeting_sharing_interface.h
* \brief Meeting Service Sharing Interface
* 
*/
#ifndef _MEETING_SHARING_INTERFACE_H_
#define _MEETING_SHARING_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE
/*! \enum ShareSettingType
	\brief Share setting type.
	Here are more detailed structural descriptions..
*/
enum ShareSettingType
{
	ShareSettingType_LOCK_SHARE, ///<Only host can share, the same as "lock share"
	ShareSettingType_HOST_GRAB,   ///<Anyone can share, but one sharing only at one moment, and only host can grab other's sharing
	ShareSettingType_ANYONE_GRAB,  ///<Anyone can share, but one sharing only at one moment, and anyone can grab other's sharing
	ShareSettingType_MULTI_SHARE,  ///<Anyone can share, Multi-share can exist at the same time
	
};

/*! \enum AudioShareMode
	\brief Audio share mode.
	Here are more detailed structural descriptions.
*/
enum AudioShareMode
{
	AudioShareMode_Mono,		///Mono mode.
	AudioShareMode_Stereo		///Stereo mode
};

/*! \struct tagViewableShareSource
    \brief Visible shared source information.
    Here are more detailed structural descriptions..
*/
typedef struct tagViewableShareSource
{
	unsigned int userid;///<User ID.
	bool isShowingInFirstView;///<Display or not on the primary view. Valid for ZOOM style only.
	bool isShowingInSecondView;///<Display or not on the secondary view. Valid for ZOOM style only.
	bool isCanBeRemoteControl;///<Enable or disable the remote control.
	tagViewableShareSource()
	{
		userid = 0;
		isShowingInFirstView = false;
		isShowingInSecondView = false;
		isCanBeRemoteControl = false;
	}
}ViewableShareSource;

/*! \enum AdvanceShareOption 
    Additional type of current sharing sent to others.
    Here are more detailed structural descriptions.
*/
enum AdvanceShareOption
{
	AdvanceShareOption_ShareFrame,///<Type of sharing a selected area of desktop.
	AdvanceShareOption_PureComputerAudio,///<Type of sharing only the computer audio.
	AdvanceShareOption_ShareCamera,///<Type of sharing the camera.
};
/*! \struct tagShareInfo
    \brief Information of current sharing.
    Here are more detailed structural descriptions.
*/
enum MultiShareOption
{
	Enable_Multi_Share = 0, ///<Multi-participants can share simultaneously.
	Enable_Only_HOST_Start_Share, ///<Only host can share at a time.
	Enable_Only_HOST_Grab_Share, ///<One participant can share at a time, during sharing only host can start a new sharing and the previous sharing will be replaced.
	Enable_All_Grab_Share, ///<One participant can share at a time, during sharing everyone can start a new sharing and the previous sharing will be replaced.
};

enum ZoomSDKVideoFileSharePlayError
{
	ZoomSDKVideoFileSharePlayError_None, 
	ZoomSDKVideoFileSharePlayError_Not_Supported, ///<Not supported.
	ZoomSDKVideoFileSharePlayError_Resolution_Too_High, ///<The resolution is too high to play.
	ZoomSDKVideoFileSharePlayError_Open_Fail, ///<Failed to open.
	ZoomSDKVideoFileSharePlayError_Play_Fail, ///<Failed to play.
	ZoomSDKVideoFileSharePlayError_Seek_Fail  ///<Failed to seek.
};

typedef struct tagShareInfo
{
	ShareType eShareType;///<Type of sharing, see \link ShareType \endlink enum.
	union
	{
		HWND hwndSharedApp;///<Handle of sharing application or white-board. It is invalid unless the value of the eShareType is SHARE_TYPE_AS or SHARE_TYPE_WB.
		const zchar_t* monitorID;///<The ID of screen to be shared. It is invalid unless the value of the eShareType is SHARE_TYPE_DS.
	}ut;
	tagShareInfo()
	{
		eShareType = SHARE_TYPE_UNKNOWN;
		memset(&ut, 0, sizeof(ut));  //checked safe
	}
}ShareInfo;

/// \brief Reminder handler of switching from multi-share to single share.
///
class IShareSwitchMultiToSingleConfirmHandler
{
public:
	/// \brief Cancel to switch multi-share to single share. All sharing will be remained.
	virtual SDKError Cancel() = 0;

	/// \brief Switch multi-share to single share. All sharing will be stopped.
	virtual SDKError Confirm() = 0;

	virtual ~IShareSwitchMultiToSingleConfirmHandler() {};
};

/// \brief Callback event of meeting share controller.
///
class IMeetingShareCtrlEvent
{
public:
	virtual ~IMeetingShareCtrlEvent() {}

	/// \brief Callback event of the changed sharing status. 
	/// \param status The values of sharing status. For more details, see \link SharingStatus \endlink enum.
	/// \param userId Sharer ID. 
	/// \remarks The userId changes according to the status value. When the status value is the Sharing_Self_Send_Begin or Sharing_Self_Send_End, the userId is the user own ID. Otherwise, the value of userId is the sharer ID.
	virtual void onSharingStatus(SharingStatus status, unsigned int userId) = 0;

	/// \brief Callback event of locked share status.
	/// \param bLocked TRUE indicates that it is locked. FALSE unlocked.
	virtual void onLockShareStatus(bool bLocked) = 0;

	/// \brief Callback event of changed sharing information.
	/// \param shareInfo Sharing information. For more details, see \link ShareInfo \endlink structure.
	virtual void onShareContentNotification(ShareInfo& shareInfo) = 0;

	/// \brief Callback event of switching multi-participants share to one participant share.
	/// \param handler_ An object pointer used by user to complete all the related operations. For more details, see \link IShareSwitchMultiToSingleConfirmHandler \endlink.
	virtual void onMultiShareSwitchToSingleShareNeedConfirm(IShareSwitchMultiToSingleConfirmHandler* handler_) = 0;	

	/// \brief Callback event of sharing setting type changed.
	/// \param type Sharing setting type. For more details, see \link ShareSettingType \endlink structure.
	virtual void onShareSettingTypeChangedNotification(ShareSettingType type) = 0;

	/// \brief Callback event of the shared video's playback has completed.
	virtual void onSharedVideoEnded() = 0;

	/// \brief Callback event of the video file playback error.
	/// \param error The error type. For more details, see \link ZoomSDKVideoFileSharePlayError \endlink structure.
	virtual void onVideoFileSharePlayError(ZoomSDKVideoFileSharePlayError error) = 0;
};

/// \brief Meeting share controller interface.
///
class IMeetingShareController
{
public:
	/// \brief Set meeting share controller callback event handler.
	/// \param pEvent A pointer to the IMeetingShareCtrlEvent that receives sharing event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingShareCtrlEvent* pEvent) = 0;
#if defined(WIN32)
	/// \brief Share the specified application.
	/// \param hwndSharedApp Specify the window handle of the application to be shared. If the hwndSharedApp can't be shared, the return value is the SDKERR_INVALID_PARAMETER error code. If the hwndSharedApp is NULL, the primary monitor will be shared. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StartAppShare(HWND hwndSharedApp) = 0;

	/// \brief Share the specified monitor.
	/// \param monitorID Specify the monitor ID to be shared. You may get the value via EnumDisplayMonitors System API. If the monitorID is NULL, the primary monitor will be shared. For more details, see szDevice in MONITORINFOEX structure.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StartMonitorShare(const zchar_t* monitorID) = 0;

	/// \brief A dialog box pops up that enable the user to choose the application or window to share.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for ZOOM style mode. 
	virtual SDKError ShowSharingAppSelectWnd() = 0;

	/// \brief Start sharing with mobile device. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StartAirPlayShare() = 0;

	/// \brief Start sharing camera.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for ZOOM style only.	
	virtual SDKError StartShareCamera() = 0;

	/// \brief Block the window when sharing in full screen.
	///Once the function is called, you need to redraw the window to take effect.
	/// \param bBlock TRUE indicates to block the window when sharing in full screen.
	/// \param hWnd Specify the window to be blocked.
	/// \param bChangeWindowStyle If it is FALSE, please call this function either after the StartMonitorShare is called or when you get the callback event of the onSharingStatus with Sharing_Self_Send_Begin. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for ZOOM style user interface mode.
	///It is not suggested to use this function for it will change the property of the window and leads to some unknown errors.
	///It won't work until the IMeetingShareController::StartMonitorShare() is called if the bChangeWindowStyle is set to FALSE. 
	///If you want to use the specified window during the share, you need to redraw the window.
	///Set the bBlock to FALSE before ending the share and call the function for the specified window to resume the property of the window.
	virtual SDKError BlockWindowFromScreenshare(bool bBlock, HWND hWnd, bool bChangeWindowStyle = true) = 0;
	
	/// \brief Switch to auto-adjust mode from sharing window by the function when watching the share on the specified view.
	/// \param type Specify the view you want to set, either primary or secondary. For more details, see \link SDKViewType \endlink enum.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for ZOOM style user interface mode.
	virtual SDKError SwitchToFitWindowModeWhenViewShare(SDKViewType type) = 0;

	/// \brief Switch the window size by the function when watching the share on the specified view.
	/// \param userid Specify the user ID that you want to switch zoom ratio.
	/// \param shareViewZoomRatio Specify the size you want to set. For more details, see \link ZMBShareViewZoomRatio \endlink enum.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SwitchZoomRatioWhenViewShare(unsigned int userid, SDKShareViewZoomRatio shareViewZoomRatio) = 0;

	/// \brief Enable follow presenter's pointer by the function when watching the share on the specified view.
	/// \param userid Specify the user ID that you want to follow his pointer. 
	/// \param bEnable TRUE indicates to enable. FALSE not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableFollowPresenterPointerWhenViewShare(unsigned int userid, bool bEnable) = 0;

	/// \brief Determine if the follow presenter's pointer can be enabled when watching the share on the specified view.
	/// \param userid Specify the user ID that you want to follow his pointer. 
	/// \param [out] bCan TRUE indicates that the pointer can be enabled. FALSE indicates that it can't.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise it fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanEnableFollowPresenterPointerWhenViewShare(unsigned int userid, bool& bCan) = 0;

	/// \brief View the share from the specified user.
	/// \param userid Specify the user ID that you want to view his share. 
	/// \param type Specify the view that you want to display the share, either primary or secondary. For more details, see \link SDKViewType \endlink enum.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for ZOOM style user interface mode.
	/// \deprecated This interface is marked as deprecated
	virtual SDKError ViewShare(unsigned int userid, SDKViewType type) = 0;

	/// \brief Start sharing with White board.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	///You need to draw your own annotation bar for custom mode when you get the onShareContentNotification with SHARE_TYPE_WB.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StartWhiteBoardShare() = 0;

	/// \brief Start sharing frame.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StartShareFrame() = 0;

	/// \brief Start sharing only the computer audio.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.	
	virtual SDKError StartSharePureComputerAudio() = 0;

	/// \brief Start sharing camera.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for user custom interface mode only.	
	virtual SDKError StartShareCamera(const zchar_t* deviceID, HWND hWnd) = 0;

	/// \brief Display the dialog of sharing configuration.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for ZOOM style user interface mode.
	virtual SDKError ShowShareOptionDialog() = 0;
#endif
	
	/// \brief Determine if the specified ADVANCE SHARE OPTION is supported. 
	/// \param option_ The ADVANCE SHARE OPTION to be determined. For more information, see \link AdvanceShareOption \endlink enum.
	/// \return If it is supported, the return value is SDKErr_Success.
	///Otherwise not. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError IsSupportAdvanceShareOption(AdvanceShareOption option_) = 0;
	
	/// \brief Stop the current sharing.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError StopShare() = 0;

	/// \brief host / co - host can use this function to lock current meeting share.
	/// \param isLock TRUE means to lock the meeting share, FALSE not.
    /// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError LockShare(bool isLock) = 0;

	/// \brief Pause the current sharing.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError PauseCurrentSharing() = 0;

	/// \brief Resume the current sharing.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError ResumeCurrentSharing() = 0;

	/// \brief Get the list of all the sharers in the current meeting.
	/// \return If the function succeeds, the return value is list of user ID.
	///If the function fails, the return value is NULL.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual IList<unsigned int >* GetViewableShareSourceList() = 0;

	/// \brief Get the sharing information from the specified sharer.
	/// \param userid Specify the user ID that you want to get his sharing information.
	/// \param [out] shareSource Store the viewable sharing information. For more details, see \link ViewableShareSource \endlink structure.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode. 
	/// For custom interface mode, this interface is only valid after subscribing the sharing content from the specified user by calling ICustomizedShareRender::SetUserID(unsigned int userid) successfully.
	virtual SDKError GetViewableShareSourceByUserID(unsigned int userid, ViewableShareSource& shareSource) = 0;

	/// \brief Determine if it is able to share. 
	/// \return Enable or disable to start sharing.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	/// \deprecated This interface is marked as deprecated, and is replaced by CanStartShare(CannotShareReasonType& reason).
	virtual bool CanStartShare() = 0;

	/// \brief Determine whether the current meeting can start sharing. 
	/// \param [out] reason The reason that no one can start sharing. See \link CannotShareReasonType \endlink enum.
	/// \return True indicates you can start sharing.
	virtual bool CanStartShare(CannotShareReasonType& reason) = 0;

	/// \brief Determine if it is able to share desktop in the current meeting.
	/// \return True indicates it is able to share desktop in the current meeting. False not.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual bool IsDesktopSharingEnabled() = 0;

	/// \brief Determine if the sharing is locked. 
	/// \param bLocked TRUE indicates that the sharing is locked. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError IsShareLocked(bool& bLocked) = 0;

	/// \brief Determine if the sound of the computer in the current sharing or before share is supported. 
	/// \param [out] bCurEnableOrNot The parameter is valid only when the return value is TRUE. And TRUE indicates to sharing the sound of the computer for the moment.
	/// \return If it is TRUE, the value of bCurEnableOrNot can be used to check whether the computer sound is supported or not when sharing. FALSE not.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual bool IsSupportEnableShareComputerSound(bool& bCurEnableOrNot) = 0;

	/// \brief Determine whether to optimize the video fluidity when sharing in full screen mode. 
	/// \param bCurEnableOrNot This parameter is valid only when the return value is TRUE. And TRUE indicates to optimize video for the moment. 
	/// \return If it is TRUE, the value of bCurEnableOrNot can be used to check whether to support optimize video fluidity or not. FALSE not.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual bool IsSupportEnableOptimizeForFullScreenVideoClip(bool& bCurEnableOrNot) = 0;

	/// \brief Set to enable or disable the audio before sharing.
	/// \param bEnable TRUE indicates to enable. FALSE not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError EnableShareComputerSound(bool bEnable) = 0;

	/// \brief Set to enable or disable the audio when sharing.
	/// \param bEnable TRUE indicates to enable. FALSE not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError EnableShareComputerSoundWhenSharing(bool bEnable) = 0;

	/// \brief Set the audio share mode before or during sharing. 
	/// \param mode The mode for audio share, see \link AudioShareMode \endlink enum.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError SetAudioShareMode(AudioShareMode mode) = 0;

	/// \brief Get the audio share mode. 
	/// \param mode The mode for audio share, see \link AudioShareMode \endlink enum.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError GetAudioShareMode(AudioShareMode& mode) = 0;

	/// \brief Set to enable the video optimization before sharing. 
	/// \param bEnable TRUE indicates to enable. FALSE not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError EnableOptimizeForFullScreenVideoClip(bool bEnable) = 0;

	/// \brief Set to enable the video optimization when sharing. 
	/// \param bEnable TRUE indicates to enable. FALSE not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for both ZOOM style and user custom interface mode.
	virtual SDKError EnableOptimizeForFullScreenVideoClipWhenSharing(bool bEnable) = 0;

	/// \brief Set the options for multi-participants share.
	/// \param [in] shareOption New options for sharing, see \link MultiShareOption \endlink enum.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetMultiShareSettingOptions(MultiShareOption shareOption) = 0;

	/// \brief Get the options for multi-participants share.
	/// \param [out] shareOption Options for sharing, see \link MultiShareOption \endlink enum.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError GetMultiShareSettingOptions(MultiShareOption& shareOption) = 0;

	/// \brief Determine whether can switch to next camera, when share camera. 
	/// \param [Out] bCan, if bCan is true it means you can switch, else can not.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanSwitchToShareNextCamera(bool& bCan) = 0;

	/// \brief switch to next camera, when you are sharing the camera.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SwitchToShareNextCamera() = 0;

	/// \brief Determine whether the user can share video files.
	/// \return True indicates that the user can share video files. Otherwise False.
	virtual bool CanShareVideoFile() = 0;

#if defined(WIN32)
	/// \brief Determine whether the user can share to the breakout room.
	/// \return True indicates that the user can share to the breakout room.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for user custom interface mode only.	
	virtual SDKError CanEnableShareToBO(bool& bCan) = 0;

	/// \brief Set to enable sharing to the breakout room. 
	/// \param bEnable TRUE indicates to enable. FALSE indicates that sharing to the breakout room is not enabled.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for user custom interface mode only.	
	virtual SDKError EnableShareToBO(bool bEnable) = 0;

	/// \brief Determine if sharing to the breakout room is enabled. 
	/// \param bEnabled TRUE indicates that the sharing is locked. 
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid for user custom interface mode only.	
	virtual SDKError IsShareToBOEnabled(bool& bEnabled) = 0;

	/// \brief Share the video file.
	/// \param filePath Specify the video file path. Only supports mov, mp4, or avi format.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails and returns error. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartVideoFileShare(const zchar_t* filePath) = 0;


	/// \brief Determine whether the legal notice for white board is available
	/// \return True indicates the legal notice for white board is available. Otherwise False.
	virtual bool IsWhiteboardLegalNoticeAvailable() = 0;

	/// Get the white board legal notices prompt.
	virtual const zchar_t* getWhiteboardLegalNoticesPrompt() = 0;

	/// Get the white board legal notices explained.
	virtual const zchar_t* getWhiteboardLegalNoticesExplained() = 0;
#endif
};
END_ZOOM_SDK_NAMESPACE
#endif