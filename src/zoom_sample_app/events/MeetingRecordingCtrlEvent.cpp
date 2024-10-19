#include "MeetingRecordingCtrlEvent.h"

void MeetingRecordingCtrlEvent::onRecordPrivilegeChanged(bool bCanRec) {
    if (m_onRecordingPrivilegeChanged)
        m_onRecordingPrivilegeChanged(bCanRec);
}
