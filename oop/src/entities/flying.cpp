#include "entities/flying.hpp"

#include "shared/behavior_rules.hpp"

#include <cmath>

namespace oop {

// ==================== Bat ====================
Bat::Bat(EntityID id)
    : FlyingCreature(id, 6, shared::EntityType::Bat, 8.0f, 0.2f) {
}

void Bat::update(float dt) {
    FlyingCreature::update(dt);
    updateRandomFlight(dt);
}

void Bat::updateRandomFlight(float dt) {
    // 蝙蝠随机飞行 - 不需要目标距离
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
        getTargetDistanceSq(),
        0.0f,
        shared::simulationContext());
}

// ==================== Bee ====================
Bee::Bee(EntityID id)
    : FlyingCreature(id, 10, shared::EntityType::Bee, 5.0f, 0.3f) {
}

void Bee::update(float dt) {
    FlyingCreature::update(dt);
    updatePollination(dt);

    const auto& config = shared::getEntityConfig(entityType_);
    (void)shared::updateSpecialBehavior(
        entityType_,
        config,
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
        getTargetDistanceSq(),
        config.attackRange * config.attackRange,
        shared::simulationContext());
}

void Bee::updatePollination(float dt) {
    // 蜜蜂采蜜行为
    (void)dt;
}

} // namespace oop
