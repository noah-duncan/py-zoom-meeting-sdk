#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>

#include "zoom_sdk.h"

#include "meeting_service_interface.h"
#include "setting_service_interface.h"
#include "auth_service_interface.h"
#include "meeting_service_components/meeting_ai_companion_interface.h"
#include "meeting_service_components/meeting_recording_interface.h"
#include "meeting_service_components/meeting_audio_interface.h"
#include "meeting_service_components/meeting_reminder_ctrl_interface.h"
#include "meeting_service_components/meeting_breakout_rooms_interface_v2.h"
#include "meeting_service_components/meeting_sharing_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "meeting_service_components/meeting_smart_summary_interface.h"
#include "meeting_service_components/meeting_configuration_interface.h"
#include "meeting_service_components/meeting_video_interface.h"
#include "meeting_service_components/meeting_inmeeting_encryption_interface.h"
#include "meeting_service_components/meeting_participants_ctrl_interface.h"
#include "meeting_service_components/meeting_waiting_room_interface.h"
#include "meeting_service_components/meeting_webinar_interface.h"
#include "meeting_service_components/meeting_raw_archiving_interface.h"
#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"
#include "raw_record/ZoomSDKAudioRawDataDelegate.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

void init_m1(nb::module_ &m) {
    nb::class_<AuthServiceEvent, IAuthServiceEvent>(m, "AuthServiceEvent")
    .def(nb::init<std::function<void()>&>())
    .def("onAuthenticationReturn", &AuthServiceEvent::onAuthenticationReturn)
    .def("onLoginReturnWithReason", &AuthServiceEvent::onLoginReturnWithReason)
    .def("onLogout", &AuthServiceEvent::onLogout)
    .def("onZoomIdentityExpired", &AuthServiceEvent::onZoomIdentityExpired)
    .def("onZoomAuthIdentityExpired", &AuthServiceEvent::onZoomAuthIdentityExpired)
    .def("setOnAuth", &AuthServiceEvent::setOnAuth)
    .def("setOnAuthenticationReturn", &AuthServiceEvent::setOnAuthenticationReturn);

    nb::class_<MeetingServiceEvent, IMeetingServiceEvent>(m, "MeetingServiceEvent")
    .def(nb::init<>())
    .def("onMeetingStatusChanged", &MeetingServiceEvent::onMeetingStatusChanged)
    .def("onMeetingParameterNotification", &MeetingServiceEvent::onMeetingParameterNotification)
    .def("onMeetingStatisticsWarningNotification", &MeetingServiceEvent::onMeetingStatisticsWarningNotification)
    .def("onSuspendParticipantsActivities", &MeetingServiceEvent::onSuspendParticipantsActivities)
    .def("onAICompanionActiveChangeNotice", &MeetingServiceEvent::onAICompanionActiveChangeNotice)
    .def("onMeetingTopicChanged", &MeetingServiceEvent::onMeetingTopicChanged)
    .def("setOnMeetingJoin", &MeetingServiceEvent::setOnMeetingJoin)
    .def("setOnMeetingEnd", &MeetingServiceEvent::setOnMeetingEnd)
    .def("setOnMeetingStatusChanged", &MeetingServiceEvent::setOnMeetingStatusChanged);

    // Binding for MeetingReminderEvent
    nb::class_<MeetingReminderEvent, IMeetingReminderEvent>(m, "MeetingReminderEvent")
        .def(nb::init<>())
        .def("onReminderNotify", &MeetingReminderEvent::onReminderNotify)
        .def("onEnableReminderNotify", &MeetingReminderEvent::onEnableReminderNotify);
}