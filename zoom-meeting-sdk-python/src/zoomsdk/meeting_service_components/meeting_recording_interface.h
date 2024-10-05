/*!
* \file meeting_recording_interface.h
* \brief Recording of Meeting Service Interface
* Valid for both ZOOM style and user custom interface mode.
*/
#ifndef _MEETING_Recording_INTERFACE_H_
#define _MEETING_Recording_INTERFACE_H_
#include "zoom_sdk_def.h"
#include <time.h>
BEGIN_ZOOM_SDK_NAMESPACE
/*! \enum RecordingStatus
    \brief Recording status.
    Here are more detailed structural descriptions.
*/
enum RecordingStatus
{
	Recording_Start,///Start recording on local computer or on cloud.
	Recording_Stop,///Stop recording on local computer or on cloud.
	Recording_DiskFull,///There is no more space to store both local and cloud recording.
	Recording_Pause,///Pause recording on local or on cloud.
	Recording_Connecting,///Connecting, only for cloud recording.
	Recording_Fail,///Saving the recording failed.
};

#if defined(__linux__)
enum TranscodingStatus
{
	Transcoding_Start,
	Transcoding_Inprogress,
	Transcoding_End,
	Transcoding_Error_LowDiskSpace,
	Transcoding_Error_UnknownSources,
	Transcoding_Error_WrongFile,
	Transcoding_Error_Unknown,
};

typedef enum 
{
	REC_TYPE_VIDEO = 0,
	REC_TYPE_SHARE,
}LocalRecordingSubscribeType;

typedef enum LocalRecordingResolution
{
	LocalRecordingResolution_90P = 0,
	LocalRecordingResolution_180P,
	LocalRecordingResolution_360P,
	LocalRecordingResolution_720P,
	LocalRecordingResolution_1080P,
	LocalRecordingResolution_NoUse = 100
}LocalRecordingResolution;
#endif

/*! \enum RequestLocalRecordingStatus
	\brief Request local recording privilege status.
	Here are more detailed structural descriptions.
*/
enum RequestLocalRecordingStatus
{
	RequestLocalRecording_Granted,///host grant the request.
	RequestLocalRecording_Denied,///host deny the request.
	RequestLocalRecording_Timeout,///the request local recording timeout.	
};

/*! \enum RequestStartCloudRecordingStatus
	\brief Request host to start cloud recording status.
*/
enum RequestStartCloudRecordingStatus
{
	RequestStartCloudRecording_Granted,///host grants the request.
	RequestStartCloudRecording_Denied,///host denies the request.
	RequestStartCloudRecording_TimedOut,///the request for cloud recording timed out.	
};

enum LocalRecordingRequestPrivilegeStatus
{
	LocalRecordingRequestPrivilege_None,
	LocalRecordingRequestPrivilege_AllowRequest,///allow participant to send privilege requests.
	LocalRecordingRequestPrivilege_AutoGrant,///host auto-allow all privilege requests.
	LocalRecordingRequestPrivilege_AutoDeny,///host auto-deny all privilege requests.
};

/// \brief Process after the host receives the requirement from the user to give the local recording privilege.
class IRequestLocalRecordingPrivilegeHandler
{
public:
	virtual ~IRequestLocalRecordingPrivilegeHandler() {};
	/// \brief Get the request ID.
	/// \return If the function succeeds, the return value is the request ID.
	virtual const zchar_t* GetRequestId() = 0;

	/// \brief Get the user ID who requested privilege.
	/// \return If the function succeeds, the return value is the user ID. Otherwise, this returns 0.
	virtual unsigned int GetRequesterId() = 0;

	/// \brief Get the user name who requested privileges.
	/// \return If the function succeeds, the return value is the user name.
	virtual const zchar_t* GetRequesterName() = 0;

	/// \brief Allows the user to start local recording and finally self-destroy.
	virtual SDKError GrantLocalRecordingPrivilege() = 0;

	/// \brief Denies the user permission to start local recording and finally self-destroy.
	virtual SDKError DenyLocalRecordingPrivilege() = 0;
};

/// \brief Object to handle a user's request to start cloud recording.
/// \remarks If current user can control web setting for smart recording, they will get IRequestEnableAndStartSmartRecordingHandler or ISmartRecordingEnableActionHandler when attendee request to start cloud recording or start cloud recording by self.
class IRequestStartCloudRecordingHandler
{
public:
	virtual ~IRequestStartCloudRecordingHandler() {};
	/// \brief Get the user ID who requested that the host start cloud recording.
	/// \return If the function succeeds, the return value is the user ID. Otherwise, this returns 0.
	virtual unsigned int GetRequesterId() = 0;

	/// \brief Get the user name who requested that the host start cloud recording.
	/// \return If the function succeeds, the return value is the user name.
	virtual const zchar_t* GetRequesterName() = 0;

	/// \brief Accept the request to start cloud recording and then destroys the IRequestCloudRecordingHandler instance.
	virtual SDKError Start() = 0;

	/// \brief Deny the request to start cloud recording and then destroys the IRequestCloudRecordingHandler instance.
	/// \param bDenyAll TRUE indicates to deny all requests. Participants can't send requests again until the host change the setting.
	virtual SDKError Deny(bool bDenyAll) = 0;
};

/// \brief Enable and start smart cloud recording request handler
class IRequestEnableAndStartSmartRecordingHandler
{
public:
	virtual ~IRequestEnableAndStartSmartRecordingHandler() {};
	/// \brief Get the user ID who requests to enable and start smart cloud recording.
	/// \return If the function succeeds, the return value is the user ID.
	virtual unsigned int GetRequestUserId() = 0;

	/// \brief Get the legal tip that you should agree to handle the user request.
	/// \return If the function succeeds, the return value is the legal notice about enabling and starting smart cloud recording.
	virtual const zchar_t* GetTipString() = 0;

	/// \brief Start normal cloud recording without enabling smart recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartCloudRecordingWithoutEnableSmartRecording() = 0;

	/// \brief Agree to the legal notice to enable and start smart cloud recording.
	/// \param bAllMeetings True indicates to enable smart recording for all future meetings including the current meeting. False indicates to only enable smart recording for the current meeting.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AgreeToEnableAndStart(bool bAllMeetings) = 0;

	/// \brief Decline the request to start cloud recording.
	/// \param bDenyAll True indicates to deny all attendees' requests for the host to start cloud recording. Participants can't send these types of requests again until the host change the setting.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError Decline(bool bDenyAll) = 0;
};

/// \brief Enable and start smart recording.
class ISmartRecordingEnableActionHandler
{
public:
	virtual ~ISmartRecordingEnableActionHandler() {};

	/// \brief Get the legal tip to enable smart recording.
	/// \return The legal notice.
	virtual const zchar_t* GetTipString() = 0;

	/// \brief Confirm enabling and starting the smart recording.
	/// \param bAllMeetings True indicates to enable smart recording for all future meetings including the current meeting. False indicates to only enable smart recording for the current meeting.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ActionConfirm(bool bAllMeetings) = 0;

	/// \brief Cancel enabling and starting the smart recording
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ActionCancel() = 0;
};

#if defined(WIN32)
class ICustomizedLocalRecordingLayoutHelper;
#endif
/// \brief Meeting recording callback event.
///
class IMeetingRecordingCtrlEvent
{
public:
	virtual ~IMeetingRecordingCtrlEvent() {}

	/// \brief Callback event that the status of my local recording changes.
	/// \param status Value of recording status. For more details, see \link RecordingStatus \endlink enum.
	virtual void onRecordingStatus(RecordingStatus status) = 0;

	/// \brief Callback event that the status of cloud recording changes.
	/// \param status Value of recording status. For more details, see \link RecordingStatus \endlink enum.
	virtual void onCloudRecordingStatus(RecordingStatus status) = 0;

	/// \brief Callback event that the recording authority changes.
	/// \param bCanRec TRUE indicates to enable to record.
	virtual void onRecordPrivilegeChanged(bool bCanRec) = 0;

	/// \brief Callback event that the status of request local recording privilege.
	/// \param status Value of request local recording privilege status. For more details, see \link RequestLocalRecordingStatus \endlink enum.
	virtual void onLocalRecordingPrivilegeRequestStatus(RequestLocalRecordingStatus status) = 0;

	/// \brief Callback event for when the host responds to a cloud recording permission request
	/// \param status Value of request host to start cloud recording response status. For more details, see \link RequestStartCloudRecordingStatus \endlink enum.
	virtual void onRequestCloudRecordingResponse(RequestStartCloudRecordingStatus status) = 0;

	/// \brief Callback event when a user requests local recording privilege.
	/// \param handler A pointer to the IRequestLocalRecordingPrivilegeHandler. For more details, see \link IRequestLocalRecordingPrivilegeHandler \endlink.
	virtual void onLocalRecordingPrivilegeRequested(IRequestLocalRecordingPrivilegeHandler* handler) = 0;
	
	/// \brief Callback event received only by the host when a user requests to start cloud recording.
	/// \param handler A pointer to the IRequestStartCloudRecordingHandler. For more details, see \link IRequestStartCloudRecordingHandler \endlink.
	virtual void onStartCloudRecordingRequested(IRequestStartCloudRecordingHandler* handler) = 0;

#if defined(WIN32)
	/// \brief Callback event of ending the conversion to MP4 format.
	/// \param bsuccess TRUE indicates to convert successfully. FALSE not.
	/// \param iResult This value is used to save the error code only when the convert fails.
	/// \param szPath If the conversion is successful, this value is used to save the path of the recording file. 
	/// \remarks In order to trigger this callback correctly, you need call IMeetingConfiguration.EnableLocalRecordingConvertProgressBarDialog(false) before you start a meeting.
	virtual void onRecording2MP4Done(bool bsuccess, int iResult, const zchar_t* szPath) = 0;

	/// \brief Callback event of the process of the conversion to MP4 format.
	/// \param iPercentage Percentage of conversion process. Range from ZERO(0) to ONE HUNDREAD(100).
	/// \remarks In order to trigger this callback correctly, you need call IMeetingConfiguration.EnableLocalRecordingConvertProgressBarDialog(false) before you start a meeting.
	virtual void onRecording2MP4Processing(int iPercentage) = 0;

	/// \brief Callback event that the local recording source changes in the custom user interface mode.
	/// \param layout_helper An object pointer to ICustomizedLocalRecordingLayoutHelper. For more details, see \link ICustomizedLocalRecordingLayoutHelper \endlink.
	///The layout_helper won't be released till the call ends. The user needs to complete the related layout before the call ends. 
	virtual void onCustomizedLocalRecordingSourceNotification(ICustomizedLocalRecordingLayoutHelper* layout_helper) = 0;
#endif
	
	 /// \brief Callback event that the cloud recording storage is full.
	 /// \param gracePeriodDate a point in time, in milliseconds, in UTC. You can use the cloud recording storage until the gracePeriodDate.
	virtual void onCloudRecordingStorageFull(time_t gracePeriodDate) = 0;

	/// \brief Callback event received only by the host when a user requests to enable and start smart cloud recording.
	/// \param handler A pointer to the IRequestEnableAndStartSmartRecordingHandler. For more details, see \link IRequestEnableAndStartSmartRecordingHandler \endlink.
	virtual void onEnableAndStartSmartRecordingRequested(IRequestEnableAndStartSmartRecordingHandler* handler) = 0;

	/// \brief Callback event received when you call \link EnableSmartRecording \endlink. You can use the handler to confirm or cancel enabling the smart recording.
	/// \param handler A pointer to the ISmartRecordingEnableActionHandler. For more details, see \link ISmartRecordingEnableActionHandler \endlink.
	virtual void onSmartRecordingEnableActionCallback(ISmartRecordingEnableActionHandler* handler) = 0;
#if defined(__linux__)
	virtual void onTranscodingStatusChanged(TranscodingStatus status,const zchar_t* path) = 0;
#endif
};

/// \brief Meeting recording controller interface.
///
class IMeetingRecordingController
{
public:
	/// \brief Set meeting recording callback event handler.
	/// \param pEvent A pointer to the IMeetingRecordingCtrlEvent that receives the recording event. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetEvent(IMeetingRecordingCtrlEvent* pEvent) = 0;

	/// \brief Determine if the user owns the authority to enable the local recording. 	
	/// \return If the host is enabled to handle local recording request, the return value is SDKErr_Success. 
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError IsSupportRequestLocalRecordingPrivilege() = 0;

	/// \brief Send a request to enable the SDK to start local recording.
	/// \return If the function succeeds, the return value is SDKErr_Success and the SDK will send the request.
	///Otherwise it fails and the request will not be sent. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestLocalRecordingPrivilege() = 0;

	/// \brief Send a request to ask the host to start cloud recording.
	/// \return If the function succeeds, the return value is SDKErr_Success and the SDK sends the request.
	///Otherwise it fails and the request is not sent. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError RequestStartCloudRecording() = 0;

	/// \brief Start recording.
	/// \param [out] startTimestamp The timestamps when start recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartRecording(time_t& startTimestamp) = 0;

	/// \brief Stop recording.
	/// \param [out] stopTimestamp The timestamps when stop recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopRecording(time_t& stopTimestamp) = 0;

	/// \brief Determine if the specified user is enabled to start recording.
	/// \param cloud_recording TRUE indicates to determine whether to enable the cloud recording. FALSE local recording. 
	/// \param userid Specifies the user ID.
	/// \return If the value of cloud_recording is set to TRUE and the cloud recording is enabled, the return value is SDKErr_Success.
	///If the value of cloud_recording is set to FALSE and the local recording is enabled, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanStartRecording(bool cloud_recording, unsigned int userid) = 0;

	/// \brief Determine if the smart recording feature is enabled in the meeting.
	/// \return true means that the feature enabled, false means that the feature isn't enabled.
	virtual bool IsSmartRecordingEnabled() = 0;

	/// \brief Whether the current user can enable the smart recording feature.
	/// \return true means the current user can enable the smart recording feature.
	virtual bool CanEnableSmartRecordingFeature() = 0;

	/// \brief Enable the smart recording feature.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise the function fails. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError EnableSmartRecording() = 0;

	/// \brief Determine if the current user own the authority to change the recording permission of the others.
	/// \return If the user own the authority, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanAllowDisAllowLocalRecording() = 0;

	/// \brief Start cloud recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartCloudRecording() = 0;

	/// \brief Stop cloud recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopCloudRecording() = 0;

	/// \brief Determine if the user owns the authority to enable the local recording. 
	/// \param userid Specifies the user ID.
	/// \return If the specified user is enabled to start local recording, the return value is SDKErr_Success. 
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError IsSupportLocalRecording(unsigned int userid) = 0;

	/// \brief Give the specified user authority for local recording.
	/// \param userid Specifies the user ID.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError AllowLocalRecording(unsigned int userid) = 0;

	/// \brief Abrogate the authority of the specified user for local recoding.
	/// \param userid Specifies the user ID.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError DisAllowLocalRecording(unsigned int userid) = 0;
#if defined(WIN32)
	/// \brief Send a request to enable the SDK to call IMeetingRecordingCtrlEvent::onCustomizedLocalRecordingSourceNotification().
	/// \return If the function succeeds, the return value is SDKErr_Success, and you will receive the onCustomizedLocalRecordingSourcenNotification callback event.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks Valid only for custom style user interface mode only when recording.
	virtual SDKError RequestCustomizedLocalRecordingSource() = 0;
#endif
	/// \brief Pause recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError PauseRecording() = 0;

	/// \brief Resume recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ResumeRecording() = 0;

	/// \brief Pause cloud recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError PauseCloudRecording() = 0;

	/// \brief Resume cloud recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError ResumeCloudRecording() = 0;


	/// \brief Determine if the specified user is enabled to start raw recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError CanStartRawRecording() = 0;

	/// \brief Start rawdata recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartRawRecording() = 0;

	/// \brief Stop rawdata recording.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopRawRecording() = 0;

	/// \brief Get current cloud recording.
	/// \return If the function succeeds, the return value is recording status.
	///To get extended error information, see \link RecordingStatus \endlink enum.
	virtual RecordingStatus GetCloudRecordingStatus() = 0;
#if defined(__linux__)
	virtual SDKError SubscribeLocalrecordingResource(unsigned int sourceId, LocalRecordingSubscribeType type,LocalRecordingResolution resolution) = 0;
	virtual SDKError UnSubscribeLocalrecordingResource(unsigned int sourceId, LocalRecordingSubscribeType type) = 0;
#endif
};
END_ZOOM_SDK_NAMESPACE
#endif