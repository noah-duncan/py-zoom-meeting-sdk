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
#include <vector>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <chrono>

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

class ZoomSDKAudioRawDataDelegateCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    function<void(AudioRawData*)> m_onMixedAudioRawDataReceivedCallback;
    function<void(AudioRawData*, uint32_t)> m_onOneWayAudioRawDataReceivedCallback;
    function<void(AudioRawData*)> m_onShareAudioRawDataReceivedCallback;
    function<void(AudioRawData*, const zchar_t*)> m_onOneWayInterpreterAudioRawDataReceivedCallback;
    std::chrono::microseconds totalProcessingTime{0};  // Track total processing time
    long numOnOneWayAudioRawDataReceivedCalls{0};
    std::mutex m_metricsLock;
public:
    ZoomSDKAudioRawDataDelegateCallbacks(
        const function<void(AudioRawData*)>& onMixedAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, uint32_t)>& onOneWayAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*)>& onShareAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, const zchar_t*)>& onOneWayInterpreterAudioRawDataReceivedCallback = nullptr
    ) : m_onMixedAudioRawDataReceivedCallback(onMixedAudioRawDataReceivedCallback),
        m_onOneWayAudioRawDataReceivedCallback(onOneWayAudioRawDataReceivedCallback),
        m_onShareAudioRawDataReceivedCallback(onShareAudioRawDataReceivedCallback),
        m_onOneWayInterpreterAudioRawDataReceivedCallback(onOneWayInterpreterAudioRawDataReceivedCallback) {}

    void onMixedAudioRawDataReceived(AudioRawData* data_) override {
        if (m_onMixedAudioRawDataReceivedCallback)
            m_onMixedAudioRawDataReceivedCallback(data_);
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) override {
        auto start = std::chrono::high_resolution_clock::now();
        if (m_onOneWayAudioRawDataReceivedCallback)
            m_onOneWayAudioRawDataReceivedCallback(data_, node_id);
        auto end = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> metricsLock(m_metricsLock);
            totalProcessingTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            numOnOneWayAudioRawDataReceivedCalls++;
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

    long getTotalProcessingTimeMicroseconds() {
        return totalProcessingTime.count();
    }

    long getNumOnOneWayAudioRawDataReceivedCalls() {
        return numOnOneWayAudioRawDataReceivedCalls;
    }
};

class ZoomSDKAudioRawDataDelegateBuffered : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    struct BufferedAudioData {
        std::vector<char> buffer;
        unsigned int sampleRate;
        unsigned int channelNum;
        uint32_t nodeId;
        std::mutex mutex;  // Per-node mutex
    };

    std::mutex m_metricsLock;
    std::chrono::microseconds totalProcessingTime{0};  // Track total processing time
    long numOnOneWayAudioRawDataReceivedCalls{0};
    std::map<uint32_t, std::shared_ptr<BufferedAudioData>> m_audioBuffer;
    std::mutex m_mapMutex;  // Only needed when modifying the map structure

public:
    ZoomSDKAudioRawDataDelegateBuffered() {}

    void onMixedAudioRawDataReceived(AudioRawData* data_) override {}
    
    void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) override {
        if (!data_) return;
        
        auto start = std::chrono::high_resolution_clock::now();
        std::shared_ptr<BufferedAudioData> bufferData;
        
        // Quick check without lock first
        {
            auto it = m_audioBuffer.find(node_id);
            if (it != m_audioBuffer.end()) {
                bufferData = it->second;
            } else {
                std::lock_guard<std::mutex> mapLock(m_mapMutex);
                bufferData = m_audioBuffer[node_id] = std::make_shared<BufferedAudioData>();
                bufferData->nodeId = node_id;
                bufferData->sampleRate = data_->GetSampleRate();
                bufferData->channelNum = data_->GetChannelNum();
                // Pre-reserve 1MB of space to reduce reallocations
                bufferData->buffer.reserve(1024 * 1024);
            }
        }
        
        // Lock only this node's data
        //std::lock_guard<std::mutex> nodeLock(bufferData->mutex);
        const char* newData = data_->GetBuffer();
        size_t newDataLen = data_->GetBufferLen();
        // Reserve additional space if needed
        if (bufferData->buffer.capacity() < bufferData->buffer.size() + newDataLen) {
            bufferData->buffer.reserve(bufferData->buffer.size() + newDataLen + (1024 * 1024));
        }
        bufferData->buffer.insert(bufferData->buffer.end(), newData, newData + newDataLen);
        
        auto end = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> metricsLock(m_metricsLock);
            totalProcessingTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            numOnOneWayAudioRawDataReceivedCalls++;
        }
    }

    void onShareAudioRawDataReceived(AudioRawData* data_) override {}
    
    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {}



    nb::bytes getBufferData(uint32_t node_id) {
        std::cout << "gzetBufferData node_id = " << node_id << std::endl;
        size_t length = 0;
        auto it = m_audioBuffer.find(node_id);
        if (it != m_audioBuffer.end()) {
            std::lock_guard<std::mutex> nodeLock(it->second->mutex);
            length = it->second->buffer.size();
            std::cout << "getBufferData node_id = " << node_id << " length = " << length << std::endl;
            if (length == 0) return nb::bytes();
            
            // Create a copy of the buffer data
            char* data = new char[length];
            memcpy(data, it->second->buffer.data(), length);
            return nb::bytes(data, length);
        }
        length = 0;
        std::cout << "getBufferData node_id = " << node_id << " length = " << length << std::endl;
        return nb::bytes();
    }

    void clearBuffer(uint32_t node_id) {
        auto it = m_audioBuffer.find(node_id);
        if (it != m_audioBuffer.end()) {
            std::lock_guard<std::mutex> nodeLock(it->second->mutex);
            it->second->buffer.clear();
        }
    }

    std::vector<uint32_t> getActiveNodes() {
        std::vector<uint32_t> nodes;
        nodes.reserve(m_audioBuffer.size());
        for (const auto& pair : m_audioBuffer) {
            nodes.push_back(pair.first);
        }
        return nodes;
    }

    long getTotalProcessingTimeMicroseconds() {
        return totalProcessingTime.count();
    }

    long getNumOnOneWayAudioRawDataReceivedCalls() {
        return numOnOneWayAudioRawDataReceivedCalls;
    }
};

void init_zoom_sdk_audio_raw_data_delegate_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKAudioRawDataDelegateCallbacks, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegateCallbacks")
        .def(nb::init<
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, uint32_t)>&,
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, const zchar_t*)>&
        >(),
        nb::arg("onMixedAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onShareAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayInterpreterAudioRawDataReceivedCallback") = nullptr
    )
    .def("getTotalProcessingTimeMicroseconds", &ZoomSDKAudioRawDataDelegateCallbacks::getTotalProcessingTimeMicroseconds)
    .def("getNumOnOneWayAudioRawDataReceivedCalls", &ZoomSDKAudioRawDataDelegateCallbacks::getNumOnOneWayAudioRawDataReceivedCalls);

    nb::class_<ZoomSDKAudioRawDataDelegateBuffered, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegateBuffered")
        .def(nb::init<>())
        .def("getBufferData", &ZoomSDKAudioRawDataDelegateBuffered::getBufferData)
        .def("clearBuffer", &ZoomSDKAudioRawDataDelegateBuffered::clearBuffer)
        .def("getActiveNodes", &ZoomSDKAudioRawDataDelegateBuffered::getActiveNodes)
        .def("getTotalProcessingTimeMicroseconds", &ZoomSDKAudioRawDataDelegateBuffered::getTotalProcessingTimeMicroseconds)
        .def("getNumOnOneWayAudioRawDataReceivedCalls", &ZoomSDKAudioRawDataDelegateBuffered::getNumOnOneWayAudioRawDataReceivedCalls);
}