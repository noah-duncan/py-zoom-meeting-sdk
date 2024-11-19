#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"


#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"

#include <iostream>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

/*
class IZoomSDKAudioRawDataDelegate
{
public:
	~IZoomSDKAudioRawDataDelegate(){}
	virtual void onMixedAudioRawDataReceived(AudioRawData* data_) = 0;
	virtual void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) = 0;
	virtual void onShareAudioRawDataReceived(AudioRawData* data_) = 0;

	/// \brief Invoked when individual interpreter's raw audio data received
	/// \param data_ Raw audio data, see \link AudioRawData \endlink.
	/// \param pLanguageName The pointer to interpreter language name.
	virtual void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) = 0;
};
*/

#define PROCESSING_TIME_BIN_COUNT 200

struct CallbackPerformanceData {
    uint64_t totalProcessingTimeMicroseconds;
    uint64_t numCalls;
    uint64_t maxProcessingTimeMicroseconds = 0;
    uint64_t minProcessingTimeMicroseconds = UINT64_MAX;
    std::vector<uint64_t> processingTimeBinCounts;
    uint64_t processingTimeBinMax = 20000;
    uint64_t processingTimeBinMin = 0;
    std::mutex lock;

    CallbackPerformanceData() : processingTimeBinCounts(PROCESSING_TIME_BIN_COUNT, 0) {}

    CallbackPerformanceData(const CallbackPerformanceData& other) 
        : totalProcessingTimeMicroseconds(other.totalProcessingTimeMicroseconds),
          numCalls(other.numCalls),
          maxProcessingTimeMicroseconds(other.maxProcessingTimeMicroseconds),
          minProcessingTimeMicroseconds(other.minProcessingTimeMicroseconds),
          processingTimeBinCounts(other.processingTimeBinCounts),
          processingTimeBinMax(other.processingTimeBinMax),
          processingTimeBinMin(other.processingTimeBinMin) {
    }

    void updatePerformanceData(uint64_t processingTimeMicroseconds) {
        std::lock_guard<std::mutex> lockGuard(lock);
        totalProcessingTimeMicroseconds += processingTimeMicroseconds;
        numCalls++;
        int binIndex = ((processingTimeMicroseconds - processingTimeBinMin) * PROCESSING_TIME_BIN_COUNT) / (processingTimeBinMax - processingTimeBinMin);
        if (binIndex >= PROCESSING_TIME_BIN_COUNT)
            binIndex = PROCESSING_TIME_BIN_COUNT - 1;
        if (binIndex < 0)
            binIndex = 0;
        processingTimeBinCounts[binIndex]++;
        if (processingTimeMicroseconds > maxProcessingTimeMicroseconds)
            maxProcessingTimeMicroseconds = processingTimeMicroseconds;
        if (processingTimeMicroseconds < minProcessingTimeMicroseconds)
            minProcessingTimeMicroseconds = processingTimeMicroseconds;
    }

    
};

class ZoomSDKAudioRawDataDelegateCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    function<void(AudioRawData*)> m_onMixedAudioRawDataReceivedCallback;
    function<void(AudioRawData*, uint32_t)> m_onOneWayAudioRawDataReceivedCallback;
    function<void(AudioRawData*)> m_onShareAudioRawDataReceivedCallback;
    function<void(AudioRawData*, const zchar_t*)> m_onOneWayInterpreterAudioRawDataReceivedCallback;
    bool m_collectPerformanceData;
    CallbackPerformanceData m_performanceData;
public:
    ZoomSDKAudioRawDataDelegateCallbacks(
        const function<void(AudioRawData*)>& onMixedAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, uint32_t)>& onOneWayAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*)>& onShareAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, const zchar_t*)>& onOneWayInterpreterAudioRawDataReceivedCallback = nullptr,
        bool collectPerformanceData = false
    ) : m_onMixedAudioRawDataReceivedCallback(onMixedAudioRawDataReceivedCallback),
        m_onOneWayAudioRawDataReceivedCallback(onOneWayAudioRawDataReceivedCallback),
        m_onShareAudioRawDataReceivedCallback(onShareAudioRawDataReceivedCallback),
        m_onOneWayInterpreterAudioRawDataReceivedCallback(onOneWayInterpreterAudioRawDataReceivedCallback),
        m_collectPerformanceData(collectPerformanceData) {}

    void onMixedAudioRawDataReceived(AudioRawData* data_) override {
        if (m_onMixedAudioRawDataReceivedCallback)
            m_onMixedAudioRawDataReceivedCallback(data_);
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) override {
        if (m_onOneWayAudioRawDataReceivedCallback)
        {
            if (m_collectPerformanceData) {
                auto start = std::chrono::high_resolution_clock::now();
                m_onOneWayAudioRawDataReceivedCallback(data_, node_id);
                auto end = std::chrono::high_resolution_clock::now();
                uint64_t processingTimeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                m_performanceData.updatePerformanceData(processingTimeMicroseconds);
            }
            else
                m_onOneWayAudioRawDataReceivedCallback(data_, node_id);
        }
    }

    void onShareAudioRawDataReceived(AudioRawData* data_) override {
        if (m_onShareAudioRawDataReceivedCallback)
            m_onShareAudioRawDataReceivedCallback(data_);
    }

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {
        if (m_onOneWayInterpreterAudioRawDataReceivedCallback)
            m_onOneWayInterpreterAudioRawDataReceivedCallback(data_, pLanguageName);
    }

    const CallbackPerformanceData & getPerformanceData() {
        return m_performanceData;
    }
};

void init_zoom_sdk_audio_raw_data_delegate_callbacks(nb::module_ &m) {

    nb::class_<CallbackPerformanceData>(m, "CallbackPerformanceData")
        .def_ro("totalProcessingTimeMicroseconds", &CallbackPerformanceData::totalProcessingTimeMicroseconds)
        .def_ro("numCalls", &CallbackPerformanceData::numCalls)
        .def_ro("maxProcessingTimeMicroseconds", &CallbackPerformanceData::maxProcessingTimeMicroseconds)
        .def_ro("minProcessingTimeMicroseconds", &CallbackPerformanceData::minProcessingTimeMicroseconds)
        .def_ro("processingTimeBinCounts", &CallbackPerformanceData::processingTimeBinCounts)
        .def_ro("processingTimeBinMax", &CallbackPerformanceData::processingTimeBinMax)
        .def_ro("processingTimeBinMin", &CallbackPerformanceData::processingTimeBinMin);

    nb::class_<ZoomSDKAudioRawDataDelegateCallbacks, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegateCallbacks")
        .def(nb::init<
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, uint32_t)>&,
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, const zchar_t*)>&,
            bool
        >(),
        nb::arg("onMixedAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onShareAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayInterpreterAudioRawDataReceivedCallback") = nullptr,
        nb::arg("collectPerformanceData") = false
    )
    .def("getPerformanceData", &ZoomSDKAudioRawDataDelegateCallbacks::getPerformanceData);
}
