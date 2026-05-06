#include "benchmark/include/timer.hpp"
#include "benchmark/include/memory_tracker.hpp"
#include "benchmark/include/hw_counter.hpp"
#include "benchmark/include/benchmark_runner.hpp"
#include "ecs_entt/include/world.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include "shared/benchmark_config.hpp"
#include "shared/random_utils.hpp"
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <random>

using namespace benchmark;
using namespace ecs_entt;
using namespace shared;

namespace {

std::filesystem::path getProjectResultsDir(const char* executablePath) {
    std::filesystem::path projectDir = std::filesystem::absolute(executablePath).parent_path();

    for (int i = 0; i < 3; ++i) {
        projectDir = projectDir.parent_path();
    }

    return projectDir / "results";
}

} // namespace

void generateRandomPosition(std::mt19937& rng, float& x, float& y, float& z) {
    std::uniform_real_distribution<float> distX(0.0f, shared::WorldConstants::WORLD_SIZE_X);
    std::uniform_real_distribution<float> distY(0.0f, shared::WorldConstants::WORLD_SIZE_Y);
    std::uniform_real_distribution<float> distZ(0.0f, shared::WorldConstants::WORLD_SIZE_Z);

    x = distX(rng);
    y = distY(rng);
    z = distZ(rng);
}

EntityType getRandomEntityType(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float r = dist(rng);

    if (r < 0.30f) {
        std::uniform_int_distribution<int> monsterDist(0, 7);
        return static_cast<EntityType>(monsterDist(rng));
    } else if (r < 0.70f) {
        std::uniform_int_distribution<int> animalDist(8, 15);
        return static_cast<EntityType>(animalDist(rng));
    } else if (r < 0.85f) {
        std::uniform_int_distribution<int> flyingDist(16, 17);
        return static_cast<EntityType>(flyingDist(rng));
    } else {
        std::uniform_int_distribution<int> aquaticDist(18, 21);
        return static_cast<EntityType>(aquaticDist(rng));
    }
}

int main(int argc, char* argv[]) {
    std::cout << "====================================" << std::endl;
    std::cout << "  EnTT ECS Benchmark - ECS vs OOP Performance" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;

    std::filesystem::path outputPath = getProjectResultsDir(argc > 0 ? argv[0] : "");
    if (argc > 1) {
        outputPath = std::filesystem::path(argv[1]);
    }

    std::filesystem::create_directories(outputPath);

    BenchmarkRunner runner(outputPath.string());
    auto configs = getAllBenchmarkConfigs();

    std::vector<std::vector<FrameData>> allFrameData;
    std::vector<MemoryStats> allMemoryStats;
    std::vector<CreationStats> allCreationStats;
    std::vector<std::pair<size_t, HWCounters>> allHWCounters;
    std::vector<size_t> scales;

    for (const auto& config : configs) {
        std::cout << "\n--- Running EnTT ECS Benchmark: " << config.entityCount
                  << " entities, " << config.frameCount << " frames ---" << std::endl;

        std::mt19937 rng(config.randomSeed);
        MemoryTracker::instance().reset();
        shared::seedBenchmarkRng(config.randomSeed);

        World world(config.entityCount);

        std::cout << "Creating " << config.entityCount << " entities..." << std::endl;

        Timer createTimer;
        createTimer.start();

        for (size_t i = 0; i < config.entityCount; ++i) {
            EntityType type = getRandomEntityType(rng);
            float x, y, z;
            generateRandomPosition(rng, x, y, z);
            world.createEntity(type, x, y, z);
        }

        createTimer.stop();
        double createTime = createTimer.elapsedMs();

        std::cout << "Created " << world.getEntityCount() << " entities in "
                  << createTime << " ms" << std::endl;

        auto memData = MemoryTracker::instance().getStats();
        MemoryStats memStats;
        memStats.scale = config.entityCount;
        memStats.totalAllocated = memData.totalAllocated;
        memStats.totalFreed = memData.totalFreed;
        memStats.peakUsage = memData.peakUsage;
        memStats.perEntityOverhead = config.entityCount > 0 ?
                                     memData.peakUsage / config.entityCount : 0;
        allMemoryStats.push_back(memStats);

        std::cout << "Running simulation..." << std::endl;
        std::vector<FrameData> frameData;
        frameData.reserve(config.frameCount);

        const float dt = shared::WorldConstants::FIXED_TIMESTEP;

        for (size_t frame = 0; frame < config.frameCount; ++frame) {
            FrameData data;
            data.frameNumber = frame;

            Timer frameTimer;
            frameTimer.start();

            world.update(dt);

            frameTimer.stop();
            data.frameTimeMs = frameTimer.elapsedMs();
            data.updateTimeMs = data.frameTimeMs;

            frameData.push_back(data);

            if ((frame + 1) % 1000 == 0) {
                std::cout << "  Frame " << (frame + 1) << "/" << config.frameCount << std::endl;
            }
        }

        allFrameData.push_back(frameData);
        scales.push_back(config.entityCount);

        HWCounters hwCounters;
        allHWCounters.emplace_back(config.entityCount, hwCounters);

        Timer destroyTimer;
        destroyTimer.start();

        world.clear();

        destroyTimer.stop();

        CreationStats creationStats;
        creationStats.scale = config.entityCount;
        creationStats.createTimeMs = createTime;
        creationStats.destroyTimeMs = destroyTimer.elapsedMs();
        creationStats.avgCreateTimeMs = createTime / config.entityCount;
        creationStats.avgDestroyTimeMs = creationStats.destroyTimeMs / config.entityCount;
        allCreationStats.push_back(creationStats);

        std::cout << "Benchmark completed for " << config.entityCount << " entities" << std::endl;
    }

    std::cout << "\nWriting results to " << outputPath.string() << "..." << std::endl;
    runner.writeFrameTimeCSV((outputPath / "ecs_entt_frame_time.csv").string(), allFrameData, scales);
    runner.writeMemoryCSV((outputPath / "ecs_entt_memory.csv").string(), allMemoryStats);
    runner.writeCreationCSV((outputPath / "ecs_entt_creation.csv").string(), allCreationStats);
    runner.writeHWCountersCSV((outputPath / "ecs_entt_hw_counters.csv").string(), allHWCounters);

    std::cout << "\n====================================" << std::endl;
    std::cout << "  EnTT ECS Benchmark Complete!" << std::endl;
    std::cout << "====================================" << std::endl;

    return 0;
}
