#include <nanobind/nanobind.h>

namespace nb = nanobind;

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
void init_zoom_sdk_audio_raw_data_delegate_callbacks(nb::module_ &);
void init_zoom_sdk_virtual_audio_mic_event_callbacks(nb::module_ &);
void init_meeting_recording_ctrl_event_callbacks(nb::module_ &);

NB_MODULE(_zoom_meeting_sdk_impl, m) {
    m.doc() = "Python bindings for Zoom Meeting SDK";
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
    init_zoom_sdk_audio_raw_data_delegate_callbacks(m);
    init_zoom_sdk_virtual_audio_mic_event_callbacks(m);
    init_meeting_recording_ctrl_event_callbacks(m);
}