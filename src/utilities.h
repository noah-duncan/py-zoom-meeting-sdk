#ifndef UTILITIES_H
#define UTILITIES_H

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

struct CallbackPerformanceData {
    uint64_t totalProcessingTimeMicroseconds;
    uint64_t numCalls;
    uint64_t maxProcessingTimeMicroseconds;
    uint64_t minProcessingTimeMicroseconds;
    std::vector<uint64_t> processingTimeBinCounts;
    uint64_t processingTimeBinMax;
    uint64_t processingTimeBinMin;
    std::mutex lock;

    CallbackPerformanceData();
    CallbackPerformanceData(const CallbackPerformanceData& other);
    void updatePerformanceData(uint64_t processingTimeMicroseconds);
};

#endif