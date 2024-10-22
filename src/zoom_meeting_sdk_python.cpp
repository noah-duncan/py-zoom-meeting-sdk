#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

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

void init_m1(nb::module_ &);
void init_m3(nb::module_ &);
void init_m4(nb::module_ &);

void init_auth_service_interface_binding(nb::module_ &);
void init_meeting_service_interface_binding(nb::module_ &);
void init_zoom_rawdata_api_binding(nb::module_ &);
void init_zoom_sdk_raw_data_def_interface_binding(nb::module_ &);
void init_meeting_recording_interface_binding(nb::module_ &);
void init_rawdata_audio_helper_interface_binding(nb::module_ &);
void init_zoom_sdk_binding(nb::module_ &);
void init_meeting_reminder_ctrl_interface_binding(nb::module_ &);
void init_setting_service_interface_binding(nb::module_ &);
void init_zoom_sdk_def_binding(nb::module_ &);
void init_meeting_participants_ctrl_interface_binding(nb::module_ &);
void init_auth_service_event_callbacks(nb::module_ &);
void init_meeting_service_event_callbacks(nb::module_ &);
void init_meeting_reminder_event_callbacks(nb::module_ &);

NB_MODULE(_zoom_meeting_sdk_python_impl, m) {
    m.doc() = "Python bindings for Zoom SDK";
    //nb::set_leak_warnings(false);

    init_auth_service_interface_binding(m);
    init_meeting_service_interface_binding(m);
    init_zoom_rawdata_api_binding(m);
    init_zoom_sdk_raw_data_def_interface_binding(m);
    init_meeting_recording_interface_binding(m);
    init_rawdata_audio_helper_interface_binding(m);
    init_zoom_sdk_binding(m);
    init_meeting_reminder_ctrl_interface_binding(m);
    init_setting_service_interface_binding(m);
    init_zoom_sdk_def_binding(m);
    init_meeting_participants_ctrl_interface_binding(m);

    init_auth_service_event_callbacks(m);
    init_meeting_service_event_callbacks(m);
    init_meeting_reminder_event_callbacks(m);

    init_m1(m);
    init_m3(m);
    init_m4(m);
}