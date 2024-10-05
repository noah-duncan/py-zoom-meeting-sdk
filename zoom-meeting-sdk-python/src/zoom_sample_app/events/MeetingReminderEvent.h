
#ifndef MEETING_SDK_LINUX_SAMPLE_MEETINGREMINDEREVENT_H
#define MEETING_SDK_LINUX_SAMPLE_MEETINGREMINDEREVENT_H


#include <iostream>
#include "meeting_service_components/meeting_reminder_ctrl_interface.h"

using namespace std;
using namespace ZOOMSDK;

class MeetingReminderEvent : public IMeetingReminderEvent
{
public:
    /**
     * Fires when the reminder dialog is shown
     * @param content content of the reminder
     * @param handle reminder handler for the reminder type
     */
    void onReminderNotify(IMeetingReminderContent* content, IMeetingReminderHandler* handle) override;

    /**
     * Callback event of the enable reminder dialog show.
     * @param content The detail content in the reminder dialog. For more details, see IMeetingEnableReminderHandler enum.
     * @param handle A pointer to the IMeetingReminderHandler. For more details, see IMeetingEnableReminderHandler.
    */
    void onEnableReminderNotify(IMeetingReminderContent* content, IMeetingEnableReminderHandler* handle) override {};
};


#endif //MEETING_SDK_LINUX_SAMPLE_MEETINGREMINDEREVENT_H
