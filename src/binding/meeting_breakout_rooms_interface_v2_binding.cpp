#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "meeting_service_components/meeting_breakout_rooms_interface_v2.h"

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;
using namespace nb::literals;

void init_meeting_breakout_rooms_interface_v2_binding(nb::module_ &m) {
  // Enums
  nb::enum_<PreAssignBODataStatus>(m, "PreAssignBODataStatus")
    .value("PreAssignBODataStatus_none", PreAssignBODataStatus_none)
    .value("PreAssignBODataStatus_downloading", PreAssignBODataStatus_downloading)
    .value("PreAssignBODataStatus_download_ok", PreAssignBODataStatus_download_ok)
    .value("PreAssignBODataStatus_download_fail", PreAssignBODataStatus_download_fail);

  nb::enum_<BO_STOP_COUNTDOWN>(m, "BO_STOP_COUNTDOWN")
    .value("BO_STOP_NOT_COUNTDOWN", BO_STOP_NOT_COUNTDOWN)
    .value("BO_STOP_COUNTDOWN_SECONDS_10", BO_STOP_COUNTDOWN_SECONDS_10)
    .value("BO_STOP_COUNTDOWN_SECONDS_15", BO_STOP_COUNTDOWN_SECONDS_15)
    .value("BO_STOP_COUNTDOWN_SECONDS_30", BO_STOP_COUNTDOWN_SECONDS_30)
    .value("BO_STOP_COUNTDOWN_SECONDS_60", BO_STOP_COUNTDOWN_SECONDS_60)
    .value("BO_STOP_COUNTDOWN_SECONDS_120", BO_STOP_COUNTDOWN_SECONDS_120);

  nb::enum_<BO_STATUS>(m, "BO_STATUS")
    .value("BO_STATUS_INVALID", BO_STATUS_INVALID)
    .value("BO_STATUS_EDIT", BO_STATUS_EDIT)
    .value("BO_STATUS_STARTED", BO_STATUS_STARTED)
    .value("BO_STATUS_STOPPING", BO_STATUS_STOPPING)
    .value("BO_STATUS_ENDED", BO_STATUS_ENDED);

  // Structs
  nb::class_<BOOption>(m, "BOOption")
    .def(nb::init<>())
    .def_rw("countdown_seconds", &BOOption::countdown_seconds)
    .def_rw("IsParticipantCanChooseBO", &BOOption::IsParticipantCanChooseBO)
    .def_rw("IsParticipantCanReturnToMainSessionAtAnyTime", &BOOption::IsParticipantCanReturnToMainSessionAtAnyTime)
    .def_rw("IsAutoMoveAllAssignedParticipantsEnabled", &BOOption::IsAutoMoveAllAssignedParticipantsEnabled)
    .def_rw("IsBOTimerEnabled", &BOOption::IsBOTimerEnabled)
    .def_rw("IsTimerAutoStopBOEnabled", &BOOption::IsTimerAutoStopBOEnabled)
    .def_rw("nTimerDuration", &BOOption::nTimerDuration)
    .def_rw("IsAttendeeContained", &BOOption::IsAttendeeContained)
    .def_rw("IsPanelistCanChooseBO", &BOOption::IsPanelistCanChooseBO)
    .def_rw("IsAttendeeCanChooseBO", &BOOption::IsAttendeeCanChooseBO)
    .def_rw("IsUserConfigMaxRoomUserLimitsEnabled", &BOOption::IsUserConfigMaxRoomUserLimitsEnabled)
    .def_rw("nUserConfigMaxRoomUserLimits", &BOOption::nUserConfigMaxRoomUserLimits);

  // Helper classes
  nb::class_<IBatchCreateBOHelper>(m, "IBatchCreateBOHelper")
    .def("CreateBOTransactionBegin", &IBatchCreateBOHelper::CreateBOTransactionBegin)
    .def("AddNewBoToList", &IBatchCreateBOHelper::AddNewBoToList)
    .def("CreateBoTransactionCommit", &IBatchCreateBOHelper::CreateBoTransactionCommit);

  nb::class_<IReturnToMainSessionHandler>(m, "IReturnToMainSessionHandler")
    .def("ReturnToMainSession", &IReturnToMainSessionHandler::ReturnToMainSession)
    .def("Ignore", &IReturnToMainSessionHandler::Ignore);

  nb::class_<IBOCreatorEvent>(m, "IBOCreatorEvent")
    .def("onBOCreateSuccess", &IBOCreatorEvent::onBOCreateSuccess)
    .def("OnWebPreAssignBODataDownloadStatusChanged", &IBOCreatorEvent::OnWebPreAssignBODataDownloadStatusChanged)
    .def("OnBOOptionChanged", &IBOCreatorEvent::OnBOOptionChanged)
    .def("onCreateBOResponse", &IBOCreatorEvent::onCreateBOResponse)
    .def("onRemoveBOResponse", &IBOCreatorEvent::onRemoveBOResponse)
    .def("onUpdateBONameResponse", &IBOCreatorEvent::onUpdateBONameResponse);

  // Main interface classes
  nb::class_<IBOCreator>(m, "IBOCreator")
    .def("SetEvent", &IBOCreator::SetEvent)
    .def("CreateBO", &IBOCreator::CreateBO)
    .def("CreateBreakoutRoom", &IBOCreator::CreateBreakoutRoom)
    .def("UpdateBOName", &IBOCreator::UpdateBOName)
    .def("RemoveBO", &IBOCreator::RemoveBO)
    .def("AssignUserToBO", &IBOCreator::AssignUserToBO)
    .def("RemoveUserFromBO", &IBOCreator::RemoveUserFromBO)
    .def("SetBOOption", &IBOCreator::SetBOOption)
    .def("GetBOOption", &IBOCreator::GetBOOption)
    .def("GetBatchCreateBOHelper", &IBOCreator::GetBatchCreateBOHelper)
    .def("IsWebPreAssignBOEnabled", &IBOCreator::IsWebPreAssignBOEnabled)
    .def("RequestAndUseWebPreAssignBOList", &IBOCreator::RequestAndUseWebPreAssignBOList)
    .def("GetWebPreAssignBODataStatus", &IBOCreator::GetWebPreAssignBODataStatus)
    .def("CreateWebinarBo", &IBOCreator::CreateWebinarBo);

  nb::class_<IBOAttendee>(m, "IBOAttendee")
    .def("JoinBo", &IBOAttendee::JoinBo)
    .def("LeaveBo", &IBOAttendee::LeaveBo)
    .def("GetBoName", &IBOAttendee::GetBoName)
    .def("SetEvent", &IBOAttendee::SetEvent)
    .def("RequestForHelp", &IBOAttendee::RequestForHelp)
    .def("IsHostInThisBO", &IBOAttendee::IsHostInThisBO)
    .def("IsCanReturnMainSession", &IBOAttendee::IsCanReturnMainSession);

  nb::class_<IMeetingBOControllerEvent>(m, "IMeetingBOControllerEvent")
    .def("onHasAttendeeRightsNotification", &IMeetingBOControllerEvent::onHasAttendeeRightsNotification)
    .def("onHasCreatorRightsNotification", &IMeetingBOControllerEvent::onHasCreatorRightsNotification)
    .def("onHasAdminRightsNotification", &IMeetingBOControllerEvent::onHasAdminRightsNotification)
    .def("onHasAssistantRightsNotification", &IMeetingBOControllerEvent::onHasAssistantRightsNotification)
    .def("onHasDataHelperRightsNotification", &IMeetingBOControllerEvent::onHasDataHelperRightsNotification)
    .def("onLostCreatorRightsNotification", &IMeetingBOControllerEvent::onLostCreatorRightsNotification)
    .def("onLostAdminRightsNotification", &IMeetingBOControllerEvent::onLostAdminRightsNotification)
    .def("onLostAssistantRightsNotification", &IMeetingBOControllerEvent::onLostAssistantRightsNotification)
    .def("onLostAttendeeRightsNotification", &IMeetingBOControllerEvent::onLostAttendeeRightsNotification)
    .def("onLostDataHelperRightsNotification", &IMeetingBOControllerEvent::onLostDataHelperRightsNotification)
    .def("onNewBroadcastMessageReceived", &IMeetingBOControllerEvent::onNewBroadcastMessageReceived)
    .def("onBOStopCountDown", &IMeetingBOControllerEvent::onBOStopCountDown)
    .def("onHostInviteReturnToMainSession", &IMeetingBOControllerEvent::onHostInviteReturnToMainSession)
    .def("onBOStatusChanged", &IMeetingBOControllerEvent::onBOStatusChanged)
    .def("onBOSwitchRequestReceived", &IMeetingBOControllerEvent::onBOSwitchRequestReceived)
    .def("onBroadcastBOVoiceStatus", &IMeetingBOControllerEvent::onBroadcastBOVoiceStatus)
    ;

  nb::class_<IMeetingBOController>(m, "IMeetingBOController")
    .def("SetEvent", &IMeetingBOController::SetEvent)
    .def("GetBOCreatorHelper", &IMeetingBOController::GetBOCreatorHelper)
    .def("GetBOAdminHelper", &IMeetingBOController::GetBOAdminHelper)
    .def("GetBOAssistantHelper", &IMeetingBOController::GetBOAssistantHelper)
    .def("GetBOAttedeeHelper", &IMeetingBOController::GetBOAttedeeHelper)
    .def("GetBODataHelper", &IMeetingBOController::GetBODataHelper)
    .def("IsBOStarted", &IMeetingBOController::IsBOStarted)
    .def("IsBOEnabled", &IMeetingBOController::IsBOEnabled)
    .def("IsInBOMeeting", &IMeetingBOController::IsInBOMeeting);
}
