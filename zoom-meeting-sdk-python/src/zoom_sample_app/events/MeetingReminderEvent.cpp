#include "MeetingReminderEvent.h"

void MeetingReminderEvent::onReminderNotify(IMeetingReminderContent* content, IMeetingReminderHandler* handle) {
    if (content) {
        cout << "Reminder Notification Received" << endl;
        cout << "Type: " << content->GetType() << endl;
        cout << "Title: " << content->GetTitle() << endl;
        cout << "Content: " << content->GetContent() << endl;
        cout << "Is Blocking?: " << content->IsBlocking() << endl;
    }

    if (handle)
        handle->Accept();
}