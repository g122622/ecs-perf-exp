#include "entities/aquatic.hpp"

#include "shared/behavior_rules.hpp"
#include "shared/constants.hpp"

#include <cmath>

namespace oop {

// ==================== Fish ====================
Fish::Fish(EntityID id)
    : AquaticCreature(id, 3, shared::EntityType::Fish, shared::getEntityConfig(shared::EntityType::Fish).swimSpeed, false) {
    swimDirection_ = 0.0f;
}

void Fish::update(float dt) {
    AquaticCreature::update(dt);
    updateSwimBehavior(dt);
}

void Fish::updateSwimBehavior(float dt) {
    // 鱼随机游动
    (void)shared::updateSpecialBehavior(
        entityType_,
        shared::getEntityConfig(entityType_),
        shared::getBehaviorProfile(entityType_),
        behaviorState_,
        aiState_,
        x_,
        y_,
        z_,
        wanderDirX_,
        wanderDirZ_,
        dt,
        hasTarget(),
        0.0f,
        0.0f,
        shared::simulationContext());
}

// ==================== Squid ====================
Squid::Squid(EntityID id)
    : AquaticCreature(id, 10, shared::EntityType::Squid, 0.3f, false) {
}

void Squid::update(float dt) {
    AquaticCreature::update(dt);
    updateInkDefense(dt);

    if (inkCooldown_ > 0.0f) {
        inkCooldown_ -= dt;
    }
}

void Squid::updateInkDefense(float dt) {
    // 鱿鱼喷墨逃跑
    (void)shared::updateSpecialBehavior(
        entityType_,
        shared::getEntityConfig(entityType_),
        shared::getBehaviorProfile(entityType_),
        behaviorState_,
        aiState_,
        x_,
        y_,
        z_,
        wanderDirX_,
        wanderDirZ_,
        dt,
        hasTarget(),
        0.0f,
        0.0f,
        shared::simulationContext());
}

// ==================== Dolphin ====================
Dolphin::Dolphin(EntityID id)
    : AquaticCreature(id, 10, shared::EntityType::Dolphin, 0.8f, true) {
}

void Dolphin::update(float dt) {
    AquaticCreature::update(dt);
    updateJumpBehavior(dt);

    if (jumpCooldown_ > 0.0f) {
        jumpCooldown_ -= dt;
    }
}

void Dolphin::updateJumpBehavior(float dt) {
    // 海豚跳跃行为
    (void)shared::updateSpecialBehavior(
        entityType_,
        shared::getEntityConfig(entityType_),
        shared::getBehaviorProfile(entityType_),
        behaviorState_,
        aiState_,
        x_,
        y_,
        z_,
        wanderDirX_,
        wanderDirZ_,
        dt,
        hasTarget(),
        0.0f,
        0.0f,
        shared::simulationContext());
}

// ==================== Turtle ====================
Turtle::Turtle(EntityID id)
    : AquaticCreature(id, 30, shared::EntityType::Turtle, shared::getEntityConfig(shared::EntityType::Turtle).swimSpeed, true) {
    moveSpeed_ = shared::getEntityConfig(shared::EntityType::Turtle).moveSpeed;  // 很慢
}

void Turtle::update(float dt) {
    AquaticCreature::update(dt);
    updateEggLaying(dt);
}

void Turtle::updateEggLaying(float dt) {
    // 海龟产卵行为
    (void)shared::updateSpecialBehavior(
        entityType_,
        shared::getEntityConfig(entityType_),
        shared::getBehaviorProfile(entityType_),
        behaviorState_,
        aiState_,
        x_,
        y_,
        z_,
        wanderDirX_,
        wanderDirZ_,
        dt,
        hasTarget(),
        0.0f,
        0.0f,
        shared::simulationContext());
}

} // namespace oop
