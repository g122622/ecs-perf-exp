#include "benchmark/include/timer.hpp"
#include "benchmark/include/memory_tracker.hpp"
#include "benchmark/include/hw_counter.hpp"
#include "benchmark/include/benchmark_runner.hpp"
#include "oop/include/world.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include "shared/benchmark_config.hpp"
#include <iostream>
#include <iomanip>
#include <random>

using namespace benchmark;
using namespace oop;
using namespace shared;

// 生成随机位置
void generateRandomPosition(std::mt19937& rng, float& x, float& y, float& z) {
    std::uniform_real_distribution<float> distX(0.0f, shared::WorldConstants::WORLD_SIZE_X);
    std::uniform_real_distribution<float> distY(0.0f, shared::WorldConstants::WORLD_SIZE_Y);
    std::uniform_real_distribution<float> distZ(0.0f, shared::WorldConstants::WORLD_SIZE_Z);

    x = distX(rng);
    y = distY(rng);
    z = distZ(rng);
}

// 按比例生成随机实体类型
EntityType getRandomEntityType(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float r = dist(rng);

    // Monster 30%, Animal 40%, Flying 15%, Aquatic 15%
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
    std::cout << "  OOP Benchmark - ECS vs OOP Performance" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;

    // 配置
    std::string outputPath = "./results";
    if (argc > 1) {
        outputPath = argv[1];
    }

    // 创建benchmark runner
    BenchmarkRunner runner(outputPath);

    // 测试配置
    auto configs = getAllBenchmarkConfigs();

    // 收集结果
    std::vector<std::vector<FrameData>> allFrameData;
    std::vector<MemoryStats> allMemoryStats;
    std::vector<CreationStats> allCreationStats;
    std::vector<std::pair<size_t, HWCounters>> allHWCounters;
    std::vector<size_t> scales;

    for (const auto& config : configs) {
        std::cout << "\n--- Running OOP Benchmark: " << config.entityCount
                  << " entities, " << config.frameCount << " frames ---" << std::endl;

        // 初始化随机数生成器
        std::mt19937 rng(config.randomSeed);
        MemoryTracker::instance().reset();

        // 创建世界
        World world;
        world.getRNG().seed(config.randomSeed);

        // 生成实体
        std::cout << "Creating " << config.entityCount << " entities..." << std::endl;

        Timer createTimer;
        createTimer.start();

        for (size_t i = 0; i < config.entityCount; ++i) {
            EntityType type = getRandomEntityType(rng);
            float x, y, z;
            generateRandomPosition(rng, x, y, z);
            world.spawnEntity(type, x, y, z);
        }

        createTimer.stop();
        double createTime = createTimer.elapsedMs();

        std::cout << "Created " << world.getEntityCount() << " entities in "
                  << createTime << " ms" << std::endl;

        // 记录内存
        auto memData = MemoryTracker::instance().getStats();
        MemoryStats memStats;
        memStats.scale = config.entityCount;
        memStats.totalAllocated = memData.totalAllocated;
        memStats.totalFreed = memData.totalFreed;
        memStats.peakUsage = memData.peakUsage;
        memStats.perEntityOverhead = config.entityCount > 0 ?
                                     memData.peakUsage / config.entityCount : 0;
        allMemoryStats.push_back(memStats);

        // 运行帧时间测试
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
            data.updateTimeMs = data.frameTimeMs;  // OOP没有单独测量

            frameData.push_back(data);

            // 进度显示
            if ((frame + 1) % 1000 == 0) {
                std::cout << "  Frame " << (frame + 1) << "/" << config.frameCount << std::endl;
            }
        }

        allFrameData.push_back(frameData);
        scales.push_back(config.entityCount);

        // 硬件计数器测试 (简化版)
        HWCounters hwCounters;
        allHWCounters.emplace_back(config.entityCount, hwCounters);

        // 销毁测试
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

    // 写入CSV文件
    std::cout << "\nWriting results to " << outputPath << "..." << std::endl;
    runner.writeFrameTimeCSV(outputPath + "/oop_frame_time.csv", allFrameData, scales);
    runner.writeMemoryCSV(outputPath + "/oop_memory.csv", allMemoryStats);
    runner.writeCreationCSV(outputPath + "/oop_creation.csv", allCreationStats);
    runner.writeHWCountersCSV(outputPath + "/oop_hw_counters.csv", allHWCounters);

    std::cout << "\n====================================" << std::endl;
    std::cout << "  OOP Benchmark Complete!" << std::endl;
    std::cout << "====================================" << std::endl;

    return 0;
}
