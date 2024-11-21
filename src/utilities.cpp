#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/vector.h>

#include <iostream>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace nb = nanobind;
using namespace std;
#define PROCESSING_TIME_BIN_COUNT 200

struct CallbackPerformanceData {
    uint64_t totalProcessingTimeMicroseconds = 0;
    uint64_t numCalls = 0;
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

void init_utilities(nb::module_ &m) {
    nb::class_<CallbackPerformanceData>(m, "CallbackPerformanceData")
        .def(nb::init<>())
        .def_ro("totalProcessingTimeMicroseconds", &CallbackPerformanceData::totalProcessingTimeMicroseconds)
        .def_ro("numCalls", &CallbackPerformanceData::numCalls)
        .def_ro("maxProcessingTimeMicroseconds", &CallbackPerformanceData::maxProcessingTimeMicroseconds)
        .def_ro("minProcessingTimeMicroseconds", &CallbackPerformanceData::minProcessingTimeMicroseconds)
        .def_ro("processingTimeBinCounts", &CallbackPerformanceData::processingTimeBinCounts)
        .def_ro("processingTimeBinMax", &CallbackPerformanceData::processingTimeBinMax)
        .def_ro("processingTimeBinMin", &CallbackPerformanceData::processingTimeBinMin)
        .def("updatePerformanceData", &CallbackPerformanceData::updatePerformanceData);
};