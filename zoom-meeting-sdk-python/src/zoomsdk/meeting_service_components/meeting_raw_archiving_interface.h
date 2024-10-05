/*!
* \file meeting_raw_archiving_interface.h
* \brief Meeting Raw Archiving Interface. 
*/
#ifndef _MEETING_RAW_ARCHIVING_INTERFACE_H_
#define _MEETING_RAW_ARCHIVING_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

/// \brief Meeting raw archiving controller interface.
///
class IMeetingRawArchivingController
{
public:
	/// \brief start raw archiving,call this method can get rawdata receive privilege.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StartRawArchiving() = 0;

	/// \brief stop raw archiving, call this method reclaim rawdata receive privilege.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError StopRawArchiving() = 0;
};

END_ZOOM_SDK_NAMESPACE
#endif