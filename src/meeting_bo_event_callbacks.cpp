#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_breakout_rooms_interface_v2.h"

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

class MeetingBOEventCallbacks : public IMeetingBOControllerEvent {
private:
  function<void(IBOAttendee*)> m_onHasAttendeeRightsNotificationCallback;
  function<void(IBOCreator*)> m_onHasCreatorRightsNotificationCallback;
  function<void(IBOAdmin*)> m_onHasAdminRightsNotificationCallback;
  function<void(IBOAssistant*)> m_onHasAssistantRightsNotificationCallback;
  function<void(IBOData*)> m_onHasDataHelperRightsNotificationCallback;
  function<void()> m_onLostCreatorRightsNotificationCallback;
  function<void()> m_onLostAdminRightsNotificationCallback;
  function<void()> m_onLostAssistantRightsNotificationCallback;
  function<void()> m_onLostAttendeeRightsNotificationCallback;
  function<void()> m_onLostDataHelperRightsNotificationCallback;
  function<void(const zchar_t*, unsigned int, const zchar_t*)> m_onNewBroadcastMessageReceivedCallback;
  function<void(unsigned int)> m_onBOStopCountDownCallback;
  function<void(const zchar_t*, IReturnToMainSessionHandler*)> m_onHostInviteReturnToMainSessionCallback;
  function<void(BO_STATUS)> m_onBOStatusChangedCallback;
  function<void(const zchar_t*, const zchar_t*)> m_onBOSwitchRequestReceivedCallback;
  function<void(bool)> m_onBroadcastBOVoiceStatusCallback;

public:
  MeetingBOEventCallbacks(
    const function<void(IBOAttendee*)>& onHasAttendeeRightsNotificationCallback = nullptr,
    const function<void(IBOCreator*)>& onHasCreatorRightsNotificationCallback = nullptr,
    const function<void(IBOAdmin*)>& onHasAdminRightsNotificationCallback = nullptr,
    const function<void(IBOAssistant*)>& onHasAssistantRightsNotificationCallback = nullptr,
    const function<void(IBOData*)>& onHasDataHelperRightsNotificationCallback = nullptr,
    const function<void()>& onLostCreatorRightsNotificationCallback = nullptr,
    const function<void()>& onLostAdminRightsNotificationCallback = nullptr,
    const function<void()>& onLostAssistantRightsNotificationCallback = nullptr,
    const function<void()>& onLostAttendeeRightsNotificationCallback = nullptr,
    const function<void()>& onLostDataHelperRightsNotificationCallback = nullptr,
    const function<void(const zchar_t*, unsigned int, const zchar_t*)>& onNewBroadcastMessageReceivedCallback = nullptr,
    const function<void(unsigned int)>& onBOStopCountDownCallback = nullptr,
    const function<void(const zchar_t*, IReturnToMainSessionHandler*)>& onHostInviteReturnToMainSessionCallback = nullptr,
    const function<void(BO_STATUS)>& onBOStatusChangedCallback = nullptr,
    const function<void(const zchar_t*, const zchar_t*)>& onBOSwitchRequestReceivedCallback = nullptr,
    const function<void(bool)>& onBroadcastBOVoiceStatusCallback = nullptr
  ) : m_onHasAttendeeRightsNotificationCallback(onHasAttendeeRightsNotificationCallback),
      m_onHasCreatorRightsNotificationCallback(onHasCreatorRightsNotificationCallback),
      m_onHasAdminRightsNotificationCallback(onHasAdminRightsNotificationCallback),
      m_onHasAssistantRightsNotificationCallback(onHasAssistantRightsNotificationCallback),
      m_onHasDataHelperRightsNotificationCallback(onHasDataHelperRightsNotificationCallback),
      m_onLostCreatorRightsNotificationCallback(onLostCreatorRightsNotificationCallback),
      m_onLostAdminRightsNotificationCallback(onLostAdminRightsNotificationCallback),
      m_onLostAssistantRightsNotificationCallback(onLostAssistantRightsNotificationCallback),
      m_onLostAttendeeRightsNotificationCallback(onLostAttendeeRightsNotificationCallback),
      m_onLostDataHelperRightsNotificationCallback(onLostDataHelperRightsNotificationCallback),
      m_onNewBroadcastMessageReceivedCallback(onNewBroadcastMessageReceivedCallback),
      m_onBOStopCountDownCallback(onBOStopCountDownCallback),
      m_onHostInviteReturnToMainSessionCallback(onHostInviteReturnToMainSessionCallback),
      m_onBOStatusChangedCallback(onBOStatusChangedCallback),
      m_onBOSwitchRequestReceivedCallback(onBOSwitchRequestReceivedCallback),
      m_onBroadcastBOVoiceStatusCallback(onBroadcastBOVoiceStatusCallback)
      {}

  void onHasCreatorRightsNotification(IBOCreator* pCreatorObj) override {
    if (m_onHasCreatorRightsNotificationCallback)
      m_onHasCreatorRightsNotificationCallback(pCreatorObj);
  }

  void onHasAdminRightsNotification(IBOAdmin* pAdminObj) override {
    if (m_onHasAdminRightsNotificationCallback)
      m_onHasAdminRightsNotificationCallback(pAdminObj);
  }

  void onHasAssistantRightsNotification(IBOAssistant* pAssistantObj) override {
    if (m_onHasAssistantRightsNotificationCallback)
      m_onHasAssistantRightsNotificationCallback(pAssistantObj);
  }

  void onHasAttendeeRightsNotification(IBOAttendee* pAttendeeObj) override {
    if (m_onHasAttendeeRightsNotificationCallback)
      m_onHasAttendeeRightsNotificationCallback(pAttendeeObj);
  }

  void onHasDataHelperRightsNotification(IBOData* pDataHelperObj) override {
    if (m_onHasDataHelperRightsNotificationCallback)
      m_onHasDataHelperRightsNotificationCallback(pDataHelperObj);
  }

  void onLostCreatorRightsNotification() override {
    if (m_onLostCreatorRightsNotificationCallback)
      m_onLostCreatorRightsNotificationCallback();
  }

  void onLostAdminRightsNotification() override {
    if (m_onLostAdminRightsNotificationCallback)
      m_onLostAdminRightsNotificationCallback();
  }

  void onLostAssistantRightsNotification() override {
    if (m_onLostAssistantRightsNotificationCallback)
      m_onLostAssistantRightsNotificationCallback();
  }

  void onLostAttendeeRightsNotification() override {
    if (m_onLostAttendeeRightsNotificationCallback)
      m_onLostAttendeeRightsNotificationCallback();
  }

  void onLostDataHelperRightsNotification() override {
    if (m_onLostDataHelperRightsNotificationCallback)
      m_onLostDataHelperRightsNotificationCallback();
  }

  void onNewBroadcastMessageReceived(const zchar_t* strMsg, unsigned int nSenderID, const zchar_t* strSenderName) override {
    if (m_onNewBroadcastMessageReceivedCallback)
      m_onNewBroadcastMessageReceivedCallback(strMsg, nSenderID, strSenderName);
  }

  void onBOStopCountDown(unsigned int nSeconds) override {
    if (m_onBOStopCountDownCallback)
      m_onBOStopCountDownCallback(nSeconds);
  }

  void onHostInviteReturnToMainSession(const zchar_t* strName, IReturnToMainSessionHandler* handler) override {
    if (m_onHostInviteReturnToMainSessionCallback)
      m_onHostInviteReturnToMainSessionCallback(strName, handler);
  }

  void onBOStatusChanged(BO_STATUS eStatus) override {
    if (m_onBOStatusChangedCallback)
      m_onBOStatusChangedCallback(eStatus);
  }

  void onBOSwitchRequestReceived(const zchar_t* strNewBOName, const zchar_t* strNewBOID) override {
    if (m_onBOSwitchRequestReceivedCallback)
      m_onBOSwitchRequestReceivedCallback(strNewBOName, strNewBOID);
  }

  void onBroadcastBOVoiceStatus(bool bStart) override {
    if (m_onBroadcastBOVoiceStatusCallback)
      m_onBroadcastBOVoiceStatusCallback(bStart);
  }
};

class BOAdminEventCallbacks : public IBOAdminEvent {
private:
  function<void(const zchar_t*)> m_onHelpRequestReceivedCallback;
  function<void(BOControllerError)> m_onStartBOErrorCallback;
  function<void(int, bool)> m_onBOEndTimerUpdatedCallback;
  function<void(bool)> m_onStartBOResponseCallback;
  function<void(bool)> m_onStopBOResponseCallback;

public:
  BOAdminEventCallbacks(
    const function<void(const zchar_t*)>& onHelpRequestReceivedCallback = nullptr,
    const function<void(BOControllerError)>& onStartBOErrorCallback = nullptr,
    const function<void(int, bool)>& onBOEndTimerUpdatedCallback = nullptr,
    const function<void(bool)>& onStartBOResponseCallback = nullptr,
    const function<void(bool)>& onStopBOResponseCallback = nullptr
  ) : m_onHelpRequestReceivedCallback(onHelpRequestReceivedCallback),
      m_onStartBOErrorCallback(onStartBOErrorCallback),
      m_onBOEndTimerUpdatedCallback(onBOEndTimerUpdatedCallback),
      m_onStartBOResponseCallback(onStartBOResponseCallback),
      m_onStopBOResponseCallback(onStopBOResponseCallback)
      {}

  void onHelpRequestReceived(const zchar_t* strUserID) override {
    if (m_onHelpRequestReceivedCallback)
      m_onHelpRequestReceivedCallback(strUserID);
  }

  void onStartBOError(BOControllerError errCode) override {
    if (m_onStartBOErrorCallback)
      m_onStartBOErrorCallback(errCode);
  }

  void onBOEndTimerUpdated(int remaining, bool isTimesUpNotice) override {
    if (m_onBOEndTimerUpdatedCallback)
      m_onBOEndTimerUpdatedCallback(remaining, isTimesUpNotice);
  }

  void onStartBOResponse(bool bSuccess) override {
    if (m_onStartBOResponseCallback)
      m_onStartBOResponseCallback(bSuccess);
  }

  void onStopBOResponse(bool bSuccess) override {
    if (m_onStopBOResponseCallback)
      m_onStopBOResponseCallback(bSuccess);
  }
};

class BODataEventCallbacks : public IBODataEvent {
private:
  function<void(const zchar_t*)> m_onBOInfoUpdatedCallback;
  function<void()> m_onUnAssignedUserUpdatedCallback;
  function<void()> m_onBOListInfoUpdatedCallback;

public:
  BODataEventCallbacks(
    const function<void(const zchar_t*)>& onBOInfoUpdatedCallback = nullptr,
    const function<void()>& onUnAssignedUserUpdatedCallback = nullptr,
    const function<void()>& onBOListInfoUpdatedCallback = nullptr
  ) : m_onBOInfoUpdatedCallback(onBOInfoUpdatedCallback),
      m_onUnAssignedUserUpdatedCallback(onUnAssignedUserUpdatedCallback),
      m_onBOListInfoUpdatedCallback(onBOListInfoUpdatedCallback)
      {}

  void onBOInfoUpdated(const zchar_t* strBOID) override {
    if (m_onBOInfoUpdatedCallback)
      m_onBOInfoUpdatedCallback(strBOID);
  }

  void onUnAssignedUserUpdated() override {
    if (m_onUnAssignedUserUpdatedCallback)
      m_onUnAssignedUserUpdatedCallback();
  }

  void OnBOListInfoUpdated() override {
    if (m_onBOListInfoUpdatedCallback)
      m_onBOListInfoUpdatedCallback();
  }
};

void init_meeting_bo_event_callbacks(nb::module_ &m) {
  nb::class_<BOAdminEventCallbacks, IBOAdminEvent>(m, "BOAdminEventCallbacks")
    .def(nb::init<
      const function<void(const zchar_t*)>&,
      const function<void(BOControllerError)>&,
      const function<void(int, bool)>&,
      const function<void(bool)>&,
      const function<void(bool)>&
    >(),
      nb::arg("onHelpRequestReceivedCallback") = nullptr,
      nb::arg("onStartBOErrorCallback") = nullptr,
      nb::arg("onBOEndTimerUpdatedCallback") = nullptr,
      nb::arg("onStartBOResponseCallback") = nullptr,
      nb::arg("onStopBOResponseCallback") = nullptr
    );

  nb::class_<BODataEventCallbacks, IBODataEvent>(m, "BODataEventCallbacks")
    .def(nb::init<
      const function<void(const zchar_t*)>&,
      const function<void()>&,
      const function<void()>&
    >(),
      nb::arg("onBOInfoUpdatedCallback") = nullptr,
      nb::arg("onUnAssignedUserUpdatedCallback") = nullptr,
      nb::arg("onBOListInfoUpdatedCallback") = nullptr
    );

  nb::class_<MeetingBOEventCallbacks, IMeetingBOControllerEvent>(m, "MeetingBOEventCallbacks")
    .def(nb::init<
      const function<void(IBOAttendee*)>&,
      const function<void(IBOCreator*)>&,
      const function<void(IBOAdmin*)>&,
      const function<void(IBOAssistant*)>&,
      const function<void(IBOData*)>&,
      const function<void()>&,
      const function<void()>&,
      const function<void()>&,
      const function<void()>&,
      const function<void()>&,
      const function<void(const zchar_t*, unsigned int, const zchar_t*)>&,
      const function<void(unsigned int)>&,
      const function<void(const zchar_t*, IReturnToMainSessionHandler*)>&,
      const function<void(BO_STATUS)>&,
      const function<void(const zchar_t*, const zchar_t*)>&,
      const function<void(bool)>&
    >(),
      nb::arg("onHasAttendeeRightsNotificationCallback") = nullptr,
      nb::arg("onHasCreatorRightsNotificationCallback") = nullptr,
      nb::arg("onHasAdminRightsNotificationCallback") = nullptr,
      nb::arg("onHasAssistantRightsNotificationCallback") = nullptr,
      nb::arg("onHasDataHelperRightsNotificationCallback") = nullptr,
      nb::arg("onLostCreatorRightsNotificationCallback") = nullptr,
      nb::arg("onLostAdminRightsNotificationCallback") = nullptr,
      nb::arg("onLostAssistantRightsNotificationCallback") = nullptr,
      nb::arg("onLostAttendeeRightsNotificationCallback") = nullptr,
      nb::arg("onLostDataHelperRightsNotificationCallback") = nullptr,
      nb::arg("onNewBroadcastMessageReceivedCallback") = nullptr,
      nb::arg("onBOStopCountDownCallback") = nullptr,
      nb::arg("onHostInviteReturnToMainSessionCallback") = nullptr,
      nb::arg("onBOStatusChangedCallback") = nullptr,
      nb::arg("onBOSwitchRequestReceivedCallback") = nullptr,
      nb::arg("onBroadcastBOVoiceStatusCallback") = nullptr
    );
}
