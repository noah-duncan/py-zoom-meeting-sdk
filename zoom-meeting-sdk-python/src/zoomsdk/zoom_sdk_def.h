/*!
* \file zoom_sdk_def.h
* \brief ZOOM windows SDK Common Definition File.
* 
*/
#ifndef _ZOOM_SDK_DEF_H_
#define _ZOOM_SDK_DEF_H_
#if defined(WIN32)
#include <tchar.h>
typedef wchar_t zchar_t;
#define TLS_KEY_DEF uint32_t
#else
#include <stdint.h>
#include <unistd.h>
#include <cstring>
#include <float.h>
typedef char zchar_t;
typedef uint64_t UINT64;
typedef int64_t INT64;
typedef float  FLOAT;
typedef void* HWND;
#define TLS_KEY_DEF pthread_key_t
#endif//#if defined(WIN32)
#define PLATFORM_IMPORT	__declspec(dllimport)  
#define PLATFORM_EXPORT	__declspec(dllexport)
#ifdef ZOOM_SDK_DLL_EXPORT
#define SDK_API PLATFORM_EXPORT
#elif defined ZOOM_SDK_DLL_IMPORT
#define SDK_API PLATFORM_IMPORT
#else
#define SDK_API
#endif

#define ZOOM_SDK_NAMESPACE ZOOMSDK
#define BEGIN_ZOOM_SDK_NAMESPACE namespace ZOOM_SDK_NAMESPACE {
#define END_ZOOM_SDK_NAMESPACE };
#define USING_ZOOM_SDK_NAMESPACE using namespace ZOOM_SDK_NAMESPACE;

BEGIN_ZOOM_SDK_NAMESPACE
/*! \enum SDKError
    \brief SDK error types.
    Here are more detailed structural descriptions.
*/ 
enum SDKError
{
	SDKERR_SUCCESS = 0,///<Success.
	SDKERR_NO_IMPL,///<This feature is currently invalid. 
	SDKERR_WRONG_USAGE,///<Incorrect usage of the feature. 
	SDKERR_INVALID_PARAMETER,///<Wrong parameter.
	SDKERR_MODULE_LOAD_FAILED,///<Loading module failed.
	SDKERR_MEMORY_FAILED,///<No memory is allocated. 
	SDKERR_SERVICE_FAILED,///<Internal service error.
	SDKERR_UNINITIALIZE,///<Not initialized before the usage.
	SDKERR_UNAUTHENTICATION,///<Not authorized before the usage.
	SDKERR_NORECORDINGINPROCESS,///<No recording in process.
	SDKERR_TRANSCODER_NOFOUND,///<Transcoder module is not found.
	SDKERR_VIDEO_NOTREADY,///<The video service is not ready.
	SDKERR_NO_PERMISSION,///<No permission.
	SDKERR_UNKNOWN,///<Unknown error.
	SDKERR_OTHER_SDK_INSTANCE_RUNNING,///<The other instance of the SDK is in process.
	SDKERR_INTERNAL_ERROR,///<SDK internal error.
	SDKERR_NO_AUDIODEVICE_ISFOUND,///<No audio device found.
	SDKERR_NO_VIDEODEVICE_ISFOUND,///<No video device found.
	SDKERR_TOO_FREQUENT_CALL,///<API calls too frequently.
	SDKERR_FAIL_ASSIGN_USER_PRIVILEGE, ///<User can't be assigned with new privilege.
	SDKERR_MEETING_DONT_SUPPORT_FEATURE,///<The current meeting doesn't support the feature.
	SDKERR_MEETING_NOT_SHARE_SENDER,///<The current user is not the presenter.
	SDKERR_MEETING_YOU_HAVE_NO_SHARE,///<There is no sharing.
	SDKERR_MEETING_VIEWTYPE_PARAMETER_IS_WRONG, ///<Incorrect ViewType parameters.
	SDKERR_MEETING_ANNOTATION_IS_OFF, ///<Annotation is disabled.
	SDKERR_SETTING_OS_DONT_SUPPORT, ///<Current OS doesn't support the setting.
	SDKERR_EMAIL_LOGIN_IS_DISABLED, ///<Email login is disable
	SDKERR_HARDWARE_NOT_MEET_FOR_VB, ///<Computer doesn't meet the minimum requirements to use virtual background feature.
	SDKERR_NEED_USER_CONFIRM_RECORD_DISCLAIMER,  ///<Need process disclaimer.
	SDKERR_NO_SHARE_DATA,///<There is no raw data of sharing.
	SDKERR_SHARE_CANNOT_SUBSCRIBE_MYSELF,
	SDKERR_NOT_IN_MEETING,
	SDKERR_NOT_JOIN_AUDIO,
	SDKERR_HARDWARE_DONT_SUPPORT, ///<The current device doesn't support the feature.
	SDKERR_DOMAIN_DONT_SUPPORT,
	SDKERR_MEETING_REMOTE_CONTROL_IS_OFF, ///<Remote control is disabled.
	SDKERR_FILETRANSFER_ERROR,

};

/*! \enum SDK_LANGUAGE_ID
    \brief The text resource type used by the SDK.
    Here are more detailed structural descriptions.
*/
enum SDK_LANGUAGE_ID
{
	LANGUAGE_Unknown = 0,///<For initialization.
	LANGUAGE_English,///<In English.
	LANGUAGE_Chinese_Simplified,///<In simplified Chinese.
	LANGUAGE_Chinese_Traditional,///<In traditional Chinese.
	LANGUAGE_Japanese,///<In Japanese.
	LANGUAGE_Spanish,///<In Spanish.
	LANGUAGE_German,///<In German.
	LANGUAGE_French,///<In French.
	LANGUAGE_Portuguese,///<In Portuguese.
	LANGUAGE_Russian,///<In Russian.
	LANGUAGE_Korean,///<In Korean.
	LANGUAGE_Vietnamese,///<In Vietnamese.
	LANGUAGE_Italian,///<In Italian.
	LANGUAGE_Polish,///<In Polish.
	LANGUAGE_Turkish,///<In Turkish.
	LANGUAGE_Indonesian,//<In Indonesian.
	LANGUAGE_Dutch,//<In Dutch.
	LANGUAGE_Swedish///<In Swedish.
};

enum ZoomSDKRawDataMemoryMode 
{
	ZoomSDKRawDataMemoryModeStack,
	ZoomSDKRawDataMemoryModeHeap
};

#if (defined WIN32 )
/*! \struct tagWndPosition
    \brief The position of the window. The coordinate of position is that of monitor when the parent window is null. If the the parent window is not null, the position coordinate is that of the parent window.
    Here are more detailed structural descriptions.
*/
typedef struct tagWndPosition 
{
	int left;///<Specifies the X-axis coordinate of the top-left corner of the window
	int top;///<Specifies the Y-axis coordinate of the top-left of the window.
	HWND hSelfWnd;///<Specifies the window handle of the window itself.
	HWND hParent;///<Specifies the window handle of the parent window. If the value is NULL, the position coordinate is the monitor coordinate.
	tagWndPosition()
	{
		left = 0;
		top = 0;
		hSelfWnd = NULL;
		hParent = NULL;
	}
}WndPosition;

/*! \enum CustomizedLanguageType
    \brief Custom resource type used by the SDK.
    Here are more detailed structural descriptions.
*/
enum CustomizedLanguageType
{
	CustomizedLanguage_None,///<No use of the custom resource.
	CustomizedLanguage_FilePath,///<Use the specified file path to assign the custom resource.
	CustomizedLanguage_Content,///<Use the specified content to assign the custom resource.
};

/*! \struct CustomizedLanguageType
    \brief The custom resource information used by the SDK.
    Here are more detailed structural descriptions.
*/ 
typedef struct tagCustomizedLanguageInfo
{
	const char* langName;///<Resource name.
	const char* langInfo;///<The value should be the full path of the resource file when the langType value is CustomizedLanguage_FilePath, including the file name. When the langType value is CustomizedLanguage_Content, the value saves the content of the resource.
	CustomizedLanguageType langType;///<Use the custom resource type.
	tagCustomizedLanguageInfo()
	{
		langName = NULL;
		langInfo = NULL;
		langType = CustomizedLanguage_None;
	}

}CustomizedLanguageInfo;

/*! \struct tagConfigurableOptions
    \brief SDK configuration options. 
	\remarks This structure is used only for the SDK initialization to configure the custom resource file and choose whether to use the UI mode.
    Here are more detailed structural descriptions.
*/
#define ENABLE_CUSTOMIZED_UI_FLAG (1 << 5)
typedef struct tagConfigurableOptions
{
	CustomizedLanguageInfo customizedLang;///The custom resource information.
	int optionalFeatures;///<Additional functional configuration. The function currently supports whether to use the custom UI mode only. When the value of the optionalFeatures&ENABLE_CUSTOMIZED_UI_FLAG is TRUE, it means the custom UI mode will be used. Otherwise the Zoom UI mode will be used.
	const zchar_t* sdkPathPostfix;
	tagConfigurableOptions()
	{
		optionalFeatures = 0;
		sdkPathPostfix = NULL;
	}

}ConfigurableOptions;

/*! \enum SDK_APP_Locale
    \brief SDK_APP locale type.
    Here are more detailed structural descriptions.
*/
enum SDK_APP_Locale
{
	SDK_APP_Locale_Default,
	SDK_APP_Locale_CN,
};

enum ZoomSDKVideoRenderMode
{
	ZoomSDKVideoRenderMode_None = 0,
	ZoomSDKVideoRenderMode_Auto,
	ZoomSDKVideoRenderMode_D3D11EnableFLIP,
	ZoomSDKVideoRenderMode_D3D11,
	ZoomSDKVideoRenderMode_D3D9,
	ZoomSDKVideoRenderMode_GDI,
};

enum ZoomSDKRenderPostProcessing
{
	ZoomSDKRenderPostProcessing_None = 0,
	ZoomSDKRenderPostProcessing_Auto,
	ZoomSDKRenderPostProcessing_Enable,
	ZoomSDKRenderPostProcessing_Disable,
};

enum ZoomSDKVideoCaptureMethod
{
	ZoomSDKVideoCaptureMethod_None = 0,
	ZoomSDKVideoCaptureMethod_Auto,
	ZoomSDKVideoCaptureMethod_DirectSHow,
	ZoomSDKVideoCaptureMethod_MediaFoundation,
};

typedef struct tagZoomSDKRenderOptions
{
	ZoomSDKVideoRenderMode    videoRenderMode;
	ZoomSDKRenderPostProcessing renderPostProcessing;
	ZoomSDKVideoCaptureMethod videoCaptureMethod;
	tagZoomSDKRenderOptions()
	{
		videoRenderMode = ZoomSDKVideoRenderMode_None;
		renderPostProcessing = ZoomSDKRenderPostProcessing_Auto;
		videoCaptureMethod = ZoomSDKVideoCaptureMethod_Auto;
	}
}ZoomSDKRenderOptions;
#endif
typedef struct tagRawDataOptions
{
	bool enableRawdataIntermediateMode; ///<false -- YUV420data, true -- intermediate data
	ZoomSDKRawDataMemoryMode  videoRawdataMemoryMode;
	ZoomSDKRawDataMemoryMode  shareRawdataMemoryMode;
	ZoomSDKRawDataMemoryMode  audioRawdataMemoryMode;
	tagRawDataOptions()
	{
		enableRawdataIntermediateMode = false;
		videoRawdataMemoryMode = ZoomSDKRawDataMemoryModeStack;
		shareRawdataMemoryMode = ZoomSDKRawDataMemoryModeStack;
		audioRawdataMemoryMode = ZoomSDKRawDataMemoryModeStack;
	}
}RawDataOptions;

/*! \struct tagInitParam
    \brief Initialize the SDK Parameter.
    Here are more detailed structural descriptions.
*/
typedef struct tagInitParam  
{
	const zchar_t* strWebDomain;///<Web domain.
	const zchar_t* strBrandingName;///<Branding name.
	const zchar_t* strSupportUrl;///<Support URL.
	SDK_LANGUAGE_ID emLanguageID;///<The ID of the SDK language.
	bool enableGenerateDump; ///<Enable generate dump file if the app crashed.
	bool enableLogByDefault;///<Enable log feature.
	unsigned int uiLogFileSize; ///<Size of a log file in M(megabyte). The default size is 5M. There are 5 log files in total and the file size varies from 1M to 50M. 
	RawDataOptions rawdataOpts;
	int wrapperType;
	tagInitParam()
	{
		strWebDomain = NULL;
		strBrandingName = NULL;
		strSupportUrl = NULL;
		emLanguageID = LANGUAGE_Unknown;
		enableGenerateDump = false;
		enableLogByDefault = false;
		uiLogFileSize = 5;
		wrapperType = 0;
	}
}InitParam;

/*! \enum ShareType
	Type of current sharing received by the user.
	Here are more detailed structural descriptions..
*/
enum ShareType
{
	SHARE_TYPE_UNKNOWN,///<Type unknown.
	SHARE_TYPE_AS,///<Type of sharing the application.
	SHARE_TYPE_DS,///<Type of sharing the desktop.
	SHARE_TYPE_WB,///<Type of sharing the white-board.
	SHARE_TYPE_AIRHOST,///<Type of sharing data from the device connected WIFI. 
	SHARE_TYPE_CAMERA,///<Type of sharing the camera.
	SHARE_TYPE_DATA,///<Type of sharing the data.
	SHARE_TYPE_VIDEO_FILE,///<Type of sharing the video file.
};

/*! \enum LastErrorType
    \brief The last error types of the SDK.
    Here are more detailed structural descriptions.
*/
enum LastErrorType
{
	LastErrorType_None,///<No error.
	LastErrorType_Auth,///<Error during verification.
	LastErrorType_Login,///<Error during login.
	LastErrorType_Meeting,///<The associated error with the meeting.
	LastErrorType_System,///<The associated error with the SDK bottom layer.
};

/// \brief Error mechanism interface provided by the SDK
///This feature is gradually improved, so some errors may not be supported.
class IZoomLastError
{
public:
	/// \brief Get the last error type.
	/// \return If the function succeeds, the error type will be returned. For more details, see \link LastErrorType \endlink enum.
	virtual LastErrorType GetErrorType() const = 0 ;
	/// \brief Get the last error code.
	/// \return If the function succeeds, the error code will be returned.
	virtual UINT64 GetErrorCode() const = 0;

	/// \brief Get the description for the last error.
	/// \return If the function succeeds, the error description will be returned. If there is no error, it will return an empty string of length zero(0).
	virtual const zchar_t* GetErrorDescription() const = 0;
	virtual ~IZoomLastError(){};
};
template<class T>
class IList
{
public:
	virtual ~IList(){};
	virtual int GetCount() = 0;
	virtual T   GetItem(int index) = 0;
};
#if (defined WIN32)
#define SDK_NULL_AUDIO_FILE_HANDLE (0xffffffff)
const RECT _SDK_TEST_VIDEO_INIT_RECT = {0,0,0,0};
#endif
enum FrameDataFormat
{
	FrameDataFormat_I420_LIMITED,
	FrameDataFormat_I420_FULL,
};

enum ZoomSDKAudioChannel
{
	ZoomSDKAudioChannel_Mono,
	ZoomSDKAudioChannel_Stereo,
};

enum CannotShareReasonType
{
	CannotShareReasonType_None,
	CannotShareReasonType_Locked,		                   ///<Only the host can share.
	CannotShareReasonType_Disabled,                        ///<Sharing is disabled.
	CannotShareReasonType_Other_Screen_Sharing,		       ///<Another is sharing their screen.
	CannotShareReasonType_Other_WB_Sharing,                ///<Another is sharing their whiteboard.
	CannotShareReasonType_Need_Grab_Myself_Screen_Sharing, ///<The user is sharing their screen, and can grab. To grab, call EnableGrabShareWithoutReminder(true) before starting share.
	CannotShareReasonType_Need_Grab_Other_Screen_Sharing,  ///<Another is sharing their screen, and can grab. To grab, call EnableGrabShareWithoutReminder(true) before starting share.
	CannotShareReasonType_Need_Grab_Audio_Sharing,         ///<Another is sharing pure computer audio, and can grab. To grab, call EnableGrabShareWithoutReminder(true) before starting share.
	CannotShareReasonType_Need_Grap_WB_Sharing,            ///<Other or myself is sharing whiteboard, and can Grab. To grab, call EnableGrabShareWithoutReminder(true) before starting share.
	CannotShareReasonType_Reach_Maximum,                   ///<The meeting has reached the maximum allowed screen share sessions.
	CannotShareReasonType_Have_Share_From_Mainsession,     ///<Other share screen in main session.
	CannotShareReasonType_UnKnown,
};

/*! \enum SharingStatus
	\brief Sharing status.
	Here are more detailed structural descriptions..
*/
enum SharingStatus
{
	Sharing_Self_Send_Begin,///<Begin to share by the user himself.
	Sharing_Self_Send_End,///<Stop sharing by the user.
	Sharing_Self_Send_Pure_Audio_Begin,///<Begin to share pure audio by the user himself.
	Sharing_Self_Send_Pure_Audio_End,///<Stop sharing pure audio by the user.
	Sharing_Other_Share_Begin,///<Others begin to share.
	Sharing_Other_Share_End,///<Others stop sharing.
	Sharing_Other_Share_Pure_Audio_Begin,///<Others begin to share pure audio.
	Sharing_Other_Share_Pure_Audio_End,///<Others stop sharing pure audio.
	Sharing_View_Other_Sharing,///<View the sharing of others.
	Sharing_Pause,///<Pause sharing.
	Sharing_Resume,///<Resume sharing.
};
END_ZOOM_SDK_NAMESPACE

#endif