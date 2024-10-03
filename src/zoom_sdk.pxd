# zoom_sdk_wrapper.pxd
from libcpp cimport bool
from libc.stddef cimport wchar_t
  
cdef extern from "zoom_sdk_def.h" namespace "ZOOM_SDK_NAMESPACE":

    ctypedef char zchar_t

    cdef enum SDK_LANGUAGE_ID:
        LANGUAGE_Unknown
        LANGUAGE_English
        LANGUAGE_Chinese_Simplified
        LANGUAGE_Chinese_Traditional
        LANGUAGE_Japanese
        LANGUAGE_Spanish
        LANGUAGE_German
        LANGUAGE_French
        LANGUAGE_Portuguese
        LANGUAGE_Russian
        LANGUAGE_Korean
        LANGUAGE_Vietnamese
        LANGUAGE_Italian
        LANGUAGE_Polish
        LANGUAGE_Turkish
        LANGUAGE_Indonesian
        LANGUAGE_Dutch
        LANGUAGE_Swedish

    cdef enum SDKError:
        SDKERR_SUCCESS
        SDKERR_NO_IMPL
        SDKERR_WRONG_USAGE
        SDKERR_INVALID_PARAMETER
        SDKERR_MODULE_LOAD_FAILED
        SDKERR_MEMORY_FAILED
        SDKERR_SERVICE_FAILED
        SDKERR_UNINITIALIZE
        SDKERR_UNAUTHENTICATION
        SDKERR_NORECORDINGINPROCESS
        SDKERR_TRANSCODER_NOFOUND
        SDKERR_VIDEO_NOTREADY
        SDKERR_NO_PERMISSION
        SDKERR_UNKNOWN
        SDKERR_OTHER_SDK_INSTANCE_RUNNING
        SDKERR_INTERNAL_ERROR
        SDKERR_NO_AUDIODEVICE_ISFOUND
        SDKERR_NO_VIDEODEVICE_ISFOUND
        SDKERR_TOO_FREQUENT_CALL
        SDKERR_FAIL_ASSIGN_USER_PRIVILEGE
        SDKERR_MEETING_DONT_SUPPORT_FEATURE
        SDKERR_MEETING_NOT_SHARE_SENDER
        SDKERR_MEETING_YOU_HAVE_NO_SHARE
        SDKERR_MEETING_VIEWTYPE_PARAMETER_IS_WRONG
        SDKERR_MEETING_ANNOTATION_IS_OFF
        SDKERR_SETTING_OS_DONT_SUPPORT
        SDKERR_EMAIL_LOGIN_IS_DISABLED
        SDKERR_HARDWARE_NOT_MEET_FOR_VB
        SDKERR_NEED_USER_CONFIRM_RECORD_DISCLAIMER
        SDKERR_NO_SHARE_DATA
        SDKERR_SHARE_CANNOT_SUBSCRIBE_MYSELF
        SDKERR_NOT_IN_MEETING
        SDKERR_NOT_JOIN_AUDIO
        SDKERR_HARDWARE_DONT_SUPPORT
        SDKERR_DOMAIN_DONT_SUPPORT
        SDKERR_MEETING_REMOTE_CONTROL_IS_OFF
        SDKERR_FILETRANSFER_ERROR
    
    cdef enum ZoomSDKRawDataMemoryMode:
        ZoomSDKRawDataMemoryModeStack
        ZoomSDKRawDataMemoryModeHeap

    cdef struct tagRawDataOptions:
        bool enableRawdataIntermediateMode
        ZoomSDKRawDataMemoryMode videoRawdataMemoryMode
        ZoomSDKRawDataMemoryMode shareRawdataMemoryMode
        ZoomSDKRawDataMemoryMode audioRawdataMemoryMode
    ctypedef tagRawDataOptions RawDataOptions

    cdef struct tagInitParam:
        const zchar_t* strWebDomain
        const zchar_t* strBrandingName
        const zchar_t* strSupportUrl
        SDK_LANGUAGE_ID emLanguageID
        bool enableGenerateDump
        bool enableLogByDefault
        unsigned int uiLogFileSize
        RawDataOptions rawdataOpts
        int wrapperType
    ctypedef tagInitParam InitParam