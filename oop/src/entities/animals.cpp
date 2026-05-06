#include "entities/animals.hpp"

#include "shared/behavior_rules.hpp"

#include <cmath>

namespace oop {

// ==================== Pig ====================
Pig::Pig(EntityID id)
    : Animal(id, 10, shared::EntityType::Pig) {
}

void Pig::update(float dt) {
    Animal::update(dt);
}

// ==================== Cow ====================
Cow::Cow(EntityID id)
    : Animal(id, 10, shared::EntityType::Cow) {
}

void Cow::update(float dt) {
    Animal::update(dt);
}

// ==================== Sheep ====================
Sheep::Sheep(EntityID id)
    : Animal(id, 8, shared::EntityType::Sheep) {
}

void Sheep::update(float dt) {
    Animal::update(dt);
}

// ==================== Chicken ====================
Chicken::Chicken(EntityID id)
    : Animal(id, 4, shared::EntityType::Chicken) {
}

void Chicken::update(float dt) {
    Animal::update(dt);
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

// ==================== Rabbit ====================
Rabbit::Rabbit(EntityID id)
    : Animal(id, 3, shared::EntityType::Rabbit) {
    // 兔子跑得快
    moveSpeed_ = 0.5f;
}

void Rabbit::update(float dt) {
    Animal::update(dt);
    updateFleeBehavior(dt);
}

void Rabbit::updateFleeBehavior(float dt) {
    // 兔子特有的逃跑行为
    (void)dt;
}

// ==================== Wolf ====================
Wolf::Wolf(EntityID id)
    : Animal(id, 8, shared::EntityType::Wolf) {
}

void Wolf::update(float dt) {
    Animal::update(dt);
    updatePackBehavior(dt);
}

void Wolf::updatePackBehavior(float dt) {
    // 狼群行为
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

// ==================== Cat ====================
Cat::Cat(EntityID id)
    : Animal(id, 10, shared::EntityType::Cat) {
    moveSpeed_ = 0.35f;
}

void Cat::update(float dt) {
    Animal::update(dt);
    updateAvoidPlayer(dt);
}

void Cat::updateAvoidPlayer(float dt) {
    // 猫躲避玩家
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

// ==================== Fox ====================
Fox::Fox(EntityID id)
    : Animal(id, 10, shared::EntityType::Fox) {
}

void Fox::update(float dt) {
    Animal::update(dt);
    updateNocturnalBehavior(dt);
}

void Fox::updateNocturnalBehavior(float dt) {
    // 狐狸夜行行为
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
