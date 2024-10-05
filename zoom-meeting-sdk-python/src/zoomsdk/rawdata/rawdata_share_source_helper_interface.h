#ifndef _RAWDATA_SHARE_SOURCE_HELPER_INTERFACE_H_
#define _RAWDATA_SHARE_SOURCE_HELPER_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

class IZoomSDKShareSender
{
public:
	virtual ~IZoomSDKShareSender() {}

	/// \brief Send a frame of YUV data.
	/// \param frameBuffer YUV data buffer.
	/// \param width Frame width.
	/// \param height Frame height.
	/// \param frameLength Buffer length.
	/// \param format YUV type.
	/// \return If the function succeeds, the return value is SDKERR_SUCCESS.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError sendShareFrame(char* frameBuffer, int width, int height, int frameLength, FrameDataFormat format = FrameDataFormat_I420_FULL) = 0;
};

class IZoomSDKShareSource
{
public:
	virtual ~IZoomSDKShareSource() {}

	/// \brief Callback for share source can start send raw data.
	/// \param pSender See \link IZoomSDKShareSender \endlink.
	virtual void onStartSend(IZoomSDKShareSender* pSender) = 0;

	/// \brief Callback for share source to stop send raw data.
	virtual void onStopSend() = 0;
};

class IZoomSDKShareAudioSender
{
public:
	virtual ~IZoomSDKShareAudioSender() {}

	/// \brief Send audio raw data.
	/// \param data The audio data¡¯s address.
	/// \param data_length The audio data¡¯s length, in even numbers.
	/// \param sample_rate The audio data¡¯s sampling rate.
	/// \param channel The channel type.
	/// Supported audio data properties:
	/// Channels: mono and stereo
	/// When the channel is mono, supported sample rates: 8000/11025/16000/32000/44100/48000/50000/50400/96000/192000
	/// When the channel is stereo, supported sample rates: 8000/16000/32000/44100/48000/50000/50400/96000
	/// resolution: little-endian, 16bit
	virtual SDKError sendShareAudio(char* data, unsigned int data_length, int sample_rate, ZoomSDKAudioChannel channel) = 0;
};

class IZoomSDKShareAudioSource
{
public:
	virtual ~IZoomSDKShareAudioSource() {}

	/// \brief Callback for audio source to start sending raw data.
	/// \param pShareAudioSender See \link IZoomSDKShareAudioSender \endlink.
	virtual void onStartSendAudio(IZoomSDKShareAudioSender* pShareAudioSender) = 0;

	/// \brief Callback for audio source to stop sending raw data.
	virtual void onStopSendAudio() = 0;
};

class IZoomSDKShareSourceHelper
{
public:
	virtual ~IZoomSDKShareSourceHelper() {}
	
	/// \brief Start sharing external source.
	/// \param pShareSource the external source object pointer. 
	/// \param pShareAudioSource the external audio source object pointer. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails and returns an error code. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks If pShareAudioSource is non-null, it indicates sharing user-defined audio at the same time.
	virtual SDKError setExternalShareSource(IZoomSDKShareSource* pShareSource, IZoomSDKShareAudioSource* pShareAudioSource = NULL) = 0;
	
	/// \brief Start sharing pure external audio source.
	/// \param pShareAudioSource the external audio source object pointer. 
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise the function fails and returns an error code. To get extended error information, see \link SDKError \endlink enum.
	/// \remarks The feature is similar to sharing computer audio, except that the shared content is user-defined.	
	virtual SDKError setSharePureAudioSource(IZoomSDKShareAudioSource* pShareAudioSource) = 0;
};

END_ZOOM_SDK_NAMESPACE
#endif