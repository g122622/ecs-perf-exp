#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace shared {

// Benchmark规模配置
enum class BenchmarkScale : uint8_t {
    Small = 0,      // 100 entities
    Medium,         // 1000 entities
    Large,          // 10000 entities
    XLarge          // 100000 entities
};

// 规模对应的实体数量
inline size_t getEntityCount(BenchmarkScale scale) {
    switch (scale) {
        case BenchmarkScale::Small:   return 1000;
        case BenchmarkScale::Medium:  return 2000;
        case BenchmarkScale::Large:   return 3000;
        case BenchmarkScale::XLarge:  return 4000;
        default: return 100;
    }
}

// Benchmark配置
struct BenchmarkConfig {
    size_t entityCount;
    size_t frameCount;
    unsigned int randomSeed;
    std::string outputPath;

    static BenchmarkConfig create(BenchmarkScale scale,
                                   size_t frames = 10000,
                                   unsigned int seed = 12345,
                                   const std::string& output = "./results") {
        return {
            getEntityCount(scale),
            frames,
            seed,
            output
        };
    }
};

// 获取所有测试规模
inline std::vector<BenchmarkConfig> getAllBenchmarkConfigs() {
    return {
        BenchmarkConfig::create(BenchmarkScale::Small),
        BenchmarkConfig::create(BenchmarkScale::Medium),
        BenchmarkConfig::create(BenchmarkScale::Large),
        BenchmarkConfig::create(BenchmarkScale::XLarge),
    };
}

// 测试配置名称
inline std::string getScaleName(BenchmarkScale scale) {
    switch (scale) {
        case BenchmarkScale::Small:   return "Small_100";
        case BenchmarkScale::Medium:  return "Medium_1000";
        case BenchmarkScale::Large:   return "Large_10000";
        case BenchmarkScale::XLarge:  return "XLarge_100000";
        default: return "Unknown";
    }
}

} // namespace shared
