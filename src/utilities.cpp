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

#include "utilities.h"

namespace nb = nanobind;
using namespace std;
#define PROCESSING_TIME_BIN_COUNT 200

CallbackPerformanceData::CallbackPerformanceData() 
    : totalProcessingTimeMicroseconds(0),
      numCalls(0),
      maxProcessingTimeMicroseconds(0),
      minProcessingTimeMicroseconds(UINT64_MAX),
      processingTimeBinCounts(PROCESSING_TIME_BIN_COUNT, 0),
      processingTimeBinMax(20000),
      processingTimeBinMin(0) {}

CallbackPerformanceData::CallbackPerformanceData(const CallbackPerformanceData& other) 
    : totalProcessingTimeMicroseconds(other.totalProcessingTimeMicroseconds),
      numCalls(other.numCalls),
      maxProcessingTimeMicroseconds(other.maxProcessingTimeMicroseconds),
      minProcessingTimeMicroseconds(other.minProcessingTimeMicroseconds),
      processingTimeBinCounts(other.processingTimeBinCounts),
      processingTimeBinMax(other.processingTimeBinMax),
      processingTimeBinMin(other.processingTimeBinMin) {}

void CallbackPerformanceData::updatePerformanceData(uint64_t processingTimeMicroseconds) {
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

void init_utilities(nb::module_ &m) {
    nb::class_<CallbackPerformanceData>(m, "CallbackPerformanceData")        
        .def_ro("totalProcessingTimeMicroseconds", &CallbackPerformanceData::totalProcessingTimeMicroseconds)
        .def_ro("numCalls", &CallbackPerformanceData::numCalls)
        .def_ro("maxProcessingTimeMicroseconds", &CallbackPerformanceData::maxProcessingTimeMicroseconds)
        .def_ro("minProcessingTimeMicroseconds", &CallbackPerformanceData::minProcessingTimeMicroseconds)
        .def_ro("processingTimeBinCounts", &CallbackPerformanceData::processingTimeBinCounts)
        .def_ro("processingTimeBinMax", &CallbackPerformanceData::processingTimeBinMax)
        .def_ro("processingTimeBinMin", &CallbackPerformanceData::processingTimeBinMin);
};