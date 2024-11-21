#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>

#include "zoom_sdk.h"

#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "rawdata/rawdata_renderer_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "zoom_sdk_def.h"

#include <iostream>
#include <functional>
#include <memory>

#include "utilities.h"

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
    bool m_collectPerformanceData;
    CallbackPerformanceData m_performanceData;

public:
    ZoomSDKRendererDelegateCallbacks(
        const function<void()>& onRendererBeDestroyedCallback = nullptr,
        const function<void(YUVRawDataI420*)>& onRawDataFrameReceivedCallback = nullptr,
        const function<void(RawDataStatus)>& onRawDataStatusChangedCallback = nullptr,
        bool collectPerformanceData = false
    ) : m_onRendererBeDestroyedCallback(onRendererBeDestroyedCallback),
        m_onRawDataFrameReceivedCallback(onRawDataFrameReceivedCallback),
        m_onRawDataStatusChangedCallback(onRawDataStatusChangedCallback),
        m_collectPerformanceData(collectPerformanceData) {}

    void onRendererBeDestroyed() override {
        if (m_onRendererBeDestroyedCallback)
            m_onRendererBeDestroyedCallback();
    }

    void onRawDataFrameReceived(YUVRawDataI420* data) override {
        if (m_collectPerformanceData) {
            auto start = std::chrono::high_resolution_clock::now();
            m_onRawDataFrameReceivedCallback(data);
            auto end = std::chrono::high_resolution_clock::now();
            uint64_t processingTimeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            m_performanceData.updatePerformanceData(processingTimeMicroseconds);
        }
        else
            m_onRawDataFrameReceivedCallback(data);
    }

    void onRawDataStatusChanged(RawDataStatus status) override {
        if (m_onRawDataStatusChangedCallback)
            m_onRawDataStatusChangedCallback(status);
    }

    const CallbackPerformanceData & getPerformanceData() const {
        return m_performanceData;
    }
};

void init_zoom_sdk_renderer_delegate_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKRendererDelegateCallbacks, IZoomSDKRendererDelegate>(m, "ZoomSDKRendererDelegateCallbacks")
        .def(nb::init<
            const function<void()>&,
            const function<void(YUVRawDataI420*)>&,
            const function<void(IZoomSDKRendererDelegate::RawDataStatus)>&,
            bool
        >(),
        nb::arg("onRendererBeDestroyedCallback") = nullptr,
        nb::arg("onRawDataFrameReceivedCallback") = nullptr,
        nb::arg("onRawDataStatusChangedCallback") = nullptr,
        nb::arg("collectPerformanceData") = false
    )
    .def("getPerformanceData", &ZoomSDKRendererDelegateCallbacks::getPerformanceData);
}