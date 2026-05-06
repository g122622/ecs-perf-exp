#include "world.hpp"
#include <cmath>
#include <algorithm>

namespace ecs_entt {

World::World(size_t initialCapacity)
    : rng_(shared::RandomConstants::DEFAULT_SEED) {
    // Note: reserve() not available in EnTT 3.x, entities are created on demand
}

Entity World::createEntity(shared::EntityType type, float x, float y, float z) {
    Entity entity = registry_.create();
    createEntityComponents(entity, type, x, y, z);
    return entity;
}

void World::destroyEntity(Entity entity) {
    registry_.destroy(entity);
}

bool World::isAlive(Entity entity) const {
    return registry_.valid(entity);
}

void World::update(float dt) {
    // Update in order: AI -> Movement -> Physics -> Decay -> Interaction
    AISystem::update(registry_, dt);
    MovementSystem::update(registry_, dt);
    PhysicsSystem::update(registry_, dt);
    DecaySystem::update(registry_, dt);
    InteractionSystem::update(registry_, dt);
}

void World::createEntityComponents(Entity entity, shared::EntityType type,
                                    float x, float y, float z) {
    const auto& config = shared::getEntityConfig(type);

    // Transform
    auto& transform = registry_.emplace<Transform>(entity);
    transform.x = x;
    transform.y = y;
    transform.z = z;

    // Physics
    registry_.emplace<Physics>(entity);

    // Health
    auto& health = registry_.emplace<Health>(entity);
    health.current = config.maxHealth;
    health.maximum = config.maxHealth;
    health.regenRate = config.regenRate;

    // AI
    auto& ai = registry_.emplace<AI>(entity);
    ai.state = static_cast<uint8_t>(config.defaultState);
    ai.perceptionRange = config.perceptionRange;

    // Attributes
    auto& attr = registry_.emplace<Attributes>(entity);
    attr.moveSpeed = config.moveSpeed;
    attr.attackDamage = config.attackDamage;
    attr.attackRange = config.attackRange;
    attr.attackCooldown = config.attackCooldown;
    attr.flightHeight = config.flightHeight;
    attr.swimSpeed = config.swimSpeed;
    attr.entityType = static_cast<uint16_t>(type);
    attr.behaviorFlags = static_cast<uint8_t>(config.behavior);

    // TypeTag
    auto& tag = registry_.emplace<TypeTag>(entity);
    tag.type = static_cast<uint16_t>(type);
    tag.isMonster = shared::isMonster(type);
    tag.isAnimal = shared::isAnimal(type);
    tag.isFlying = shared::isFlying(type);
    tag.isAquatic = shared::isAquatic(type);
}

// MovementSystem
void MovementSystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, AI, Attributes>();

    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);

        float dx = 0.0f, dz = 0.0f;
        float speed = attr.moveSpeed;

        switch (ai.state) {
            case 1: // Wander
                if (ai.wanderDirX == 0.0f && ai.wanderDirZ == 0.0f) {
                    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
                    ai.wanderDirX = std::cos(angle);
                    ai.wanderDirZ = std::sin(angle);
                }
                dx = ai.wanderDirX * speed * dt;
                dz = ai.wanderDirZ * speed * dt;
                break;
            case 2: // Chase
                if (ai.target != NullEntity && registry.valid(ai.target)) {
                    auto& targetTransform = registry.get<Transform>(ai.target);
                    float dirX = targetTransform.x - transform.x;
                    float dirZ = targetTransform.z - transform.z;
                    float dist = std::sqrt(dirX * dirX + dirZ * dirZ);
                    if (dist > 0.001f) {
                        dx = (dirX / dist) * speed * dt;
                        dz = (dirZ / dist) * speed * dt;
                    }
                }
                break;
            case 3: // Flee
                if (ai.wanderDirX == 0.0f && ai.wanderDirZ == 0.0f) {
                    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
                    ai.wanderDirX = std::cos(angle);
                    ai.wanderDirZ = std::sin(angle);
                }
                dx = ai.wanderDirX * speed * 1.5f * dt;
                dz = ai.wanderDirZ * speed * 1.5f * dt;
                break;
            default:
                break;
        }

        // Flying entities
        if (attr.flightHeight > 0.0f) {
            float heightDiff = attr.flightHeight - transform.y;
            if (std::abs(heightDiff) > 0.1f) {
                transform.y += std::copysign(attr.moveSpeed * dt, heightDiff);
            }
        }

        // Aquatic entities
        if (attr.swimSpeed > 0.0f) {
            speed = attr.swimSpeed;
        }

        transform.x += dx;
        transform.z += dz;

        // World bounds
        transform.x = std::clamp(transform.x, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        transform.z = std::clamp(transform.z, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    }
}

// PhysicsSystem
void PhysicsSystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, Physics>();

    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& physics = view.get<Physics>(entity);

        physics.vx += physics.ax * dt;
        physics.vy += physics.ay * dt;
        physics.vz += physics.az * dt;

        transform.x += physics.vx * dt;
        transform.y += physics.vy * dt;
        transform.z += physics.vz * dt;

        physics.vx *= physics.drag;
        physics.vy *= physics.drag;
        physics.vz *= physics.drag;

        physics.ax = 0.0f;
        physics.ay = 0.0f;
        physics.az = 0.0f;
    }
}

// AISystem
void AISystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<AI, Attributes>();

    for (auto entity : view) {
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);

        ai.stateTimer += dt;

        // Attack cooldown
        if (attr.attackCooldownTimer > 0.0f) {
            attr.attackCooldownTimer -= dt;
        }

        // State machine
        switch (ai.state) {
            case 0: // Idle
                if (ai.stateTimer > shared::WorldConstants::IDLE_DURATION_MAX) {
                    ai.state = 1; // Wander
                    ai.stateTimer = 0.0f;
                }
                break;
            case 1: // Wander
                if (ai.stateTimer > shared::WorldConstants::WANDER_DURATION_MAX) {
                    ai.state = 0; // Idle
                    ai.stateTimer = 0.0f;
                    ai.wanderDirX = 0.0f;
                    ai.wanderDirZ = 0.0f;
                }
                break;
            case 2: // Chase
                if (ai.target == NullEntity || !registry.valid(ai.target)) {
                    ai.state = 1;
                    ai.stateTimer = 0.0f;
                } else if (ai.stateTimer > shared::WorldConstants::CHASE_DURATION_MAX) {
                    ai.target = NullEntity;
                    ai.state = 1;
                    ai.stateTimer = 0.0f;
                }
                break;
            case 3: // Flee
                if (ai.stateTimer > shared::WorldConstants::FLEE_DURATION_MAX) {
                    ai.state = 1;
                    ai.stateTimer = 0.0f;
                    ai.wanderDirX = 0.0f;
                    ai.wanderDirZ = 0.0f;
                }
                break;
        }
    }
}

// DecaySystem
void DecaySystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<Health>();

    for (auto entity : view) {
        auto& health = view.get<Health>(entity);

        if (health.regenRate > 0.0f && health.current < health.maximum) {
            float healAmount = health.regenRate * dt;
            health.current = static_cast<int>(health.current + healAmount);
            if (health.current > health.maximum) {
                health.current = health.maximum;
            }
        }
    }
}

// InteractionSystem
void InteractionSystem::update(entt::registry& registry, float dt) {
    auto hostiles = registry.view<Transform, AI, Attributes, Health>();
    auto passives = registry.view<Transform, TypeTag, Health>();

    for (auto entity : hostiles) {
        auto& transform = hostiles.get<Transform>(entity);
        auto& ai = hostiles.get<AI>(entity);
        auto& attr = hostiles.get<Attributes>(entity);
        auto& health = hostiles.get<Health>(entity);

        // Find passive targets for hostile entities
        if ((attr.behaviorFlags & 0x01) && ai.target == NullEntity) { // Hostile
            for (auto target : passives) {
                if (entity == target) continue;

                auto& targetTransform = passives.get<Transform>(target);
                auto& tag = passives.get<TypeTag>(target);
                auto& targetHealth = passives.get<Health>(target);

                if (tag.isAnimal) {
                    float dx = targetTransform.x - transform.x;
                    float dy = targetTransform.y - transform.y;
                    float dz = targetTransform.z - transform.z;
                    float distSq = dx * dx + dy * dy + dz * dz;

                    if (distSq <= ai.perceptionRange * ai.perceptionRange) {
                        ai.target = target;
                        ai.state = 2; // Chase
                        break;
                    }
                }
            }
        }

        // Attack logic
        if (ai.target != NullEntity && attr.attackCooldownTimer <= 0.0f) {
            if (registry.valid(ai.target)) {
                auto& targetTransform = registry.get<Transform>(ai.target);
                float dx = targetTransform.x - transform.x;
                float dy = targetTransform.y - transform.y;
                float dz = targetTransform.z - transform.z;
                float distSq = dx * dx + dy * dy + dz * dz;

                float rangeSq = attr.attackRange * attr.attackRange;

                if (distSq <= rangeSq) {
                    if (registry.all_of<Health>(ai.target)) {
                        auto& targetHealth = registry.get<Health>(ai.target);
                        targetHealth.current -= static_cast<int>(attr.attackDamage);
                        if (targetHealth.current <= 0) {
                            targetHealth.isDead = true;
                        }
                        attr.attackCooldownTimer = attr.attackCooldown;

                        if (targetHealth.isDead) {
                            ai.target = NullEntity;
                            ai.state = 1;
                        }
                    }
                } else if (distSq > shared::WorldConstants::DEAGGRO_RANGE * shared::WorldConstants::DEAGGRO_RANGE) {
                    ai.target = NullEntity;
                    ai.state = 1;
                }
            } else {
                ai.target = NullEntity;
                ai.state = 1;
            }
        }
    }
}

} // namespace ecs_entt
