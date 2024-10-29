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





#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void printJoinParam(const JoinParam& param) {
    std::cout << "JoinParam contents:" << std::endl;
    std::cout << "User Type: " << (param.userType == SDK_UT_NORMALUSER ? "Normal User" : "Without Login") << std::endl;

    if (param.userType == SDK_UT_NORMALUSER) {
        const auto& np = param.param.normaluserJoin;
        std::cout << "Normal User Join Parameters:" << std::endl;
        std::cout << "  Meeting Number: " << np.meetingNumber << std::endl;
        std::cout << "  Vanity ID: " << (np.vanityID ? np.vanityID : "NULL") << std::endl;
        std::cout << "  User Name: " << (np.userName ? np.userName : "NULL") << std::endl;
        std::cout << "  Password: " << (np.psw ? np.psw : "NULL") << std::endl;
        std::cout << "  App Privilege Token: " << (np.app_privilege_token ? np.app_privilege_token : "NULL") << std::endl;
        std::cout << "  Customer Key: " << (np.customer_key ? np.customer_key : "NULL") << std::endl;
        std::cout << "  Webinar Token: " << (np.webinarToken ? np.webinarToken : "NULL") << std::endl;
        std::cout << "  Video Off: " << (np.isVideoOff ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Off: " << (np.isAudioOff ? "Yes" : "No") << std::endl;
        std::cout << "  Join Token: " << (np.join_token ? np.join_token : "NULL") << std::endl;
        std::cout << "  Is My Voice In Mix: " << (np.isMyVoiceInMix ? "Yes" : "No") << std::endl;
        std::cout << "  Is Audio Raw Data Stereo: " << (np.isAudioRawDataStereo ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Raw Data Sampling Rate: " << np.eAudioRawdataSamplingRate << std::endl;
    } else {
        const auto& wp = param.param.withoutloginuserJoin;
        std::cout << "Without Login User Join Parameters:" << std::endl;
        std::cout << "  Meeting Number: " << wp.meetingNumber << std::endl;
        std::cout << "  Vanity ID: " << (wp.vanityID ? wp.vanityID : "NULL") << std::endl;
        std::cout << "  User Name: " << (wp.userName ? wp.userName : "NULL") << std::endl;
        std::cout << "  Password: " << (wp.psw ? wp.psw : "NULL") << std::endl;
        std::cout << "  App Privilege Token: " << (wp.app_privilege_token ? wp.app_privilege_token : "NULL") << std::endl;
        std::cout << "  User ZAK: " << (wp.userZAK ? wp.userZAK : "NULL") << std::endl;
        std::cout << "  Customer Key: " << (wp.customer_key ? wp.customer_key : "NULL") << std::endl;
        std::cout << "  Webinar Token: " << (wp.webinarToken ? wp.webinarToken : "NULL") << std::endl;
        std::cout << "  Video Off: " << (wp.isVideoOff ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Off: " << (wp.isAudioOff ? "Yes" : "No") << std::endl;
        std::cout << "  Join Token: " << (wp.join_token ? wp.join_token : "NULL") << std::endl;
        std::cout << "  Is My Voice In Mix: " << (wp.isMyVoiceInMix ? "Yes" : "No") << std::endl;
        std::cout << "  Is Audio Raw Data Stereo: " << (wp.isAudioRawDataStereo ? "Yes" : "No") << std::endl;
        std::cout << "  Audio Raw Data Sampling Rate: " << wp.eAudioRawdataSamplingRate << std::endl;
    }
}

void init_meeting_service_interface_binding(nb::module_ &m) {
    nb::enum_<ZOOM_SDK_NAMESPACE::SDKUserType>(m, "SDKUserType")
        .value("SDK_UT_NORMALUSER", ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER)
        .value("SDK_UT_WITHOUT_LOGIN", ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::StartParam4NormalUser>(m, "StartParam4NormalUser")
        .def(nb::init<>())
        .def_rw("meetingNumber", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::meetingNumber)
        .def_rw("vanityID", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::vanityID)
        .def_rw("customer_key", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::customer_key)
        .def_rw("isAudioOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isAudioOff)
        .def_rw("isVideoOff", &ZOOM_SDK_NAMESPACE::StartParam4NormalUser::isVideoOff);

    nb::class_<ZOOM_SDK_NAMESPACE::StartParam>(m, "StartParam")
        .def(nb::init<>())
        .def_rw("userType", &ZOOM_SDK_NAMESPACE::StartParam::userType)
        .def_prop_rw("normaluserStart",
            [](ZOOM_SDK_NAMESPACE::StartParam &p) { return p.param.normaluserStart; },
            [](ZOOM_SDK_NAMESPACE::StartParam &p, const ZOOM_SDK_NAMESPACE::StartParam4NormalUser &normalUser) { p.param.normaluserStart = normalUser; }
        );

    nb::class_<ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin>(m, "JoinParam4WithoutLogin")
        .def(nb::init<>())
        .def_rw("meetingNumber", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::meetingNumber)
        .def_rw("userName", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::userName)
        .def_rw("psw", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::psw)
        .def_prop_rw("vanityID",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.vanityID ? std::string(p.vanityID) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.vanityID = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("customer_key",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.customer_key ? std::string(p.customer_key) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.customer_key = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("webinarToken",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.webinarToken ? std::string(p.webinarToken) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.webinarToken = s.empty() ? nullptr : s.c_str(); })
        .def_rw("isVideoOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isVideoOff)
        .def_rw("isAudioOff", &ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin::isAudioOff)
        .def_prop_rw("userZAK",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.userZAK ? std::string(p.userZAK) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.userZAK = s.empty() ? nullptr : s.c_str(); })
        .def_prop_rw("app_privilege_token",
            [](const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p) { return p.app_privilege_token ? std::string(p.app_privilege_token) : std::string(); },
            [](ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &p, const std::string &s) { p.app_privilege_token = s.empty() ? nullptr : s.c_str(); });        

    nb::class_<ZOOM_SDK_NAMESPACE::JoinParam>(m, "JoinParam")
        .def(nb::init<>())
        .def_rw("userType", &ZOOM_SDK_NAMESPACE::JoinParam::userType)
        .def_prop_rw("param",
            [](ZOOM_SDK_NAMESPACE::JoinParam &p) -> ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin& { return p.param.withoutloginuserJoin; },
            [](ZOOM_SDK_NAMESPACE::JoinParam &p, const ZOOM_SDK_NAMESPACE::JoinParam4WithoutLogin &jp) { 
                std::cout << "meetingNumber " << jp.meetingNumber << std::endl; 
                std::cout << "mn" << jp.meetingNumber << std::endl;
                p.param.withoutloginuserJoin = jp; 
            }
        );

    nb::class_<IMeetingService>(m, "IMeetingService")
        .def("GetMeetingStatus", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingStatus)
        .def("Join", [](ZOOM_SDK_NAMESPACE::IMeetingService& self, ZOOM_SDK_NAMESPACE::JoinParam& param) {
            printJoinParam(param);
            return self.Join(param);
        })
        .def("SetEvent", &ZOOM_SDK_NAMESPACE::IMeetingService::SetEvent)
        .def("Start", &ZOOM_SDK_NAMESPACE::IMeetingService::Start)
        .def("Leave", &ZOOM_SDK_NAMESPACE::IMeetingService::Leave, nb::call_guard<nb::gil_scoped_release>())
        .def("GetMeetingRecordingController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingRecordingController, nb::rv_policy::reference)
        .def("GetMeetingReminderController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingReminderController, nb::rv_policy::reference)
        .def("GetMeetingParticipantsController", &ZOOM_SDK_NAMESPACE::IMeetingService::GetMeetingParticipantsController, nb::rv_policy::reference);

    nb::class_<IMeetingServiceEvent>(m, "IMeetingServiceEvent")
        .def("onMeetingStatusChanged", &IMeetingServiceEvent::onMeetingStatusChanged)
        .def("onMeetingStatisticsWarningNotification", &IMeetingServiceEvent::onMeetingStatisticsWarningNotification)
        .def("onMeetingParameterNotification", &IMeetingServiceEvent::onMeetingParameterNotification)
        .def("onSuspendParticipantsActivities", &IMeetingServiceEvent::onSuspendParticipantsActivities)
        .def("onAICompanionActiveChangeNotice", &IMeetingServiceEvent::onAICompanionActiveChangeNotice)
        .def("onMeetingTopicChanged", &IMeetingServiceEvent::onMeetingTopicChanged);

    nb::enum_<ZOOM_SDK_NAMESPACE::LeaveMeetingCmd>(m, "LeaveMeetingCmd")
        .value("LEAVE_MEETING", ZOOM_SDK_NAMESPACE::LEAVE_MEETING)
        .value("END_MEETING", ZOOM_SDK_NAMESPACE::END_MEETING)
        .export_values();       

    nb::enum_<MeetingEndReason>(m, "MeetingEndReason")
        .value("EndMeetingReason_None", EndMeetingReason_None)
        .value("EndMeetingReason_KickByHost", EndMeetingReason_KickByHost)
        .value("EndMeetingReason_EndByHost", EndMeetingReason_EndByHost)
        .value("EndMeetingReason_JBHTimeOut", EndMeetingReason_JBHTimeOut)
        .value("EndMeetingReason_NoAttendee", EndMeetingReason_NoAttendee)
        .value("EndMeetingReason_HostStartAnotherMeeting", EndMeetingReason_HostStartAnotherMeeting)
        .value("EndMeetingReason_FreeMeetingTimeOut", EndMeetingReason_FreeMeetingTimeOut)
        .value("EndMeetingReason_NetworkBroken", EndMeetingReason_NetworkBroken);

    nb::enum_<ZOOM_SDK_NAMESPACE::MeetingStatus>(m, "MeetingStatus")
        .value("MEETING_STATUS_IDLE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_IDLE)
        .value("MEETING_STATUS_CONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_CONNECTING)
        .value("MEETING_STATUS_WAITINGFORHOST", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WAITINGFORHOST)
        .value("MEETING_STATUS_INMEETING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_INMEETING)
        .value("MEETING_STATUS_DISCONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_DISCONNECTING)
        .value("MEETING_STATUS_RECONNECTING", ZOOM_SDK_NAMESPACE::MEETING_STATUS_RECONNECTING)
        .value("MEETING_STATUS_FAILED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_FAILED)
        .value("MEETING_STATUS_ENDED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_ENDED)
        .value("MEETING_STATUS_UNKNOWN", ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNKNOWN)
        .value("MEETING_STATUS_LOCKED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_LOCKED)
        .value("MEETING_STATUS_UNLOCKED", ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNLOCKED)
        .value("MEETING_STATUS_IN_WAITING_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_IN_WAITING_ROOM)
        .value("MEETING_STATUS_WEBINAR_PROMOTE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_PROMOTE)
        .value("MEETING_STATUS_WEBINAR_DEPROMOTE", ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_DEPROMOTE)
        .value("MEETING_STATUS_JOIN_BREAKOUT_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_JOIN_BREAKOUT_ROOM)
        .value("MEETING_STATUS_LEAVE_BREAKOUT_ROOM", ZOOM_SDK_NAMESPACE::MEETING_STATUS_LEAVE_BREAKOUT_ROOM)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::StatisticsWarningType>(m, "StatisticsWarningType")
        .value("Statistics_Warning_None", ZOOM_SDK_NAMESPACE::Statistics_Warning_None)
        .value("Statistics_Warning_Network_Quality_Bad", ZOOM_SDK_NAMESPACE::Statistics_Warning_Network_Quality_Bad)
        .export_values();

    nb::enum_<ZOOM_SDK_NAMESPACE::MeetingType>(m, "MeetingType")
        .value("MEETING_TYPE_NONE", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NONE)
        .value("MEETING_TYPE_NORMAL", ZOOM_SDK_NAMESPACE::MEETING_TYPE_NORMAL)
        .value("MEETING_TYPE_WEBINAR", ZOOM_SDK_NAMESPACE::MEETING_TYPE_WEBINAR)
        .value("MEETING_TYPE_BREAKOUTROOM", ZOOM_SDK_NAMESPACE::MEETING_TYPE_BREAKOUTROOM)
        .export_values();

    nb::class_<ZOOM_SDK_NAMESPACE::MeetingParameter>(m, "MeetingParameter")
        .def(nb::init<>())
        .def_rw("meeting_type", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_type)
        .def_rw("is_view_only", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_view_only)
        .def_rw("is_auto_recording_local", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_local)
        .def_rw("is_auto_recording_cloud", &ZOOM_SDK_NAMESPACE::MeetingParameter::is_auto_recording_cloud)
        .def_rw("meeting_number", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_number)
        .def_rw("meeting_topic", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_topic)
        .def_rw("meeting_host", &ZOOM_SDK_NAMESPACE::MeetingParameter::meeting_host);
}