#include "hybrid_entity.hpp"
#include "shared/constants.hpp"
#include <cmath>
#include <memory>

namespace hybrid {

// Base HybridEntity implementation
HybridEntity::HybridEntity(uint32_t id, shared::EntityType type)
    : id_(id)
    , type_(type) {
    const auto& config = shared::getEntityConfig(type);
    health_.current = config.maxHealth;
    health_.maximum = config.maxHealth;
    health_.isDead = false;
}

void HybridEntity::takeDamage(int damage) {
    health_.current -= damage;
    if (health_.current <= 0) {
        health_.current = 0;
        health_.isDead = true;
    }
}

void HybridEntity::heal(int amount) {
    if (health_.isDead) return;
    health_.current = std::min(health_.current + amount, health_.maximum);
}

bool HybridEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, [[maybe_unused]] float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    [[maybe_unused]] float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {
    // Default: no special behavior
    return false;
}

// Creeper - explodes when close to target
CreeperEntity::CreeperEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Creeper) {
}

bool CreeperEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, [[maybe_unused]] float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    bool hasTarget,
    float targetDistanceSq,
    float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Creeper);
    float fuseRangeSq = targetRangeSq > 0.0f ? targetRangeSq * 9.0f : 9.0f;

    bool shouldIgnite = hasTarget && targetDistanceSq <= fuseRangeSq;

    if (shouldIgnite || ignited_) {
        ignited_ = true;
        fuseTimer_ += dt;

        float fuseDuration = 1.0f + profile.aggression * 0.7f;
        if (fuseTimer_ >= fuseDuration) {
            // Explode - entity dies
            return true;  // Signal deactivation
        }
    } else {
        fuseTimer_ = 0.0f;
    }

    return false;
}

// Enderman - teleports periodically
EndermanEntity::EndermanEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Enderman) {
}

bool EndermanEntity::updateSpecialBehavior(
    float& x, float& y, float& z,
    [[maybe_unused]] float& dirX, float& dirZ,
    float dt,
    bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Enderman);
    const auto& context = shared::simulationContext();

    // Decrement cooldown first (to match shared behavior)
    if (teleportCooldown_ > 0.0f) {
        teleportCooldown_ = (teleportCooldown_ - dt) > 0.0f ? (teleportCooldown_ - dt) : 0.0f;
    }

    if (teleportCooldown_ <= 0.0f && (context.isNight || (!hasTarget && behaviorState_.alertness > 0.35f))) {
        float angle = shared::stableAngle(shared::EntityType::Enderman, behaviorState_, context.elapsedTime);
        float offset = 4.0f + profile.curiosity * 4.0f;

        x = std::clamp(behaviorState_.homeX + std::cos(angle) * offset, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        z = std::clamp(behaviorState_.homeZ + std::sin(angle) * offset, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
        y = std::clamp(y + ((context.isNight ? 1.0f : -1.0f) * (profile.verticalBias + 0.5f)),
                       0.0f, shared::WorldConstants::WORLD_SIZE_Y);

        teleportCooldown_ = 4.0f + profile.caution * 3.0f;
    }

    return false;
}

// Bat - adjusts height and direction based on time
BatEntity::BatEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Bat) {
}

bool BatEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, float& z,
    float& dirX, float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& config = shared::getEntityConfig(shared::EntityType::Bat);
    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Bat);
    const auto& context = shared::simulationContext();

    // Decrement cooldown first (to match shared behavior)
    if (heightBlendTimer_ > 0.0f) {
        heightBlendTimer_ = (heightBlendTimer_ - dt) > 0.0f ? (heightBlendTimer_ - dt) : 0.0f;
    }

    if (heightBlendTimer_ <= 0.0f || context.isNight) {
        shared::setHeadingFromStableAngle(shared::EntityType::Bat, behaviorState_,
                                          context.elapsedTime + behaviorState_.roamTimer, dirX, dirZ);
        heightBlendTimer_ = 1.5f + profile.curiosity * 2.0f;
    }

    float targetHeight = config.flightHeight > 0.0f ? config.flightHeight : 6.0f;
    targetHeight += profile.verticalBias * 3.0f;

    float heightBlend = dt * 0.5f;
    heightBlend = heightBlend < 1.0f ? heightBlend : 1.0f;  // Clamp to match shared behavior
    y += (targetHeight - y) * heightBlend;

    return false;
}

// Bee - becomes angry when attacking
BeeEntity::BeeEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Bee) {
}

bool BeeEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, [[maybe_unused]] float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    bool hasTarget,
    float targetDistanceSq,
    float targetRangeSq) {

    // Decrement anger timer first (to match shared behavior)
    if (angerTimer_ > 0.0f) {
        angerTimer_ = (angerTimer_ - dt) > 0.0f ? (angerTimer_ - dt) : 0.0f;
    }

    float attackRangeSq = targetRangeSq > 0.0f ? targetRangeSq * 0.75f : 4.0f;

    // Use RuntimeFlag to match shared behavior
    bool angry = hasRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Angry);
    if (hasTarget && targetDistanceSq <= attackRangeSq) {
        angry = true;
        angerTimer_ = 0.75f;
    }

    if (angry && angerTimer_ <= 0.0f) {
        angry = false;
    }

    shared::setRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Angry, angry);
    return false;
}

// Fish - adjusts depth and changes direction
FishEntity::FishEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Fish) {
}

bool FishEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, [[maybe_unused]] float& z,
    float& dirX, float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Fish);
    const auto& context = shared::simulationContext();

    // Decrement cooldown first (to match shared behavior)
    if (directionTimer_ > 0.0f) {
        directionTimer_ = (directionTimer_ - dt) > 0.0f ? (directionTimer_ - dt) : 0.0f;
    }

    if (directionTimer_ <= 0.0f) {
        shared::setHeadingFromStableAngle(shared::EntityType::Fish, behaviorState_,
                                          context.elapsedTime + behaviorState_.homeX * 0.1f, dirX, dirZ);
        directionTimer_ = 0.8f + profile.curiosity * 0.8f;
    }

    float waterBias = profile.waterBias > 0.0f ? profile.waterBias : 1.0f;
    float targetDepth = shared::WorldConstants::WATER_LEVEL - (2.0f + waterBias * 6.0f);

    float depthBlend = dt * 0.35f;
    depthBlend = depthBlend < 1.0f ? depthBlend : 1.0f;  // Clamp to match shared behavior
    y += (targetDepth - y) * depthBlend;

    return false;
}

// Dolphin - jumps out of water
DolphinEntity::DolphinEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Dolphin) {
}

bool DolphinEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Dolphin);

    // Depth adjustment
    float waterBias = profile.waterBias > 0.0f ? profile.waterBias : 1.0f;
    float targetDepth = shared::WorldConstants::WATER_LEVEL - (1.0f + waterBias * 3.0f);
    float depthBlend = dt * 0.35f;
    depthBlend = depthBlend < 1.0f ? depthBlend : 1.0f;  // Clamp to match shared behavior
    y += (targetDepth - y) * depthBlend;

    // Decrement cooldown first (to match shared behavior)
    if (jumpCooldown_ > 0.0f) {
        jumpCooldown_ = (jumpCooldown_ - dt) > 0.0f ? (jumpCooldown_ - dt) : 0.0f;
    }

    // Jump behavior - use RuntimeFlag to match shared behavior
    bool jumping = hasRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Jumping);
    if (!jumping && jumpCooldown_ <= 0.0f && y < shared::WorldConstants::WATER_LEVEL + 0.5f) {
        float noise = shared::stableAngle(shared::EntityType::Dolphin, behaviorState_,
                                          shared::simulationContext().elapsedTime);
        if (std::fmod(noise, 1.0f) > 0.75f) {
            jumping = true;
            jumpTimer_ = 0.0f;
            jumpCooldown_ = 3.0f + profile.curiosity * 2.0f;
        }
    }

    if (jumping) {
        jumpTimer_ += dt;
        y += 0.8f * dt * (1.0f + profile.verticalBias * 0.5f);

        if (jumpTimer_ >= 1.2f || y >= shared::WorldConstants::WATER_LEVEL + 2.0f) {
            jumping = false;
            jumpTimer_ = 0.0f;
        }
    }

    shared::setRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Jumping, jumping);
    return false;
}

// Phantom - diving behavior at night
PhantomEntity::PhantomEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Phantom) {
}

bool PhantomEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Phantom);
    const auto& context = shared::simulationContext();

    // Decrement cooldown first (to match shared behavior)
    if (diveCooldown_ > 0.0f) {
        diveCooldown_ = (diveCooldown_ - dt) > 0.0f ? (diveCooldown_ - dt) : 0.0f;
    }

    // Use RuntimeFlag to match shared behavior
    bool diving = hasRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Diving);
    if (!diving && context.isNight && diveCooldown_ <= 0.0f) {
        diving = true;
        diveTimer_ = 0.0f;
        diveCooldown_ = 4.0f + profile.curiosity;
    }

    if (diving) {
        diveTimer_ += dt;
        y -= 1.2f * dt;

        if (diveTimer_ >= 1.0f || y <= 1.5f) {
            diving = false;
            diveTimer_ = 0.0f;
        }
    }

    shared::setRuntimeFlag(behaviorState_.flags, shared::RuntimeFlag::Diving, diving);
    return false;
}

// Squid - maintains depth in water
SquidEntity::SquidEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Squid) {
}

bool SquidEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Squid);

    float waterBias = profile.waterBias > 0.0f ? profile.waterBias : 1.0f;
    float targetDepth = shared::WorldConstants::WATER_LEVEL - (2.0f + waterBias * 4.0f);
    float depthBlend = dt * 0.30f;
    depthBlend = depthBlend < 1.0f ? depthBlend : 1.0f;  // Clamp to match shared behavior

    y += (targetDepth - y) * depthBlend;

    return false;
}

// Turtle - stays at shallow water depth
TurtleEntity::TurtleEntity(uint32_t id)
    : HybridEntity(id, shared::EntityType::Turtle) {
}

bool TurtleEntity::updateSpecialBehavior(
    [[maybe_unused]] float& x, float& y, [[maybe_unused]] float& z,
    [[maybe_unused]] float& dirX, [[maybe_unused]] float& dirZ,
    float dt,
    [[maybe_unused]] bool hasTarget,
    [[maybe_unused]] float targetDistanceSq,
    [[maybe_unused]] float targetRangeSq) {

    const auto& profile = shared::getBehaviorProfile(shared::EntityType::Turtle);

    float waterBias = profile.waterBias > 0.0f ? profile.waterBias : 1.0f;
    float targetDepth = shared::WorldConstants::WATER_LEVEL - (1.0f + waterBias * 2.0f);
    float depthBlend = dt * 0.25f;
    depthBlend = depthBlend < 1.0f ? depthBlend : 1.0f;  // Clamp to match shared behavior

    y += (targetDepth - y) * depthBlend;

    return false;
}

// Factory function
std::unique_ptr<HybridEntity> createEntity(uint32_t id, shared::EntityType type) {
    switch (type) {
        // Monsters with special behavior
        case shared::EntityType::Creeper:
            return std::make_unique<CreeperEntity>(id);
        case shared::EntityType::Enderman:
            return std::make_unique<EndermanEntity>(id);

        // Flying with special behavior
        case shared::EntityType::Bat:
            return std::make_unique<BatEntity>(id);
        case shared::EntityType::Bee:
            return std::make_unique<BeeEntity>(id);

        // Aquatic with special behavior
        case shared::EntityType::Fish:
            return std::make_unique<FishEntity>(id);
        case shared::EntityType::Dolphin:
            return std::make_unique<DolphinEntity>(id);
        case shared::EntityType::Phantom:
            return std::make_unique<PhantomEntity>(id);
        case shared::EntityType::Squid:
            return std::make_unique<SquidEntity>(id);
        case shared::EntityType::Turtle:
            return std::make_unique<TurtleEntity>(id);

        // Default entities (no special behavior)
        default:
            return std::make_unique<HybridEntity>(id, type);
    }
}

// Helper function
bool hasSpecialBehavior(shared::EntityType type) {
    switch (type) {
        case shared::EntityType::Creeper:
        case shared::EntityType::Enderman:
        case shared::EntityType::Bat:
        case shared::EntityType::Bee:
        case shared::EntityType::Fish:
        case shared::EntityType::Dolphin:
        case shared::EntityType::Phantom:
        case shared::EntityType::Squid:
        case shared::EntityType::Turtle:
            return true;
        default:
            return false;
    }
}

} // namespace hybrid
