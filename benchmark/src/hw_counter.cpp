#include "hw_counter.hpp"
#include <iostream>

namespace benchmark {

HWCounter::HWCounter()
    : available_(false)
    , threadHandle_(nullptr) {
#ifdef _WIN32
    available_ = initializeWindowsCounters();
#else
    available_ = initializeLinuxCounters();
#endif
}

HWCounter::~HWCounter() {
#ifdef _WIN32
    if (threadHandle_) {
        CloseHandle(threadHandle_);
    }
#endif
}

void HWCounter::start() {
    if (!available_) return;
#ifdef _WIN32
    readWindowsCounters(startCounters_);
#endif
}

void HWCounter::stop() {
    if (!available_) return;
#ifdef _WIN32
    HWCounters endCounters;
    readWindowsCounters(endCounters);

    counters_.cacheMisses = endCounters.cacheMisses - startCounters_.cacheMisses;
    counters_.cacheReferences = endCounters.cacheReferences - startCounters_.cacheReferences;
    counters_.branchMispredictions = endCounters.branchMispredictions - startCounters_.branchMispredictions;
    counters_.instructionsRetired = endCounters.instructionsRetired - startCounters_.instructionsRetired;
#endif
}

HWCounters HWCounter::getCounters() const {
    return counters_;
}

void HWCounter::reset() {
    counters_ = HWCounters{};
    startCounters_ = HWCounters{};
}

#ifdef _WIN32

bool HWCounter::initializeWindowsCounters() {
    // Use QueryThreadCycleTime for instruction count approximation
    // and use simpler counters since PDH requires complex setup
    threadHandle_ = GetCurrentThread();
    if (!DuplicateHandle(GetCurrentProcess(), threadHandle_,
                         GetCurrentProcess(), &threadHandle_,
                         THREAD_QUERY_INFORMATION, FALSE, 0)) {
        // Fallback - just use the pseudo handle
        threadHandle_ = GetCurrentThread();
    }
    return true;
}

bool HWCounter::readWindowsCounters(HWCounters& out) {
    // Use Windows Performance Counter API
    // For simplicity, we'll use cycle count as a proxy for instructions
    // Real hardware counters would require Intel VTune or similar

    ULONG64 cycleTime = 0;
    if (QueryThreadCycleTime(threadHandle_, &cycleTime)) {
        out.instructionsRetired = cycleTime;
    }

    // For cache and branch counters on Windows, we'd need:
    // 1. Intel VTune API
    // 2. Windows Performance Toolkit (WPT)
    // 3. Or a simplified estimation

    // Simplified approach: use performance data from registry
    // This is a placeholder - real implementation would need
    // proper hardware counter access

    out.cacheMisses = 0;
    out.cacheReferences = 0;
    out.branchMispredictions = 0;

    return true;
}

#else

bool HWCounter::initializeLinuxCounters() {
    // Linux perf events would go here
    // For now, mark as unavailable
    return false;
}

bool HWCounter::readLinuxCounters(HWCounters& out) {
    return false;
}

#endif

ScopedHWCounter::ScopedHWCounter(HWCounters& result)
    : result_(result) {
    counter_.start();
}

ScopedHWCounter::~ScopedHWCounter() {
    counter_.stop();
    result_ = counter_.getCounters();
}

} // namespace benchmark
