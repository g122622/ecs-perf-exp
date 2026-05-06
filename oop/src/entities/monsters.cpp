#include "entities/monsters.hpp"
#include <cmath>

namespace oop {

// ==================== Zombie ====================
Zombie::Zombie(EntityID id)
    : Monster(id, 20, shared::EntityType::Zombie, 3, 1.0f) {
}

void Zombie::update(float dt) {
    Monster::update(dt);
    updateBehavior(dt);
}

void Zombie::updateBehavior(float dt) {
    // 僵尸特有的行为逻辑
    // 基础Monster AI已经处理了大部分逻辑
}

// ==================== Skeleton ====================
Skeleton::Skeleton(EntityID id)
    : Monster(id, 20, shared::EntityType::Skeleton, 2, 15.0f) {
}

void Skeleton::update(float dt) {
    Monster::update(dt);
    updateRangedAttack(dt);
}

void Skeleton::updateRangedAttack(float dt) {
    // 远程攻击逻辑
}

// ==================== Spider ====================
Spider::Spider(EntityID id)
    : Monster(id, 16, shared::EntityType::Spider, 2, 1.0f) {
}

void Spider::update(float dt) {
    Monster::update(dt);
}

// ==================== Creeper ====================
Creeper::Creeper(EntityID id)
    : Monster(id, 20, shared::EntityType::Creeper, 15, 3.0f) {
}

void Creeper::update(float dt) {
    Monster::update(dt);
    updateExplosion(dt);
}

void Creeper::updateExplosion(float dt) {
    if (isIgnited_) {
        fuseTime_ += dt;
        if (fuseTime_ >= MAX_FUSE) {
            // 爆炸
            active_ = false;
        }
    }
}

// ==================== Enderman ====================
Enderman::Enderman(EntityID id)
    : Monster(id, 40, shared::EntityType::Enderman, 7, 1.0f) {
}

void Enderman::update(float dt) {
    Monster::update(dt);
    updateTeleport(dt);
}

void Enderman::updateTeleport(float dt) {
    if (teleportCooldown_ > 0.0f) {
        teleportCooldown_ -= dt;
    }
}

// ==================== Blaze ====================
Blaze::Blaze(EntityID id)
    : Monster(id, 20, shared::EntityType::Blaze, 6, 20.0f) {
    // 烈焰人是飞行生物
    y_ = 5.0f;  // 初始飞行高度
}

void Blaze::update(float dt) {
    Monster::update(dt);
    updateFireballAttack(dt);
}

void Blaze::updateFireballAttack(float dt) {
    if (fireballCooldown_ > 0.0f) {
        fireballCooldown_ -= dt;
    }
}

// ==================== Ghast ====================
Ghast::Ghast(EntityID id)
    : Monster(id, 10, shared::EntityType::Ghast, 17, 50.0f) {
    y_ = 10.0f;  // 飞行高度
}

void Ghast::update(float dt) {
    Monster::update(dt);
    updateFireballAttack(dt);
}

void Ghast::updateFireballAttack(float dt) {
    if (fireballCooldown_ > 0.0f) {
        fireballCooldown_ -= dt;
    }
}

// ==================== Phantom ====================
Phantom::Phantom(EntityID id)
    : Monster(id, 20, shared::EntityType::Phantom, 2, 1.0f) {
    y_ = 15.0f;  // 飞行高度
}

void Phantom::update(float dt) {
    Monster::update(dt);
    updateDiveAttack(dt);
}

void Phantom::updateDiveAttack(float dt) {
    if (diveCooldown_ > 0.0f) {
        diveCooldown_ -= dt;
    }

    if (isDiving_) {
        // 俯冲攻击逻辑
    }
}

} // namespace oop
