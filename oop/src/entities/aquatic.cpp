#include "entities/aquatic.hpp"
#include "shared/constants.hpp"
#include <cmath>

namespace oop {

// ==================== Fish ====================
Fish::Fish(EntityID id)
    : AquaticCreature(id, 3, shared::EntityType::Fish, 0.2f, false) {
    swimDirection_ = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
}

void Fish::update(float dt) {
    AquaticCreature::update(dt);
    updateSwimBehavior(dt);
}

void Fish::updateSwimBehavior(float dt) {
    // 鱼随机游动
    if (stateTimer_ > 1.0f) {
        swimDirection_ += static_cast<float>(rand()) / RAND_MAX * 0.5f - 0.25f;
        wanderDirX_ = std::cos(swimDirection_);
        wanderDirZ_ = std::sin(swimDirection_);
        stateTimer_ = 0.0f;
    }
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
    if (y_ < shared::WorldConstants::WATER_LEVEL && jumpCooldown_ <= 0.0f) {
        if (rand() % 1000 < 5) {  // 随机跳跃
            isJumping_ = true;
            jumpCooldown_ = JUMP_COOLDOWN;
        }
    }
}

// ==================== Turtle ====================
Turtle::Turtle(EntityID id)
    : AquaticCreature(id, 30, shared::EntityType::Turtle, 0.08f, true) {
    moveSpeed_ = 0.08f;  // 很慢
}

void Turtle::update(float dt) {
    AquaticCreature::update(dt);
    updateEggLaying(dt);
}

void Turtle::updateEggLaying(float dt) {
    // 海龟产卵行为
    if (isOnLand_) {
        eggTimer_ += dt;
    }
}

} // namespace oop
