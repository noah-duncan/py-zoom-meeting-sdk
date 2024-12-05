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
using namespace ZOOMSDK;

/*
typedef enum ZoomSDKResolution
{
	ZoomSDKResolution_90P = 0,
	ZoomSDKResolution_180P,
	ZoomSDKResolution_360P,
	ZoomSDKResolution_720P,
	ZoomSDKResolution_1080P,
	ZoomSDKResolution_NoUse = 100
}ZoomSDKResolution;

typedef enum
{
	RAW_DATA_TYPE_VIDEO = 0,
	RAW_DATA_TYPE_SHARE,
}ZoomSDKRawDataType;

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

class IZoomSDKRenderer
{
public:
	virtual SDKError setRawDataResolution(ZoomSDKResolution resolution) = 0;
	virtual SDKError subscribe(uint32_t userId, ZoomSDKRawDataType type) = 0;
	virtual SDKError unSubscribe() = 0;
	virtual ZoomSDKResolution getResolution() = 0;
	virtual ZoomSDKRawDataType getRawDataType() = 0;
	virtual uint32_t getUserId() = 0;
	virtual ~IZoomSDKRenderer(){}
};
*/

void init_rawdata_renderer_interface_binding(nb::module_ &m) {
    nb::enum_<ZoomSDKResolution>(m, "ZoomSDKResolution")
        .value("ZoomSDKResolution_90P", ZoomSDKResolution_90P)
        .value("ZoomSDKResolution_180P", ZoomSDKResolution_180P)
        .value("ZoomSDKResolution_360P", ZoomSDKResolution_360P)
        .value("ZoomSDKResolution_720P", ZoomSDKResolution_720P)
        .value("ZoomSDKResolution_1080P", ZoomSDKResolution_1080P)
        .value("ZoomSDKResolution_NoUse", ZoomSDKResolution_NoUse)
        .export_values();

    nb::enum_<ZoomSDKRawDataType>(m, "ZoomSDKRawDataType")
        .value("RAW_DATA_TYPE_VIDEO", RAW_DATA_TYPE_VIDEO)
        .value("RAW_DATA_TYPE_SHARE", RAW_DATA_TYPE_SHARE)
        .export_values();

    nb::enum_<IZoomSDKRendererDelegate::RawDataStatus>(m, "RawDataStatus")
        .value("RawData_On", IZoomSDKRendererDelegate::RawData_On)
        .value("RawData_Off", IZoomSDKRendererDelegate::RawData_Off)
        .export_values();

    nb::class_<IZoomSDKRendererDelegate>(m, "IZoomSDKRendererDelegate")
        .def("onRendererBeDestroyed", &IZoomSDKRendererDelegate::onRendererBeDestroyed)
        .def("onRawDataFrameReceived", &IZoomSDKRendererDelegate::onRawDataFrameReceived)
        .def("onRawDataStatusChanged", &IZoomSDKRendererDelegate::onRawDataStatusChanged);

    nb::class_<IZoomSDKRenderer>(m, "IZoomSDKRenderer")
        .def("setRawDataResolution", &IZoomSDKRenderer::setRawDataResolution)
        .def("subscribe", &IZoomSDKRenderer::subscribe, nb::call_guard<nb::gil_scoped_release>())
        .def("unSubscribe", &IZoomSDKRenderer::unSubscribe, nb::call_guard<nb::gil_scoped_release>())
        .def("getResolution", &IZoomSDKRenderer::getResolution)
        .def("getRawDataType", &IZoomSDKRenderer::getRawDataType)
        .def("getUserId", &IZoomSDKRenderer::getUserId);     
}