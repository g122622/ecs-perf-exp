#include "world.hpp"
#include "shared/random_utils.hpp"
#include <cmath>
#include <algorithm>

namespace ecs_entt {

World::World(size_t initialCapacity)
    : registry_() {
    (void)initialCapacity;
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
    // Update in order: AI -> Movement -> Decay -> Interaction
    AISystem::update(registry_, dt);
    MovementSystem::update(registry_, dt);
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

    // Health
    auto& health = registry_.emplace<Health>(entity);
    health.current = config.maxHealth;
    health.maximum = config.maxHealth;
    health.regenRate = config.regenRate;

    // AI
    auto& ai = registry_.emplace<AI>(entity);
    ai.state = static_cast<uint8_t>(config.defaultState);
    ai.perceptionRange = config.perceptionRange;
    if (shared::isFlying(type)) {
        ai.stateDuration = config.flightHeight;
    } else if (type == shared::EntityType::Fish) {
        ai.stateDuration = shared::randomAngle();
    }

    // Attributes
    auto& attr = registry_.emplace<Attributes>(entity);
    attr.moveSpeed = config.moveSpeed;
    attr.attackDamage = static_cast<float>(config.attackDamage);
    attr.attackRange = config.attackRange;
    attr.attackCooldown = config.attackCooldown;
    attr.flightHeight = config.flightHeight;
    attr.swimSpeed = config.swimSpeed;

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
    auto view = registry.view<Transform, AI, Attributes, TypeTag>();

    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);
        auto& tag = view.get<TypeTag>(entity);

        const auto entityType = static_cast<shared::EntityType>(tag.type);

        float dx = 0.0f, dz = 0.0f;
        float speed = attr.moveSpeed;
        const bool isBat = entityType == shared::EntityType::Bat;
        const bool isFish = entityType == shared::EntityType::Fish;

        if (isBat || isFish) {
            if (ai.stateTimer > (isBat ? 2.0f : 1.0f)) {
                if (isBat) {
                    float angle = shared::randomAngle();
                    ai.wanderDirX = std::cos(angle);
                    ai.wanderDirZ = std::sin(angle);
                    ai.stateDuration = 5.0f + shared::randomFloat(0.0f, 10.0f);
                } else {
                    ai.stateDuration += shared::randomFloat(-0.25f, 0.25f);
                    ai.wanderDirX = std::cos(ai.stateDuration);
                    ai.wanderDirZ = std::sin(ai.stateDuration);
                }

                ai.stateTimer = 0.0f;
            }

            if (isFish) {
                speed = (transform.y < shared::WorldConstants::WATER_LEVEL) ? attr.swimSpeed : attr.moveSpeed;
            }

            switch (ai.state) {
                case 1: // Wander
                    dx = ai.wanderDirX * speed * dt;
                    dz = ai.wanderDirZ * speed * dt;
                    break;
                case 2: // Chase
                case 3: // Flee
                    dx = ai.wanderDirX * speed * 1.5f * dt;
                    dz = ai.wanderDirZ * speed * 1.5f * dt;
                    break;
                default:
                    break;
            }
        }

        transform.x += dx;
        transform.z += dz;

        if (tag.isFlying) {
            float targetHeight = (entityType == shared::EntityType::Bat) ? ai.stateDuration : attr.flightHeight;
            float heightDiff = targetHeight - transform.y;
            if (std::abs(heightDiff) > 0.1f) {
                transform.y += std::copysign(attr.moveSpeed * dt, heightDiff);
            }
            transform.y = std::clamp(transform.y, 1.0f, shared::WorldConstants::WORLD_SIZE_Y);
        } else {
            transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        }

        // World bounds
        transform.x = std::clamp(transform.x, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        transform.z = std::clamp(transform.z, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    }
}

// AISystem
void AISystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<AI, Attributes, Health, TypeTag>();

    for (auto entity : view) {
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);
        auto& health = view.get<Health>(entity);
        auto& tag = view.get<TypeTag>(entity);

        ai.stateTimer += dt;

        // Attack cooldown
        if (attr.attackCooldownTimer > 0.0f) {
            attr.attackCooldownTimer -= dt;
        }

        // State machine
        if (tag.isMonster) {
            switch (ai.state) {
                case 0: // Idle
                    if (ai.stateTimer > shared::WorldConstants::IDLE_DURATION_MAX) {
                        ai.state = 1; // Wander
                        ai.stateTimer = 0.0f;
                    }
                    break;
                case 1: // Wander
                    if (ai.target != NullEntity && registry.valid(ai.target)) {
                        ai.state = 2; // Chase
                        ai.stateTimer = 0.0f;
                    } else if (ai.stateTimer > shared::WorldConstants::WANDER_DURATION_MAX) {
                        ai.state = 0; // Idle
                        ai.stateTimer = 0.0f;
                    }
                    break;
                case 2: // Chase
                    if (ai.target == NullEntity || !registry.valid(ai.target)) {
                        ai.target = NullEntity;
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
                    }
                    break;
            }
        } else if (tag.isAnimal) {
            switch (ai.state) {
                case 0: // Idle
                    if (ai.stateTimer > shared::WorldConstants::IDLE_DURATION_MAX * 0.5f) {
                        ai.state = 1; // Wander
                        ai.stateTimer = 0.0f;
                    }
                    break;
                case 1: // Wander
                    if (ai.stateTimer > shared::WorldConstants::WANDER_DURATION_MAX) {
                        ai.state = 0; // Idle
                        ai.stateTimer = 0.0f;
                    }
                    break;
                case 3: // Flee
                    if (ai.stateTimer > shared::WorldConstants::FLEE_DURATION_MAX) {
                        ai.state = 1;
                        ai.stateTimer = 0.0f;
                    }
                    break;
            }

            if (health.maximum > 0) {
                float healthPercent = static_cast<float>(health.current) / static_cast<float>(health.maximum);
                if (healthPercent < 0.3f && ai.state != 3) {
                    ai.state = 3; // Flee
                    ai.stateTimer = 0.0f;
                }
            }
        } else {
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
                    }
                    break;
                default:
                    break;
            }
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
    auto hostiles = registry.view<Transform, AI, Attributes, Health, TypeTag>();
    auto passives = registry.view<Transform, TypeTag, Health>();

    for (auto entity : hostiles) {
        auto& transform = hostiles.get<Transform>(entity);
        auto& ai = hostiles.get<AI>(entity);
        auto& attr = hostiles.get<Attributes>(entity);

        auto entityType = static_cast<shared::EntityType>(hostiles.get<TypeTag>(entity).type);

        // Find passive targets for hostile entities
        if (shared::isMonster(entityType) && ai.target == NullEntity) {
            for (auto target : passives) {
                if (entity == target) continue;

                auto& targetTransform = passives.get<Transform>(target);
                auto& tag = passives.get<TypeTag>(target);

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
