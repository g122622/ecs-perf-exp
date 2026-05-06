#pragma once

#include "constants.hpp"
#include "entity_types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace shared {

// 共享模拟时钟。OOP 和 ECS 都使用同一套昼夜节奏参数。
struct SimulationContext {
    float elapsedTime = 0.0f;
    float dayPhase = 0.0f;
    bool isNight = false;
    float lightLevel = WorldConstants::DAY_LIGHT_LEVEL;
};

inline SimulationContext& simulationContext() {
    static SimulationContext context{};
    return context;
}

inline void resetSimulationClock() {
    simulationContext() = SimulationContext{};
}

inline void advanceSimulationClock(float dt) {
    auto& context = simulationContext();
    context.elapsedTime += dt;

    float cycle = std::fmod(context.elapsedTime, WorldConstants::DAY_NIGHT_CYCLE_SECONDS);
    if (cycle < 0.0f) {
        cycle += WorldConstants::DAY_NIGHT_CYCLE_SECONDS;
    }

    context.dayPhase = cycle / WorldConstants::DAY_NIGHT_CYCLE_SECONDS;
    context.isNight = context.dayPhase >= WorldConstants::NIGHT_PHASE_START ||
                      context.dayPhase < WorldConstants::DAY_PHASE_END;
    context.lightLevel = context.isNight ? WorldConstants::NIGHT_LIGHT_LEVEL : WorldConstants::DAY_LIGHT_LEVEL;
}

// 行为档案：在不改变架构的前提下，让不同实体拥有更贴近游戏的行为偏好。
struct BehaviorProfile {
    float aggression = 0.0f;
    float caution = 0.5f;
    float curiosity = 0.5f;
    float staminaDrain = 0.05f;
    float staminaRecovery = 0.08f;
    float nightAffinity = 0.0f;
    float socialBias = 0.0f;
    float homeBias = 0.0f;
    float verticalBias = 0.0f;
    float waterBias = 0.0f;
    float fleeHealthThreshold = 0.3f;
};

inline const BehaviorProfile& getBehaviorProfile(EntityType type) {
    static const BehaviorProfile profiles[] = {
        // Monster: 更偏向追击和夜间活跃
        {0.95f, 0.15f, 0.20f, 0.10f, 0.05f, 0.80f, 0.10f, 0.10f, 0.00f, 0.00f, 0.20f}, // Zombie
        {0.85f, 0.25f, 0.25f, 0.09f, 0.05f, 0.70f, 0.10f, 0.15f, 0.00f, 0.00f, 0.25f}, // Skeleton
        {0.75f, 0.20f, 0.35f, 0.08f, 0.06f, 0.65f, 0.05f, 0.10f, 0.10f, 0.00f, 0.20f}, // Spider
        {1.00f, 0.10f, 0.15f, 0.10f, 0.04f, 0.90f, 0.05f, 0.05f, 0.00f, 0.00f, 0.15f}, // Creeper
        {0.55f, 0.55f, 0.70f, 0.06f, 0.08f, 0.95f, 0.10f, 0.20f, 0.20f, 0.00f, 0.30f}, // Enderman
        {0.80f, 0.30f, 0.25f, 0.07f, 0.05f, 0.40f, 0.10f, 0.10f, 0.85f, 0.00f, 0.30f}, // Blaze
        {0.85f, 0.25f, 0.30f, 0.06f, 0.05f, 0.35f, 0.10f, 0.15f, 1.00f, 0.00f, 0.25f}, // Ghast
        {0.90f, 0.20f, 0.60f, 0.09f, 0.05f, 1.00f, 0.05f, 0.15f, 1.00f, 0.00f, 0.20f}, // Phantom

        // Animal: 更偏向谨慎和群体反应
        {0.05f, 0.85f, 0.35f, 0.05f, 0.10f, 0.15f, 0.55f, 0.20f, 0.00f, 0.00f, 0.35f}, // Pig
        {0.05f, 0.80f, 0.30f, 0.05f, 0.10f, 0.10f, 0.60f, 0.20f, 0.00f, 0.00f, 0.35f}, // Cow
        {0.05f, 0.85f, 0.40f, 0.05f, 0.10f, 0.10f, 0.60f, 0.20f, 0.00f, 0.00f, 0.35f}, // Sheep
        {0.05f, 0.80f, 0.45f, 0.05f, 0.11f, 0.15f, 0.50f, 0.15f, 0.10f, 0.00f, 0.30f}, // Chicken
        {0.05f, 0.95f, 0.55f, 0.09f, 0.12f, 0.05f, 0.35f, 0.10f, 0.15f, 0.00f, 0.45f}, // Rabbit
        {0.55f, 0.45f, 0.50f, 0.08f, 0.08f, 0.35f, 0.35f, 0.25f, 0.10f, 0.00f, 0.25f}, // Wolf
        {0.10f, 0.70f, 0.60f, 0.05f, 0.10f, 0.65f, 0.20f, 0.30f, 0.05f, 0.00f, 0.30f}, // Cat
        {0.20f, 0.75f, 0.65f, 0.06f, 0.09f, 0.80f, 0.20f, 0.20f, 0.10f, 0.00f, 0.30f}, // Fox

        // Flying: 更偏向垂直活动和夜间巡游
        {0.05f, 0.65f, 0.80f, 0.04f, 0.12f, 1.00f, 0.10f, 0.35f, 1.00f, 0.00f, 0.20f}, // Bat
        {0.35f, 0.45f, 0.70f, 0.07f, 0.08f, 0.25f, 0.40f, 0.25f, 0.55f, 0.00f, 0.25f}, // Bee

        // Aquatic: 更偏向水域活动和稳定巡游
        {0.00f, 0.70f, 0.55f, 0.04f, 0.12f, 0.10f, 0.75f, 0.10f, 0.00f, 1.00f, 0.35f}, // Fish
        {0.05f, 0.60f, 0.35f, 0.05f, 0.11f, 0.15f, 0.20f, 0.15f, 0.00f, 1.00f, 0.30f}, // Squid
        {0.10f, 0.35f, 0.80f, 0.06f, 0.10f, 0.10f, 0.80f, 0.25f, 0.60f, 1.00f, 0.25f}, // Dolphin
        {0.00f, 0.90f, 0.20f, 0.03f, 0.12f, 0.05f, 0.20f, 0.35f, 0.10f, 1.00f, 0.50f}, // Turtle
    };

    const size_t index = static_cast<size_t>(type);
    if (index < static_cast<size_t>(EntityType::Count)) {
        return profiles[index];
    }
    return profiles[0];
}

// 运行时状态：OOP 和 ECS 共享同样的实体行为状态。
enum class RuntimeFlag : uint32_t {
    None = 0,
    Ignited = 1u << 0,
    Diving = 1u << 1,
    Sleeping = 1u << 2,
    Angry = 1u << 3,
    HoldingItem = 1u << 4,
    OnLand = 1u << 5,
    Jumping = 1u << 6,
};

inline bool hasRuntimeFlag(uint32_t flags, RuntimeFlag flag) {
    return (flags & static_cast<uint32_t>(flag)) != 0;
}

inline void setRuntimeFlag(uint32_t& flags, RuntimeFlag flag, bool enabled) {
    if (enabled) {
        flags |= static_cast<uint32_t>(flag);
    } else {
        flags &= ~static_cast<uint32_t>(flag);
    }
}

struct BehaviorState {
    float stamina = 1.0f;
    float alertness = 0.0f;
    float roamTimer = 0.0f;
    float specialTimer = 0.0f;
    float specialCooldown = 0.0f;
    float homeX = 0.0f;
    float homeY = 0.0f;
    float homeZ = 0.0f;
    uint32_t flags = 0;
};

inline void initializeBehaviorState(BehaviorState& state, float x, float y, float z) {
    state.stamina = 1.0f;
    state.alertness = 0.0f;
    state.roamTimer = 0.0f;
    state.specialTimer = 0.0f;
    state.specialCooldown = 0.0f;
    state.homeX = x;
    state.homeY = y;
    state.homeZ = z;
    state.flags = 0;
}

inline void setHeadingFromVector(float dx, float dz, float& dirX, float& dirZ) {
    float lengthSq = dx * dx + dz * dz;
    if (lengthSq > 0.0001f) {
        float invLength = 1.0f / std::sqrt(lengthSq);
        dirX = dx * invLength;
        dirZ = dz * invLength;
    }
}

inline uint64_t hashMix(uint64_t value) {
    value ^= value >> 33;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33;
    value *= 0xc4ceb9fe1a85ec53ULL;
    value ^= value >> 33;
    return value;
}

inline uint64_t hashCombine(uint64_t seed, uint64_t value) {
    return hashMix(seed ^ (hashMix(value) + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U)));
}

inline uint64_t quantizeCoordinate(float value) {
    return static_cast<uint64_t>(std::llround(value * 10.0f));
}

inline float stableAngle(EntityType type, const BehaviorState& state, float phase) {
    uint64_t seed = static_cast<uint64_t>(type);
    seed = hashCombine(seed, quantizeCoordinate(state.homeX));
    seed = hashCombine(seed, quantizeCoordinate(state.homeY));
    seed = hashCombine(seed, quantizeCoordinate(state.homeZ));
    seed = hashCombine(seed, static_cast<uint64_t>(std::llround(phase * 1000.0f)));

    constexpr uint64_t ANGLE_SCALE = 6283185ULL;
    return static_cast<float>(seed % ANGLE_SCALE) / 1000000.0f;
}

inline void setHeadingFromStableAngle(EntityType type, const BehaviorState& state, float phase, float& dirX, float& dirZ) {
    float angle = stableAngle(type, state, phase);
    dirX = std::cos(angle);
    dirZ = std::sin(angle);
}

struct AIThresholds {
    float idleLimit = WorldConstants::IDLE_DURATION_MAX;
    float wanderLimit = WorldConstants::WANDER_DURATION_MAX;
    float chaseLimit = WorldConstants::CHASE_DURATION_MAX;
    float fleeLimit = WorldConstants::FLEE_DURATION_MAX;
};

inline AIThresholds computeAIThresholds(const BehaviorProfile& profile, const SimulationContext& context) {
    AIThresholds thresholds{};

    thresholds.idleLimit = WorldConstants::IDLE_DURATION_MIN +
                           (WorldConstants::IDLE_DURATION_MAX - WorldConstants::IDLE_DURATION_MIN) *
                           (1.0f - profile.curiosity * 0.65f);

    thresholds.wanderLimit = WorldConstants::WANDER_DURATION_MIN +
                             (WorldConstants::WANDER_DURATION_MAX - WorldConstants::WANDER_DURATION_MIN) *
                             (0.50f + profile.curiosity * 0.25f);

    thresholds.chaseLimit = WorldConstants::CHASE_DURATION_MAX *
                            (0.70f + profile.aggression * 0.35f);

    thresholds.fleeLimit = WorldConstants::FLEE_DURATION_MIN +
                           (WorldConstants::FLEE_DURATION_MAX - WorldConstants::FLEE_DURATION_MIN) *
                           (0.35f + profile.caution * 0.45f);

    if (context.isNight) {
        thresholds.idleLimit *= 0.85f + profile.nightAffinity * 0.15f;
        thresholds.wanderLimit *= 0.90f + profile.nightAffinity * 0.10f;
    }

    return thresholds;
}

inline float computeActivityMultiplier(const BehaviorProfile& profile,
                                       const BehaviorState& state,
                                       const SimulationContext& context,
                                       AIState currentState,
                                       float healthPercent,
                                       bool isFlying,
                                       bool isAquatic) {
    float multiplier = 1.0f;

    multiplier += profile.curiosity * 0.10f;
    multiplier += profile.socialBias * 0.05f;
    multiplier += context.isNight ? profile.nightAffinity * 0.18f : -profile.nightAffinity * 0.05f;

    if (currentState == AIState::Chase) {
        multiplier += profile.aggression * 0.30f;
    } else if (currentState == AIState::Flee) {
        multiplier += profile.caution * 0.25f;
    } else if (currentState == AIState::Idle) {
        multiplier *= 0.92f;
    }

    multiplier *= 0.70f + state.stamina * 0.40f;

    if (healthPercent < 0.5f) {
        multiplier *= 0.75f + healthPercent * 0.50f;
    }

    if (isFlying) {
        multiplier += profile.verticalBias * 0.10f;
    }

    if (isAquatic) {
        multiplier += profile.waterBias * 0.10f;
    }

    if (hasRuntimeFlag(state.flags, RuntimeFlag::Sleeping)) {
        multiplier *= 0.25f;
    }

    return std::clamp(multiplier, 0.25f, 1.85f);
}

inline void updateBehaviorState(BehaviorState& state,
                                const BehaviorProfile& profile,
                                AIState currentState,
                                float dt,
                                bool moving,
                                float healthPercent,
                                const SimulationContext& context) {
    state.roamTimer += dt;
    state.specialTimer += dt;

    if (state.specialCooldown > 0.0f) {
        state.specialCooldown = (state.specialCooldown - dt) > 0.0f ? (state.specialCooldown - dt) : 0.0f;
    }

    float staminaDelta = moving ? -profile.staminaDrain : profile.staminaRecovery;
    if (currentState == AIState::Chase || currentState == AIState::Attack) {
        staminaDelta -= profile.staminaDrain * 0.5f;
    } else if (currentState == AIState::Flee) {
        staminaDelta -= profile.staminaDrain * 0.35f;
    } else if (currentState == AIState::Idle) {
        staminaDelta += profile.staminaRecovery * 0.20f;
    }

    if (context.isNight) {
        staminaDelta += profile.nightAffinity * 0.01f;
    }

    state.stamina = std::clamp(state.stamina + staminaDelta * dt, 0.15f, 1.20f);

    float alertnessDelta = 0.0f;
    if (currentState == AIState::Chase || currentState == AIState::Attack) {
        alertnessDelta += 0.15f;
    } else if (currentState == AIState::Flee) {
        alertnessDelta += 0.20f;
    } else if (currentState == AIState::Idle) {
        alertnessDelta -= 0.08f;
    }

    alertnessDelta += context.isNight ? profile.nightAffinity * 0.06f : -profile.nightAffinity * 0.03f;
    alertnessDelta += (healthPercent < 0.5f) ? profile.caution * 0.08f : -0.02f;

    state.alertness = std::clamp(state.alertness + alertnessDelta * dt, 0.0f, 1.0f);
}

inline AIState chooseAIState(AIState currentState,
                            const BehaviorProfile& profile,
                            const BehaviorState& state,
                            const SimulationContext& context,
                            float healthPercent,
                            bool hasTarget,
                            bool targetValid,
                            bool isMonster,
                            bool isAnimal,
                            bool isFlying,
                            bool isAquatic) {
    AIThresholds thresholds = computeAIThresholds(profile, context);

    if (healthPercent <= profile.fleeHealthThreshold && (isAnimal || profile.caution >= profile.aggression)) {
        return AIState::Flee;
    }

    if (currentState == AIState::Attack && hasTarget && targetValid) {
        return AIState::Attack;
    }

    if (hasTarget && targetValid) {
        if (isMonster || profile.aggression > profile.caution) {
            return AIState::Chase;
        }
        return AIState::Flee;
    }

    if (isMonster) {
        if (currentState == AIState::Idle && state.roamTimer >= thresholds.idleLimit) {
            return AIState::Wander;
        }
        if (currentState == AIState::Wander && state.roamTimer >= thresholds.wanderLimit) {
            return AIState::Idle;
        }
        if (currentState == AIState::Chase && state.roamTimer >= thresholds.chaseLimit) {
            return AIState::Wander;
        }
        if (currentState == AIState::Flee && state.roamTimer >= thresholds.fleeLimit) {
            return AIState::Wander;
        }

        if (context.isNight || profile.nightAffinity > 0.5f) {
            return (profile.aggression > 0.45f) ? AIState::Wander : AIState::Idle;
        }
        return currentState == AIState::Attack ? AIState::Attack : AIState::Wander;
    }

    if (isAnimal) {
        if (state.alertness > 0.6f || healthPercent <= profile.fleeHealthThreshold) {
            return AIState::Flee;
        }
        if (context.isNight && profile.nightAffinity < 0.25f) {
            return AIState::Idle;
        }
        if (currentState == AIState::Idle && state.roamTimer >= thresholds.idleLimit * 0.5f) {
            return AIState::Wander;
        }
        if (currentState == AIState::Wander && state.roamTimer >= thresholds.wanderLimit) {
            return AIState::Idle;
        }
        if (currentState == AIState::Flee && state.roamTimer >= thresholds.fleeLimit) {
            return AIState::Wander;
        }
        return currentState;
    }

    if (isFlying || isAquatic) {
        if (currentState == AIState::Idle && state.roamTimer >= thresholds.idleLimit) {
            return AIState::Wander;
        }
        if (currentState == AIState::Wander && state.roamTimer >= thresholds.wanderLimit) {
            return AIState::Idle;
        }
        if (currentState == AIState::Flee && state.roamTimer >= thresholds.fleeLimit) {
            return AIState::Wander;
        }
        return currentState;
    }

    return currentState;
}

inline float scoreTargetCandidate(const BehaviorProfile& profile,
                                 const SimulationContext& context,
                                 float distanceSq,
                                 float targetHealthPercent,
                                 bool targetIsAnimal,
                                 bool targetIsMonster,
                                 bool targetIsFlying,
                                 bool targetIsAquatic) {
    float clampedDistanceSq = distanceSq > 0.0f ? distanceSq : 0.0f;
    float distanceFactor = 1.0f / (1.0f + std::sqrt(clampedDistanceSq));
    float vulnerabilityFactor = 1.0f - targetHealthPercent;

    float categoryBonus = 0.0f;
    if (targetIsAnimal) {
        categoryBonus = 0.55f;
    } else if (targetIsFlying) {
        categoryBonus = 0.20f;
    } else if (targetIsAquatic) {
        categoryBonus = 0.15f;
    } else if (targetIsMonster) {
        categoryBonus = -0.10f;
    }

    float nightBonus = context.isNight ? profile.nightAffinity * 0.20f : -profile.nightAffinity * 0.05f;

    return distanceFactor * (1.10f + profile.aggression * 0.35f) +
           vulnerabilityFactor * 0.75f +
           categoryBonus +
           nightBonus -
           profile.caution * 0.10f;
}

inline float computeAttackDamageScale(const BehaviorProfile& profile,
                                      const BehaviorState& state,
                                      const SimulationContext& context,
                                      float distanceSq,
                                      float rangeSq) {
    float staminaFactor = 0.80f + state.stamina * 0.35f;
    float nightFactor = context.isNight ? (1.0f + profile.nightAffinity * 0.15f) : (1.0f - profile.nightAffinity * 0.05f);
    float rangeFactor = 1.0f;

    if (rangeSq > 0.0f) {
        float normalizedDistance = std::clamp(distanceSq / rangeSq, 0.0f, 1.0f);
        rangeFactor = 1.0f - normalizedDistance * 0.20f;
    }

    return std::clamp(staminaFactor * nightFactor * rangeFactor, 0.50f, 1.80f);
}

enum class SpecialBehaviorResult {
    None,
    Deactivate,
};

inline SpecialBehaviorResult updateSpecialBehavior(EntityType type,
                                                   const EntityConfig& config,
                                                   const BehaviorProfile& profile,
                                                   BehaviorState& state,
                                                   [[maybe_unused]] AIState& aiState,
                                                   float& x,
                                                   float& y,
                                                   float& z,
                                                   float& dirX,
                                                   float& dirZ,
                                                   float dt,
                                                   bool hasTarget,
                                                   float targetDistanceSq,
                                                   float targetRangeSq,
                                                   const SimulationContext& context) {
    if (state.specialCooldown > 0.0f) {
        state.specialCooldown = (state.specialCooldown - dt) > 0.0f ? (state.specialCooldown - dt) : 0.0f;
    }

    switch (type) {
        case EntityType::Creeper: {
            bool shouldIgnite = hasTarget && targetDistanceSq <= (config.attackRange > 0.0f ? config.attackRange * config.attackRange * 9.0f : 9.0f);
            setRuntimeFlag(state.flags, RuntimeFlag::Ignited, shouldIgnite || hasRuntimeFlag(state.flags, RuntimeFlag::Ignited));

            if (hasRuntimeFlag(state.flags, RuntimeFlag::Ignited)) {
                state.specialTimer += dt;
                float fuseDuration = 1.0f + profile.aggression * 0.7f;
                if (state.specialTimer >= fuseDuration) {
                    return SpecialBehaviorResult::Deactivate;
                }
            } else {
                state.specialTimer = 0.0f;
            }
            break;
        }

        case EntityType::Enderman: {
            if (state.specialCooldown <= 0.0f && (context.isNight || (!hasTarget && state.alertness > 0.35f))) {
                float angle = stableAngle(type, state, context.elapsedTime);
                float offset = 4.0f + profile.curiosity * 4.0f;
                x = std::clamp(state.homeX + std::cos(angle) * offset, 0.0f, WorldConstants::WORLD_SIZE_X);
                z = std::clamp(state.homeZ + std::sin(angle) * offset, 0.0f, WorldConstants::WORLD_SIZE_Z);
                y = std::clamp(y + ((context.isNight ? 1.0f : -1.0f) * (profile.verticalBias + 0.5f)), 0.0f, WorldConstants::WORLD_SIZE_Y);
                state.specialCooldown = 4.0f + profile.caution * 3.0f;
            }
            break;
        }

        case EntityType::Bat: {
            if (state.specialCooldown <= 0.0f || context.isNight) {
                setHeadingFromStableAngle(type, state, context.elapsedTime + state.roamTimer, dirX, dirZ);
                state.specialCooldown = 1.5f + profile.curiosity * 2.0f;
            }

            float targetHeight = config.flightHeight > 0.0f ? config.flightHeight : 6.0f;
            targetHeight += profile.verticalBias * 3.0f;
            float heightBlend = dt * 0.5f;
            heightBlend = heightBlend < 1.0f ? heightBlend : 1.0f;
            y += (targetHeight - y) * heightBlend;
            break;
        }

        case EntityType::Bee: {
            if (hasTarget && targetDistanceSq <= (targetRangeSq > 0.0f ? targetRangeSq * 0.75f : 4.0f)) {
                setRuntimeFlag(state.flags, RuntimeFlag::Angry, true);
                state.specialCooldown = 0.75f;
            } else if (state.specialCooldown <= 0.0f) {
                setRuntimeFlag(state.flags, RuntimeFlag::Angry, false);
            }
            break;
        }

        case EntityType::Fish: {
            if (state.specialCooldown <= 0.0f) {
                setHeadingFromStableAngle(type, state, context.elapsedTime + state.homeX * 0.1f, dirX, dirZ);
                state.specialCooldown = 0.8f + profile.curiosity * 0.8f;
            }

            float waterBias = profile.waterBias > 0.0f ? profile.waterBias : 1.0f;
            float targetDepth = WorldConstants::WATER_LEVEL - (2.0f + waterBias * 6.0f);
            float depthBlend = dt * 0.35f;
            depthBlend = depthBlend < 1.0f ? depthBlend : 1.0f;
            y += (targetDepth - y) * depthBlend;
            break;
        }

        case EntityType::Dolphin: {
            bool jumping = hasRuntimeFlag(state.flags, RuntimeFlag::Jumping);
            if (!jumping && state.specialCooldown <= 0.0f && y < WorldConstants::WATER_LEVEL + 0.5f) {
                float noise = stableAngle(type, state, context.elapsedTime);
                if (std::fmod(noise, 1.0f) > 0.75f) {
                    setRuntimeFlag(state.flags, RuntimeFlag::Jumping, true);
                    state.specialTimer = 0.0f;
                    state.specialCooldown = 3.0f + profile.curiosity * 2.0f;
                }
            }

            if (hasRuntimeFlag(state.flags, RuntimeFlag::Jumping)) {
                state.specialTimer += dt;
                y += 0.8f * dt * (1.0f + profile.verticalBias * 0.5f);
                if (state.specialTimer >= 1.2f || y >= WorldConstants::WATER_LEVEL + 2.0f) {
                    setRuntimeFlag(state.flags, RuntimeFlag::Jumping, false);
                    state.specialTimer = 0.0f;
                }
            }
            break;
        }

        case EntityType::Phantom: {
            bool diving = hasRuntimeFlag(state.flags, RuntimeFlag::Diving);
            if (!diving && context.isNight && state.specialCooldown <= 0.0f) {
                setRuntimeFlag(state.flags, RuntimeFlag::Diving, true);
                state.specialTimer = 0.0f;
                state.specialCooldown = 4.0f + profile.curiosity;
            }

            if (hasRuntimeFlag(state.flags, RuntimeFlag::Diving)) {
                state.specialTimer += dt;
                y -= 1.2f * dt;
                if (state.specialTimer >= 1.0f || y <= 1.5f) {
                    setRuntimeFlag(state.flags, RuntimeFlag::Diving, false);
                    state.specialTimer = 0.0f;
                }
            }
            break;
        }

        case EntityType::Squid:
        case EntityType::Chicken:
        case EntityType::Blaze:
        case EntityType::Ghast:
        case EntityType::Turtle:
        case EntityType::Wolf:
        case EntityType::Cat:
        case EntityType::Fox:
        case EntityType::Spider:
        case EntityType::Zombie:
        case EntityType::Skeleton:
        case EntityType::Pig:
        case EntityType::Cow:
        case EntityType::Sheep:
        case EntityType::Rabbit:
        case EntityType::Count:
            break;
    }

    return SpecialBehaviorResult::None;
}

} // namespace shared