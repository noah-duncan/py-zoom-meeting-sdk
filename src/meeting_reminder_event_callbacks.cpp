#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"

#include "meeting_service_components/meeting_reminder_ctrl_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

/*
class IMeetingReminderEvent
{
public:
	virtual ~IMeetingReminderEvent() {}

	/// \brief Callback event of the reminder dialog show.
	/// \param content The detail content in the reminder dialog. For more details, see \link IMeetingReminderContent \endlink enum.
	/// \param handle A pointer to the IMeetingReminderHandler. For more details, see \link IMeetingReminderHandler \endlink.
	virtual void onReminderNotify(IMeetingReminderContent* content, IMeetingReminderHandler* handle) = 0;

	/// \brief Callback event of the enable reminder dialog show.
	/// \param content The detail content in the reminder dialog. For more details, see \link IMeetingEnableReminderHandler \endlink enum.
	/// \param handle A pointer to the IMeetingReminderHandler. For more details, see \link IMeetingEnableReminderHandler \endlink.
	virtual void onEnableReminderNotify(IMeetingReminderContent* content, IMeetingEnableReminderHandler* handle) = 0;
};
*/

class MeetingReminderEventCallbacks : public ZOOM_SDK_NAMESPACE::IMeetingReminderEvent {
private:
    function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingReminderHandler*)> m_onReminderNotifyCallback;
    function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingEnableReminderHandler*)> m_onEnableReminderNotifyCallback;

public:
    MeetingReminderEventCallbacks(
        const function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingReminderHandler*)>& onReminderNotifyCallback = nullptr,
        const function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingEnableReminderHandler*)>& onEnableReminderNotifyCallback = nullptr
    ) : m_onReminderNotifyCallback(onReminderNotifyCallback),
        m_onEnableReminderNotifyCallback(onEnableReminderNotifyCallback) {}

    void onReminderNotify(ZOOM_SDK_NAMESPACE::IMeetingReminderContent* content, ZOOM_SDK_NAMESPACE::IMeetingReminderHandler* handle) override {
        if (m_onReminderNotifyCallback)
            m_onReminderNotifyCallback(content, handle);
    }

    void onEnableReminderNotify(ZOOM_SDK_NAMESPACE::IMeetingReminderContent* content, ZOOM_SDK_NAMESPACE::IMeetingEnableReminderHandler* handle) override {
        if (m_onEnableReminderNotifyCallback)
            m_onEnableReminderNotifyCallback(content, handle);
    }
};

void init_meeting_reminder_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingReminderEventCallbacks, ZOOM_SDK_NAMESPACE::IMeetingReminderEvent>(m, "MeetingReminderEventCallbacks")
        .def(
            nb::init<
                function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingReminderHandler*)>&,
                function<void(ZOOM_SDK_NAMESPACE::IMeetingReminderContent*, ZOOM_SDK_NAMESPACE::IMeetingEnableReminderHandler*)>&
            >(),
            nb::arg("onReminderNotifyCallback") = nullptr,
            nb::arg("onEnableReminderNotifyCallback") = nullptr
        );
}