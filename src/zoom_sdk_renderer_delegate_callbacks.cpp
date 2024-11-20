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
#include "rawdata/rawdata_renderer_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "zoom_sdk_def.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

/*
class IZoomSDKRendererDelegate
{
public:
	enum RawDataStatus
	{
		RawData_On,
		RawData_Off,
	};
	/// \brief Notify the current renderer object is going to be destroyed. 
	/// After you handle this callback, you should never user this renderer object any more.
	virtual void onRendererBeDestroyed() = 0;

	virtual void onRawDataFrameReceived(YUVRawDataI420* data) = 0;
	virtual void onRawDataStatusChanged(RawDataStatus status) = 0;
	virtual ~IZoomSDKRendererDelegate() {}
};
*/

class ZoomSDKRendererDelegateCallbacks : public IZoomSDKRendererDelegate {
private:
    function<void()> m_onRendererBeDestroyedCallback;
    function<void(YUVRawDataI420*)> m_onRawDataFrameReceivedCallback;
    function<void(RawDataStatus)> m_onRawDataStatusChangedCallback;

public:
    ZoomSDKRendererDelegateCallbacks(
        const function<void()>& onRendererBeDestroyedCallback = nullptr,
        const function<void(YUVRawDataI420*)>& onRawDataFrameReceivedCallback = nullptr,
        const function<void(RawDataStatus)>& onRawDataStatusChangedCallback = nullptr
    ) : m_onRendererBeDestroyedCallback(onRendererBeDestroyedCallback),
        m_onRawDataFrameReceivedCallback(onRawDataFrameReceivedCallback),
        m_onRawDataStatusChangedCallback(onRawDataStatusChangedCallback) {}

    void onRendererBeDestroyed() override {
        if (m_onRendererBeDestroyedCallback)
            m_onRendererBeDestroyedCallback();
    }

    void onRawDataFrameReceived(YUVRawDataI420* data) override {
        if (m_onRawDataFrameReceivedCallback)
            m_onRawDataFrameReceivedCallback(data);
    }

    void onRawDataStatusChanged(RawDataStatus status) override {
        if (m_onRawDataStatusChangedCallback)
            m_onRawDataStatusChangedCallback(status);
    }
};

void init_zoom_sdk_renderer_delegate_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKRendererDelegateCallbacks, IZoomSDKRendererDelegate>(m, "ZoomSDKRendererDelegateCallbacks")
        .def(nb::init<
            const function<void()>&,
            const function<void(YUVRawDataI420*)>&,
            const function<void(IZoomSDKRendererDelegate::RawDataStatus)>&
        >(),
        nb::arg("onRendererBeDestroyedCallback") = nullptr,
        nb::arg("onRawDataFrameReceivedCallback") = nullptr,
        nb::arg("onRawDataStatusChangedCallback") = nullptr
    );    
}