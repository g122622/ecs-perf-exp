#include "world.hpp"
#include <algorithm>

namespace hybrid {

HybridWorld::HybridWorld([[maybe_unused]] size_t initialCapacity) {
    shared::resetSimulationClock();
}

Entity HybridWorld::spawnEntity(shared::EntityType type, float x, float y, float z) {
    // Create ECS entity with components
    Entity entity = registry_.create();
    createEntityComponents(entity, type, x, y, z);

    // Create OOP entity for low-frequency data and special behaviors
    uint32_t oopIndex = nextOopIndex_++;
    auto oopEntity = createEntity(oopIndex, type);

    // Initialize behavior state
    oopEntity->behaviorState().homeX = x;
    oopEntity->behaviorState().homeY = y;
    oopEntity->behaviorState().homeZ = z;

    // Store OOP entity
    oopEntities_.push_back(std::move(oopEntity));

    // Link ECS entity to OOP entity
    auto& link = registry_.emplace<EntityLink>(entity);
    link.oopIndex = oopIndex;
    link.hasSpecialBehavior = hasSpecialBehavior(type);

    return entity;
}

void HybridWorld::destroyEntity(Entity entity) {
    registry_.destroy(entity);
}

void HybridWorld::update(float dt) {
    shared::advanceSimulationClock(dt);

    // Update in order: AI -> Movement -> Interaction
    AISystem::update(registry_, *this, dt);
    MovementSystem::update(registry_, *this, dt);
    InteractionSystem::update(registry_, *this, dt);
}

void HybridWorld::clear() {
    registry_.clear();
    oopEntities_.clear();
    nextOopIndex_ = 0;
}

void HybridWorld::createEntityComponents(Entity entity, shared::EntityType type, float x, float y, float z) {
    const auto& config = shared::getEntityConfig(type);

    // TransformComponent
    auto& transform = registry_.emplace<TransformComponent>(entity);
    transform.x = x;
    transform.y = y;
    transform.z = z;

    // AIComponent
    auto& ai = registry_.emplace<AIComponent>(entity);
    ai.state = static_cast<uint8_t>(config.defaultState);
    ai.perceptionRange = config.perceptionRange;
    if (shared::isFlying(type)) {
        ai.stateDuration = config.flightHeight;
    }

    // VelocityComponent
    auto& velocity = registry_.emplace<VelocityComponent>(entity);
    velocity.speed = config.moveSpeed;

    // TypeTagComponent
    auto& tag = registry_.emplace<TypeTagComponent>(entity);
    tag.type = static_cast<uint16_t>(type);
    tag.isMonster = shared::isMonster(type);
    tag.isAnimal = shared::isAnimal(type);
    tag.isFlying = shared::isFlying(type);
    tag.isAquatic = shared::isAquatic(type);

    // AttributesComponent
    auto& attr = registry_.emplace<AttributesComponent>(entity);
    attr.moveSpeed = config.moveSpeed;
    attr.attackDamage = static_cast<float>(config.attackDamage);
    attr.attackRange = config.attackRange;
    attr.attackCooldown = config.attackCooldown;
    attr.flightHeight = config.flightHeight;
    attr.swimSpeed = config.swimSpeed;
}

} // namespace hybrid
