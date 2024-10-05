/*!
* \file meeting_inmeeting_encryption_interface.h
* \brief Meeting Service Encryption Interface
* Valid for both ZOOM style and user custom interface mode.
*/

#ifndef _MEETING_INMEETING_ENCRYPTION_INTERFACE_H_
#define _MEETING_INMEETING_ENCRYPTION_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE

class IMeetingEncryptionControllerEvent
{
public:
	virtual ~IMeetingEncryptionControllerEvent() {}
	
	/// \brief This callback will be called when the security code changes
	virtual void onE2EEMeetingSecurityCodeChanged() = 0;
};

enum EncryptionType
{
	EncryptionType_None,      ///<For initialization.
	EncryptionType_Enhanced,  ///<Meeting encryption type is Enhanced.
	EncryptionType_E2EE       ///<Meeting encryption type is E2EE.
}; 

class IMeetingEncryptionController
{
public:
	virtual ~IMeetingEncryptionController() {}
	
	/// \brief Set the encryption controller callback handler.
	/// \param pEvent A pointer to the IMeetingEncryptionControllerEvent that receives the encryption event. 
	virtual void SetEvent(IMeetingEncryptionControllerEvent* pEvent) = 0;

	/// \brief Get meeting encryption type.
	/// \return The encryption type. For more details, see \link EncryptionType \endlink enum.
	virtual EncryptionType GetEncryptionType() = 0;

	/// \brief Get E2EE meeting security code.
	/// \return The security code.
	virtual const zchar_t* GetE2EEMeetingSecurityCode() = 0;

	/// \brief Get security code passed seconds.
	/// \return time in seconds.
	virtual unsigned int GetE2EEMeetingSecurityCodePassedSeconds() = 0;
	
	/// \brief Determine whether unencrypted exception data is valid.
	/// \return true if it's valid, otherwise return false.
	virtual bool IsUnencryptedExceptionDataValid() = 0;
	
	/// \brief Get unencrypted exception count.
	/// \return exception count.
	virtual unsigned int GetUnencryptedExceptionCount() = 0;
	
	/// \brief Get unencrypted exception details.
	/// \return exception details.
	virtual const zchar_t* GetUnencryptedExceptionInfo() = 0;
};

END_ZOOM_SDK_NAMESPACE
#endif