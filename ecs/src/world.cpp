#include "world.hpp"
#include "systems/movement_system.hpp"
#include "systems/physics_system.hpp"
#include "systems/ai_system.hpp"
#include "systems/decay_system.hpp"
#include "systems/interaction_system.hpp"
#include <cmath>

namespace ecs {

World::World(size_t initialCapacity)
    : entities_(initialCapacity)
    , transforms_(initialCapacity)
    , physics_(initialCapacity)
    , health_(initialCapacity)
    , aiStates_(initialCapacity)
    , attributes_(initialCapacity)
    , typeTags_(initialCapacity)
    , rng_(shared::RandomConstants::DEFAULT_SEED) {
    initializeSystems();
}

void World::initializeSystems() {
    // 按顺序添加系统
    systems_.push_back(std::make_unique<AISystem>(entities_, transforms_, aiStates_, attributes_, health_));
    systems_.push_back(std::make_unique<MovementSystem>(entities_, transforms_, physics_, aiStates_, attributes_));
    systems_.push_back(std::make_unique<PhysicsSystem>(entities_, transforms_, physics_));
    systems_.push_back(std::make_unique<DecaySystem>(entities_, health_));
    systems_.push_back(std::make_unique<InteractionSystem>(entities_, transforms_, aiStates_, attributes_, health_));
}

EntityID World::createEntity(shared::EntityType type, float x, float y, float z) {
    EntityID id = entities_.createEntity();
    createEntityComponents(id, type, x, y, z);
    return id;
}

void World::createEntityComponents(EntityID id, shared::EntityType type,
                                    float x, float y, float z) {
    const auto& config = shared::getEntityConfig(type);

    // 变换组件
    transforms_.add(id, x, y, z);

    // 物理组件
    physics_.add(id);

    // 生命值组件
    health_.add(id, config.maxHealth, config.regenRate);

    // AI组件
    aiStates_.add(id, config.defaultState, config.perceptionRange);

    // 属性组件
    AttributesComponent& attr = attributes_.add(id);
    attr.moveSpeed = config.moveSpeed;
    attr.attackDamage = config.attackDamage;
    attr.attackRange = config.attackRange;
    attr.attackCooldown = config.attackCooldown;
    attr.flightHeight = config.flightHeight;
    attr.swimSpeed = config.swimSpeed;
    attr.entityType = type;
    attr.behavior = config.behavior;

    // 类型标签
    typeTags_.add(id, type);
}

void World::destroyEntity(EntityID id) {
    entities_.destroyEntity(id);
    transforms_.remove(id);
    physics_.remove(id);
    health_.remove(id);
    aiStates_.remove(id);
    attributes_.remove(id);
    typeTags_.remove(id);
}

bool World::isEntityAlive(EntityID id) const {
    return entities_.isEntityAlive(id);
}

TransformComponent* World::getTransform(EntityID id) {
    return transforms_.get(id);
}

PhysicsComponent* World::getPhysics(EntityID id) {
    return physics_.get(id);
}

HealthComponent* World::getHealth(EntityID id) {
    return health_.get(id);
}

AIComponent* World::getAI(EntityID id) {
    return aiStates_.get(id);
}

AttributesComponent* World::getAttributes(EntityID id) {
    return attributes_.get(id);
}

TypeTagComponent* World::getTypeTag(EntityID id) {
    return typeTags_.get(id);
}

void World::update(float dt) {
    for (auto& system : systems_) {
        system->update(dt);
    }
}

void World::findEntitiesInRange(float x, float y, float z, float range,
                                 std::vector<EntityID>& outIds) const {
    float rangeSq = range * range;

    transforms_.forEach([&](EntityID id, const TransformComponent& t) {
        float dx = t.x - x;
        float dy = t.y - y;
        float dz = t.z - z;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq <= rangeSq) {
            outIds.push_back(id);
        }
    });
}

EntityID World::findNearestEntity(float x, float y, float z, float maxRange,
                                   std::function<bool(EntityID)> filter) const {
    EntityID nearestId = INVALID_ENTITY_ID;
    float nearestDistSq = maxRange * maxRange;

    transforms_.forEach([&](EntityID id, const TransformComponent& t) {
        if (filter && !filter(id)) return;

        float dx = t.x - x;
        float dy = t.y - y;
        float dz = t.z - z;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearestId = id;
        }
    });

    return nearestId;
}

void World::clear() {
    entities_.clear();
    transforms_.clear();
    physics_.clear();
    health_.clear();
    aiStates_.clear();
    attributes_.clear();
    typeTags_.clear();
}

} // namespace ecs
