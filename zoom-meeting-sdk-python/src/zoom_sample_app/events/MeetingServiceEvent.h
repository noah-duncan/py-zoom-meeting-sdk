#ifndef MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H
#define MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H

#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include "meeting_service_interface.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class MeetingServiceEvent : public IMeetingServiceEvent {
    function<void()> m_onMeetingJoin;
    function<void()> m_onMeetingEnd;
    function<void(MeetingStatus status, int iResult)> m_onMeetingStatusChanged;

public:
    MeetingServiceEvent() {};

    /**
     * Meeting status changed callback
     * @param status value of the current meeting status
     * @param iResult detailed reasons for special meeting statuses
     */
    void onMeetingStatusChanged(MeetingStatus status, int iResult) override;

    /**
     * callback will be triggered right before the meeting starts
     * The meeting_param will be destroyed once the function calls end
     * @param meeting_param holds parameters for a newly created meeting
     */
    void onMeetingParameterNotification(const MeetingParameter* meeting_param) override {};

    /**
     * callback used when there are Meeting statistics warning notifications
     * @param type type of statistics warning
     */
    void onMeetingStatisticsWarningNotification(StatisticsWarningType type) override {};

    /**
     * Callback event when a meeting is suspended
     */
    void onSuspendParticipantsActivities() override {};

    /**
     * Callback event used when the AI Companion active status changed.
     * @param bActive true if the AI Companion is active
     */
    void onAICompanionActiveChangeNotice(bool bActive) override {};

    /**
     * Callback event used when the Meeting Topic changes
     * @param sTopic string containing the meeting topic
     */
    void onMeetingTopicChanged(const zchar_t* sTopic) override {};


    /* Setters for Callbacks */
    void setOnMeetingJoin(const function<void()>& callback);
    void setOnMeetingEnd(const function<void()>& callback);
    void setOnMeetingStatusChanged(const function<void(MeetingStatus, int)>& callback);
};

#endif //MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H
