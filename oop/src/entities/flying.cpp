#include "entities/flying.hpp"
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
    // 蝙蝠随机飞行
    if (stateTimer_ > 2.0f) {
        // 随机改变方向
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
        wanderDirX_ = std::cos(angle);
        wanderDirZ_ = std::sin(angle);
        targetFlightHeight_ = 5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f;
        stateTimer_ = 0.0f;
    }
}

// ==================== Bee ====================
Bee::Bee(EntityID id)
    : FlyingCreature(id, 10, shared::EntityType::Bee, 5.0f, 0.3f) {
}

void Bee::update(float dt) {
    FlyingCreature::update(dt);
    updatePollination(dt);

    if (stingCooldown_ > 0.0f) {
        stingCooldown_ -= dt;
    }
}

void Bee::updatePollination(float dt) {
    // 蜜蜂采蜜行为
}

} // namespace oop
