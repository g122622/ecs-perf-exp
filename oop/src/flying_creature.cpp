#include "flying_creature.hpp"
#include "shared/constants.hpp"

#include <algorithm>
#include <cmath>

namespace oop {

FlyingCreature::FlyingCreature(EntityID id, int maxHealth, shared::EntityType type,
                               float flightHeight, float flightSpeed)
    : Creature(id, maxHealth, type)
    , flightHeight_(flightHeight)
    , flightSpeed_(flightSpeed)
    , targetFlightHeight_(flightHeight) {

    // 设置初始飞行高度
    y_ = flightHeight_;
}

void FlyingCreature::update(float dt) {
    if (!active_) return;

    Creature::update(dt);

    // 调整高度到目标飞行高度
    const auto& profile = shared::getBehaviorProfile(entityType_);
    const auto& context = shared::simulationContext();
    float targetHeight = targetFlightHeight_ + profile.verticalBias * 4.0f;
    if (context.isNight) {
        targetHeight += profile.nightAffinity * 1.5f;
    }

    float heightDiff = targetHeight - y_;
    if (std::abs(heightDiff) > 0.1f) {
        y_ += std::copysign(flightSpeed_ * dt, heightDiff);
    }
}

void FlyingCreature::updateMovement(float dt) {
    // 飞行生物的移动，忽略重力
    float dx = 0.0f, dz = 0.0f;
    const auto& profile = shared::getBehaviorProfile(entityType_);
    const auto& context = shared::simulationContext();
    float speedScale = shared::computeActivityMultiplier(profile, behaviorState_, context, aiState_, getHealthPercent(), true, false);
    float currentSpeed = flightSpeed_ * speedScale;

    switch (aiState_) {
        case shared::AIState::Wander: {
            dx = wanderDirX_ * currentSpeed * dt;
            dz = wanderDirZ_ * currentSpeed * dt;
            break;
        }
        case shared::AIState::Chase:
        case shared::AIState::Flee: {
            dx = wanderDirX_ * currentSpeed * 1.5f * dt;
            dz = wanderDirZ_ * currentSpeed * 1.5f * dt;
            break;
        }
        case shared::AIState::Attack: {
            dx = wanderDirX_ * currentSpeed * 0.4f * dt;
            dz = wanderDirZ_ * currentSpeed * 0.4f * dt;
            break;
        }
        default:
            break;
    }

    if (profile.homeBias > 0.0f) {
        float homeDx = behaviorState_.homeX - x_;
        float homeDz = behaviorState_.homeZ - z_;
        float homeDistSq = homeDx * homeDx + homeDz * homeDz;
        if (homeDistSq > 1.0f) {
            float homeDist = std::sqrt(homeDistSq);
            float homeStep = currentSpeed * profile.homeBias * 0.08f * dt;
            dx += (homeDx / homeDist) * homeStep;
            dz += (homeDz / homeDist) * homeStep;
        }
    }

    // 应用移动
    x_ += dx;
    z_ += dz;

    // 世界边界检查
    x_ = std::clamp(x_, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
    z_ = std::clamp(z_, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    y_ = std::clamp(y_, 1.0f, shared::WorldConstants::WORLD_SIZE_Y);
}

void FlyingCreature::updateAI(float dt) {
    (void)dt;
    const auto& profile = shared::getBehaviorProfile(entityType_);
    const auto& context = shared::simulationContext();

    shared::AIState nextState = shared::chooseAIState(
        aiState_,
        profile,
        behaviorState_,
        context,
        getHealthPercent(),
        hasTarget(),
        hasTarget(),
        false,
        false,
        true,
        false);

    setAIState(nextState);
}

} // namespace oop
