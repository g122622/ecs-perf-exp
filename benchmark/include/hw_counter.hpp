#pragma once

#include <cstdint>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#endif

namespace benchmark {

struct HWCounters {
    uint64_t cacheMisses = 0;
    uint64_t cacheReferences = 0;
    uint64_t branchMispredictions = 0;
    uint64_t instructionsRetired = 0;

    double cacheMissRate() const {
        if (cacheReferences == 0) return 0.0;
        return static_cast<double>(cacheMisses) / static_cast<double>(cacheReferences);
    }

    double branchMispredictionRate() const {
        // Note: We can't easily get total branches, so this is a placeholder
        return static_cast<double>(branchMispredictions);
    }
};

class HWCounter {
public:
    HWCounter();
    ~HWCounter();

    void start();
    void stop();
    HWCounters getCounters() const;
    void reset();

    bool isAvailable() const { return available_; }

private:
    bool available_;
    HWCounters counters_;
    HWCounters startCounters_;

#ifdef _WIN32
    // Windows Performance Counters
    HANDLE threadHandle_;
    bool initializeWindowsCounters();
    bool readWindowsCounters(HWCounters& out);
#else
    bool initializeLinuxCounters();
    bool readLinuxCounters(HWCounters& out);
#endif
};

class ScopedHWCounter {
public:
    explicit ScopedHWCounter(HWCounters& result);
    ~ScopedHWCounter();

private:
    HWCounter counter_;
    HWCounters& result_;
};

} // namespace benchmark
