#pragma once

#include "shared/constants.hpp"
#include <random>

namespace shared {

inline constexpr float PI = 3.14159265f;

inline std::mt19937 benchmarkRng{RandomConstants::DEFAULT_SEED};

inline void seedBenchmarkRng(unsigned int seed) {
    benchmarkRng.seed(seed);
}

inline float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(benchmarkRng);
}

inline int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(benchmarkRng);
}

inline float randomAngle() {
    return randomFloat(0.0f, 1.0f) * 2.0f * PI;
}

} // namespace shared