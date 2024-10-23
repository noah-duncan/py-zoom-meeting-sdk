#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"

#include "auth_service_interface.h"
#include "meeting_service_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

/*
	virtual ~IMeetingServiceEvent() {}

	/// \brief Meeting status changed callback.
	/// \param status The value of meeting. For more details, see \link MeetingStatus \endlink.
	/// \param iResult Detailed reasons for special meeting status.
	///If the status is MEETING_STATUS_FAILED, the value of iResult is one of those listed in MeetingFailCode enum. 
	///If the status is MEETING_STATUS_ENDED, the value of iResult is one of those listed in MeetingEndReason.
	virtual void onMeetingStatusChanged(MeetingStatus status, int iResult = 0) = 0;

	/// \brief Meeting statistics warning notification callback.
	/// \param type The warning type of the meeting statistics. For more details, see \link StatisticsWarningType \endlink.
	virtual void onMeetingStatisticsWarningNotification(StatisticsWarningType type) = 0;

	/// \brief Meeting parameter notification callback.
	/// \param meeting_param Meeting parameter. For more details, see \link MeetingParameter \endlink.
	/// \remarks The callback will be triggered right before the meeting starts. The meeting_param will be destroyed once the function calls end.
	virtual void onMeetingParameterNotification(const MeetingParameter* meeting_param) = 0;

	/// \brief Callback event when a meeting is suspended.
	virtual void onSuspendParticipantsActivities() = 0;

	/// \brief Callback event for the AI Companion active status changed. 
	/// \param active Specify whether the AI Companion active or not.
	virtual void onAICompanionActiveChangeNotice(bool bActive) = 0;

	/// \brief Callback event for the meeting topic changed. 
	/// \param sTopic The new meeting topic.
	virtual void onMeetingTopicChanged(const zchar_t* sTopic) = 0;
*/

class MeetingServiceEventCallbacks : public ZOOM_SDK_NAMESPACE::IMeetingServiceEvent {
private:
    std::function<void(ZOOM_SDK_NAMESPACE::MeetingStatus, int)> m_onMeetingStatusChangedCallback;
    std::function<void(ZOOM_SDK_NAMESPACE::StatisticsWarningType)> m_onMeetingStatisticsWarningNotificationCallback;
    std::function<void(const ZOOM_SDK_NAMESPACE::MeetingParameter*)> m_onMeetingParameterNotificationCallback;
    std::function<void()> m_onSuspendParticipantsActivitiesCallback;
    std::function<void(bool)> m_onAICompanionActiveChangeNoticeCallback;
    std::function<void(const zchar_t*)> m_onMeetingTopicChangedCallback;

public:
    MeetingServiceEventCallbacks(
        const std::function<void(ZOOM_SDK_NAMESPACE::MeetingStatus, int)>& onMeetingStatusChangedCallback = nullptr,
        const std::function<void(ZOOM_SDK_NAMESPACE::StatisticsWarningType)>& onMeetingStatisticsWarningNotificationCallback = nullptr,
        const std::function<void(const ZOOM_SDK_NAMESPACE::MeetingParameter*)>& onMeetingParameterNotificationCallback = nullptr,
        const std::function<void()>& onSuspendParticipantsActivitiesCallback = nullptr,
        const std::function<void(bool)>& onAICompanionActiveChangeNoticeCallback = nullptr,
        const std::function<void(const zchar_t*)>& onMeetingTopicChangedCallback = nullptr
    ) : m_onMeetingStatusChangedCallback(onMeetingStatusChangedCallback),
        m_onMeetingStatisticsWarningNotificationCallback(onMeetingStatisticsWarningNotificationCallback),
        m_onMeetingParameterNotificationCallback(onMeetingParameterNotificationCallback),
        m_onSuspendParticipantsActivitiesCallback(onSuspendParticipantsActivitiesCallback),
        m_onAICompanionActiveChangeNoticeCallback(onAICompanionActiveChangeNoticeCallback),
        m_onMeetingTopicChangedCallback(onMeetingTopicChangedCallback) {}

    void onMeetingStatusChanged(ZOOM_SDK_NAMESPACE::MeetingStatus status, int iResult = 0) override {
        if (m_onMeetingStatusChangedCallback)
            m_onMeetingStatusChangedCallback(status, iResult);
    }

    void onMeetingStatisticsWarningNotification(ZOOM_SDK_NAMESPACE::StatisticsWarningType type) override {
        if (m_onMeetingStatisticsWarningNotificationCallback)
            m_onMeetingStatisticsWarningNotificationCallback(type);
    }

    void onMeetingParameterNotification(const ZOOM_SDK_NAMESPACE::MeetingParameter* meeting_param) override {
        if (m_onMeetingParameterNotificationCallback)
            m_onMeetingParameterNotificationCallback(meeting_param);
    }

    void onSuspendParticipantsActivities() override {
        if (m_onSuspendParticipantsActivitiesCallback)
            m_onSuspendParticipantsActivitiesCallback();
    }

    void onAICompanionActiveChangeNotice(bool bActive) override {
        if (m_onAICompanionActiveChangeNoticeCallback)
            m_onAICompanionActiveChangeNoticeCallback(bActive);
    }

    void onMeetingTopicChanged(const zchar_t* sTopic) override {
        if (m_onMeetingTopicChangedCallback)
            m_onMeetingTopicChangedCallback(sTopic);
    }
};

void init_meeting_service_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingServiceEventCallbacks, ZOOM_SDK_NAMESPACE::IMeetingServiceEvent>(m, "MeetingServiceEventCallbacks")
        .def(
            nb::init<
                std::function<void(ZOOM_SDK_NAMESPACE::MeetingStatus, int)>&,
                std::function<void(ZOOM_SDK_NAMESPACE::StatisticsWarningType)>&,
                std::function<void(const ZOOM_SDK_NAMESPACE::MeetingParameter*)>&,
                std::function<void()>&,
                std::function<void(bool)>&,
                std::function<void(const zchar_t*)>&
            >(),
            nb::arg("onMeetingStatusChangedCallback") = nullptr,
            nb::arg("onMeetingStatisticsWarningNotificationCallback") = nullptr,
            nb::arg("onMeetingParameterNotificationCallback") = nullptr,
            nb::arg("onSuspendParticipantsActivitiesCallback") = nullptr,
            nb::arg("onAICompanionActiveChangeNoticeCallback") = nullptr,
            nb::arg("onMeetingTopicChangedCallback") = nullptr
        );

}