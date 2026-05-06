#include "benchmark_runner.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace benchmark {

BenchmarkRunner::BenchmarkRunner(const std::string& outputPath)
    : outputPath_(outputPath) {
}

void BenchmarkRunner::runFullBenchmark(const std::vector<TestConfig>& configs) {
    std::vector<std::vector<FrameData>> allFrameData;
    std::vector<MemoryStats> allMemoryStats;
    std::vector<CreationStats> allCreationStats;
    std::vector<std::pair<size_t, HWCounters>> allHWCounters;
    std::vector<size_t> scales;

    for (const auto& config : configs) {
        std::cout << "Running benchmark: " << config.name
                  << " (" << config.entityCount << " entities)" << std::endl;

        // Frame time test
        auto frameData = runFrameTimeTest(config);
        allFrameData.push_back(frameData);
        scales.push_back(config.entityCount);

        // Memory test
        auto memStats = runMemoryTest(config);
        allMemoryStats.push_back(memStats);

        // Creation test
        auto createStats = runCreationTest(config);
        allCreationStats.push_back(createStats);

        // Hardware counters
        auto hwCounters = runHWCounterTest(config);
        allHWCounters.emplace_back(config.entityCount, hwCounters);

        std::cout << "  Completed: " << config.name << std::endl;
    }

    // Write CSV files
    writeFrameTimeCSV(outputPath_ + "/frame_time.csv", allFrameData, scales);
    writeMemoryCSV(outputPath_ + "/memory.csv", allMemoryStats);
    writeCreationCSV(outputPath_ + "/creation.csv", allCreationStats);
    writeHWCountersCSV(outputPath_ + "/hw_counters.csv", allHWCounters);

    std::cout << "Benchmark results written to: " << outputPath_ << std::endl;
}

std::vector<FrameData> BenchmarkRunner::runFrameTimeTest(const TestConfig& config) {
    std::vector<FrameData> results;
    results.reserve(config.frameCount);

    // Setup
    if (setupFunc_) {
        setupFunc_(config.entityCount);
    }

    MemoryTracker::instance().reset();

    const float dt = 1.0f / 60.0f; // 60 FPS fixed timestep

    for (size_t frame = 0; frame < config.frameCount; ++frame) {
        FrameData data;
        data.frameNumber = frame;

        Timer frameTimer;
        frameTimer.start();

        Timer updateTimer;
        updateTimer.start();

        if (updateFunc_) {
            updateFunc_(dt);
        }

        updateTimer.stop();
        data.updateTimeMs = updateTimer.elapsedMs();

        frameTimer.stop();
        data.frameTimeMs = frameTimer.elapsedMs();

        results.push_back(data);
    }

    return results;
}

MemoryStats BenchmarkRunner::runMemoryTest(const TestConfig& config) {
    MemoryStats stats;
    stats.scale = config.entityCount;

    MemoryTracker::instance().reset();

    // Setup entities
    if (setupFunc_) {
        setupFunc_(config.entityCount);
    }

    auto memData = MemoryTracker::instance().getStats();
    stats.totalAllocated = memData.totalAllocated;
    stats.totalFreed = memData.totalFreed;
    stats.peakUsage = memData.peakUsage;
    stats.perEntityOverhead = MemoryTracker::instance().getPerEntityOverhead(config.entityCount);

    return stats;
}

CreationStats BenchmarkRunner::runCreationTest(const TestConfig& config) {
    CreationStats stats;
    stats.scale = config.entityCount;

    // Measure creation time
    Timer timer;

    MemoryTracker::instance().reset();
    timer.start();

    for (size_t i = 0; i < config.entityCount; ++i) {
        if (createEntityFunc_) {
            createEntityFunc_();
        }
    }

    timer.stop();
    stats.createTimeMs = timer.elapsedMs();
    stats.avgCreateTimeMs = stats.createTimeMs / static_cast<double>(config.entityCount);

    // Measure destruction time
    timer.start();

    for (size_t i = 0; i < config.entityCount; ++i) {
        if (destroyEntityFunc_) {
            destroyEntityFunc_();
        }
    }

    timer.stop();
    stats.destroyTimeMs = timer.elapsedMs();
    stats.avgDestroyTimeMs = stats.destroyTimeMs / static_cast<double>(config.entityCount);

    return stats;
}

HWCounters BenchmarkRunner::runHWCounterTest(const TestConfig& config) {
    HWCounter counter;

    if (!counter.isAvailable()) {
        return HWCounters{};
    }

    // Setup
    if (setupFunc_) {
        setupFunc_(config.entityCount);
    }

    counter.reset();
    counter.start();

    const float dt = 1.0f / 60.0f;
    for (size_t frame = 0; frame < config.frameCount; ++frame) {
        if (updateFunc_) {
            updateFunc_(dt);
        }
    }

    counter.stop();
    return counter.getCounters();
}

void BenchmarkRunner::writeFrameTimeCSV(const std::string& filename,
                                         const std::vector<std::vector<FrameData>>& allData,
                                         const std::vector<size_t>& scales) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    // Header
    file << "scale,frame,frame_time_ms,update_time_ms\n";

    // Data
    for (size_t i = 0; i < allData.size(); ++i) {
        const auto& frames = allData[i];
        size_t scale = scales[i];

        for (const auto& frame : frames) {
            file << scale << ","
                 << frame.frameNumber << ","
                 << std::fixed << std::setprecision(6)
                 << frame.frameTimeMs << ","
                 << frame.updateTimeMs << "\n";
        }
    }

    file.close();
}

void BenchmarkRunner::writeMemoryCSV(const std::string& filename,
                                      const std::vector<MemoryStats>& stats) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    file << "scale,total_allocated,total_freed,peak_usage,per_entity_overhead\n";

    for (const auto& s : stats) {
        file << s.scale << ","
             << s.totalAllocated << ","
             << s.totalFreed << ","
             << s.peakUsage << ","
             << s.perEntityOverhead << "\n";
    }

    file.close();
}

void BenchmarkRunner::writeCreationCSV(const std::string& filename,
                                        const std::vector<CreationStats>& stats) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    file << "scale,create_time_ms,destroy_time_ms,avg_create_time_ms,avg_destroy_time_ms\n";

    for (const auto& s : stats) {
        file << s.scale << ","
             << std::fixed << std::setprecision(6)
             << s.createTimeMs << ","
             << s.destroyTimeMs << ","
             << s.avgCreateTimeMs << ","
             << s.avgDestroyTimeMs << "\n";
    }

    file.close();
}

void BenchmarkRunner::writeHWCountersCSV(const std::string& filename,
                                          const std::vector<std::pair<size_t, HWCounters>>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    file << "scale,cache_misses,cache_references,branch_mispredictions,instructions_retired\n";

    for (const auto& [scale, counters] : data) {
        file << scale << ","
             << counters.cacheMisses << ","
             << counters.cacheReferences << ","
             << counters.branchMispredictions << ","
             << counters.instructionsRetired << "\n";
    }

    file.close();
}

} // namespace benchmark
