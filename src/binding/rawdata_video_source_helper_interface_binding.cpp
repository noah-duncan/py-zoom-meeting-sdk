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
#include "rawdata/rawdata_video_source_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

void init_rawdata_video_source_helper_interface_binding(nb::module_ &m) {
    // Bind IZoomSDKPreProcessor
    nb::class_<IZoomSDKPreProcessor>(m, "IZoomSDKPreProcessor")
        .def("onPreProcessRawData", &IZoomSDKPreProcessor::onPreProcessRawData);

    // Bind IZoomSDKVideoSender
    nb::class_<IZoomSDKVideoSender>(m, "IZoomSDKVideoSender")
        .def("sendVideoFrame", [](IZoomSDKVideoSender& self, nb::bytes frameBuffer, int width, int height, int rotation, FrameDataFormat format = FrameDataFormat_I420_FULL) -> SDKError {
            return self.sendVideoFrame((char*)frameBuffer.c_str(), width, height, frameBuffer.size(), rotation, format);
        });

    // Bind VideoSourceCapability
    nb::class_<VideoSourceCapability>(m, "VideoSourceCapability")
        .def(nb::init<>())
        .def(nb::init<unsigned int, unsigned int, unsigned int>())
        .def_rw("width", &VideoSourceCapability::width)
        .def_rw("height", &VideoSourceCapability::height)
        .def_rw("frame", &VideoSourceCapability::frame)
        .def("Reset", &VideoSourceCapability::Reset);

    // Bind IZoomSDKVideoSource
    nb::class_<IZoomSDKVideoSource>(m, "IZoomSDKVideoSource")
        .def("onInitialize", &IZoomSDKVideoSource::onInitialize)
        .def("onPropertyChange", &IZoomSDKVideoSource::onPropertyChange)
        .def("onStartSend", &IZoomSDKVideoSource::onStartSend)
        .def("onStopSend", &IZoomSDKVideoSource::onStopSend)
        .def("onUninitialized", &IZoomSDKVideoSource::onUninitialized);

    // Bind IZoomSDKVideoSourceHelper
    nb::class_<IZoomSDKVideoSourceHelper>(m, "IZoomSDKVideoSourceHelper")
        .def("setPreProcessor", &IZoomSDKVideoSourceHelper::setPreProcessor, nb::call_guard<nb::gil_scoped_release>())
        .def("setExternalVideoSource", &IZoomSDKVideoSourceHelper::setExternalVideoSource, nb::call_guard<nb::gil_scoped_release>());
}