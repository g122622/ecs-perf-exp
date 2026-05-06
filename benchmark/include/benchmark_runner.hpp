#pragma once

#include "timer.hpp"
#include "memory_tracker.hpp"
#include "hw_counter.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <fstream>

namespace benchmark {

struct FrameData {
    uint64_t frameNumber;
    double frameTimeMs;
    double updateTimeMs;
};

struct MemoryStats {
    size_t scale;              // 实体数量
    size_t totalAllocated;
    size_t totalFreed;
    size_t peakUsage;
    size_t perEntityOverhead;
};

struct CreationStats {
    size_t scale;
    double createTimeMs;
    double destroyTimeMs;
    double avgCreateTimeMs;
    double avgDestroyTimeMs;
};

struct TestConfig {
    std::string name;
    size_t entityCount;
    size_t frameCount;
    unsigned int randomSeed;
};

class BenchmarkRunner {
public:
    using UpdateFunc = std::function<void(float dt)>;
    using CreateEntityFunc = std::function<void()>;
    using DestroyEntityFunc = std::function<void()>;
    using SetupFunc = std::function<void(size_t entityCount)>;

    BenchmarkRunner(const std::string& outputPath);

    void setSetupFunc(SetupFunc func) { setupFunc_ = std::move(func); }
    void setUpdateFunc(UpdateFunc func) { updateFunc_ = std::move(func); }
    void setCreateEntityFunc(CreateEntityFunc func) { createEntityFunc_ = std::move(func); }
    void setDestroyEntityFunc(DestroyEntityFunc func) { destroyEntityFunc_ = std::move(func); }

    // Run full benchmark suite
    void runFullBenchmark(const std::vector<TestConfig>& configs);

    // Individual benchmark functions
    std::vector<FrameData> runFrameTimeTest(const TestConfig& config);
    MemoryStats runMemoryTest(const TestConfig& config);
    CreationStats runCreationTest(const TestConfig& config);
    HWCounters runHWCounterTest(const TestConfig& config);

    // CSV output functions
    void writeFrameTimeCSV(const std::string& filename, const std::vector<std::vector<FrameData>>& allData, const std::vector<size_t>& scales);
    void writeMemoryCSV(const std::string& filename, const std::vector<MemoryStats>& stats);
    void writeCreationCSV(const std::string& filename, const std::vector<CreationStats>& stats);
    void writeHWCountersCSV(const std::string& filename, const std::vector<std::pair<size_t, HWCounters>>& data);

private:
    std::string outputPath_;
    SetupFunc setupFunc_;
    UpdateFunc updateFunc_;
    CreateEntityFunc createEntityFunc_;
    DestroyEntityFunc destroyEntityFunc_;
};

} // namespace benchmark
