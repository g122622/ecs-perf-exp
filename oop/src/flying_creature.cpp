#include "flying_creature.hpp"
#include "shared/constants.hpp"
#include <cmath>
#include <algorithm>

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
    float heightDiff = targetFlightHeight_ - y_;
    if (std::abs(heightDiff) > 0.1f) {
        y_ += std::copysign(flightSpeed_ * dt, heightDiff);
    }
}

void FlyingCreature::updateMovement(float dt) {
    // 飞行生物的移动，忽略重力
    float dx = 0.0f, dz = 0.0f;

    switch (aiState_) {
        case shared::AIState::Wander: {
            dx = wanderDirX_ * flightSpeed_ * dt;
            dz = wanderDirZ_ * flightSpeed_ * dt;
            break;
        }
        case shared::AIState::Chase:
        case shared::AIState::Flee: {
            dx = wanderDirX_ * flightSpeed_ * 1.5f * dt;
            dz = wanderDirZ_ * flightSpeed_ * 1.5f * dt;
            break;
        }
        default:
            break;
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
    // 飞行生物的默认AI行为
    switch (aiState_) {
        case shared::AIState::Idle:
            if (stateTimer_ > shared::WorldConstants::IDLE_DURATION_MAX) {
                setAIState(shared::AIState::Wander);
            }
            break;
        case shared::AIState::Wander:
            if (stateTimer_ > shared::WorldConstants::WANDER_DURATION_MAX) {
                setAIState(shared::AIState::Idle);
            }
            break;
        default:
            break;
    }
}

} // namespace oop
