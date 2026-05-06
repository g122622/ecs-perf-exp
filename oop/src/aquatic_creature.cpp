#include "aquatic_creature.hpp"
#include "shared/constants.hpp"
#include <cmath>
#include <algorithm>

namespace oop {

AquaticCreature::AquaticCreature(EntityID id, int maxHealth, shared::EntityType type,
                                 float swimSpeed, bool canBreathe)
    : Creature(id, maxHealth, type)
    , swimSpeed_(swimSpeed)
    , canBreathe_(canBreathe) {

    // 水生生物初始位置在水下
    y_ = shared::WorldConstants::WATER_LEVEL * 0.5f;
}

void AquaticCreature::update(float dt) {
    if (!active_) return;

    Creature::update(dt);

    // 不能呼吸的生物在水面以上会受到伤害
    if (!canBreathe_ && y_ > shared::WorldConstants::WATER_LEVEL) {
        takeDamage(1);  // 简单的窒息伤害
    }
}

void AquaticCreature::updateMovement(float dt) {
    // 水生生物的游泳移动
    float dx = 0.0f, dz = 0.0f;
    float speed = (y_ < shared::WorldConstants::WATER_LEVEL) ? swimSpeed_ : moveSpeed_;

    switch (aiState_) {
        case shared::AIState::Wander: {
            dx = wanderDirX_ * speed * dt;
            dz = wanderDirZ_ * speed * dt;
            break;
        }
        case shared::AIState::Chase:
        case shared::AIState::Flee: {
            dx = wanderDirX_ * speed * 1.5f * dt;
            dz = wanderDirZ_ * speed * 1.5f * dt;
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
    y_ = std::clamp(y_, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
}

void AquaticCreature::updateAI(float dt) {
    // 水生生物的默认AI行为
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
