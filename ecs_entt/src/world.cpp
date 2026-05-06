#include "world.hpp"

#include <algorithm>
#include <cmath>

namespace ecs_entt {

World::World(size_t initialCapacity)
    : registry_() {
    (void)initialCapacity;
    // Note: reserve() not available in EnTT 3.x, entities are created on demand
    shared::resetSimulationClock();
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
    shared::advanceSimulationClock(dt);

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
    }

    // 运行时行为状态
    auto& behavior = registry_.emplace<BehaviorState>(entity);
    shared::initializeBehaviorState(behavior, x, y, z);
    shared::setHeadingFromStableAngle(type, behavior, shared::simulationContext().elapsedTime, ai.wanderDirX, ai.wanderDirZ);

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
    auto view = registry.view<Transform, AI, Attributes, TypeTag, BehaviorState>();

    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);
        auto& tag = view.get<TypeTag>(entity);
        auto& behavior = view.get<BehaviorState>(entity);

        if (registry.all_of<Health>(entity)) {
            const auto& health = registry.get<Health>(entity);
            if (health.isDead) {
                continue;
            }
        }

        const auto entityType = static_cast<shared::EntityType>(tag.type);
        const auto& profile = shared::getBehaviorProfile(entityType);

        float dx = 0.0f, dz = 0.0f;
        float speed = attr.moveSpeed;

        // 水生生物在水下使用游泳速度
        if (tag.isAquatic && transform.y < shared::WorldConstants::WATER_LEVEL) {
            speed = attr.swimSpeed;
        }

        auto& behaviorState = behavior;
        const auto& context = shared::simulationContext();
        float healthPercent = 1.0f;
        if (registry.all_of<Health>(entity)) {
            const auto& health = registry.get<Health>(entity);
            if (health.maximum > 0) {
                healthPercent = static_cast<float>(health.current) / static_cast<float>(health.maximum);
            }
        }

        float speedScale = shared::computeActivityMultiplier(profile, behaviorState, context, static_cast<shared::AIState>(ai.state), healthPercent, tag.isFlying, tag.isAquatic);
        speed *= speedScale;

        switch (static_cast<shared::AIState>(ai.state)) {
            case shared::AIState::Wander:
                dx = ai.wanderDirX * speed * dt;
                dz = ai.wanderDirZ * speed * dt;
                break;
            case shared::AIState::Chase: {
                // 地面生物 1.35f，飞行/水生生物 1.5f
                float chaseCoeff = (tag.isFlying || tag.isAquatic) ? 1.5f : 1.35f;
                dx = ai.wanderDirX * speed * chaseCoeff * dt;
                dz = ai.wanderDirZ * speed * chaseCoeff * dt;
                break;
            }
            case shared::AIState::Flee: {
                // 地面生物 1.6f，飞行/水生生物 1.5f
                float fleeCoeff = (tag.isFlying || tag.isAquatic) ? 1.5f : 1.6f;
                dx = ai.wanderDirX * speed * fleeCoeff * dt;
                dz = ai.wanderDirZ * speed * fleeCoeff * dt;
                break;
            }
            case shared::AIState::Attack: {
                // 地面生物 0.35f，飞行/水生生物 0.4f
                float attackCoeff = (tag.isFlying || tag.isAquatic) ? 0.4f : 0.35f;
                dx = ai.wanderDirX * speed * attackCoeff * dt;
                dz = ai.wanderDirZ * speed * attackCoeff * dt;
                break;
            }
            default:
                break;
        }

        if (profile.homeBias > 0.0f) {
            float homeDx = behaviorState.homeX - transform.x;
            float homeDz = behaviorState.homeZ - transform.z;
            float homeDistSq = homeDx * homeDx + homeDz * homeDz;
            if (homeDistSq > 1.0f) {
                float homeDist = std::sqrt(homeDistSq);
                // 地面生物 0.12f，飞行/水生生物 0.08f
                float homeBiasCoeff = (tag.isFlying || tag.isAquatic) ? 0.08f : 0.12f;
                float homeStep = speed * profile.homeBias * homeBiasCoeff * dt;
                dx += (homeDx / homeDist) * homeStep;
                dz += (homeDz / homeDist) * homeStep;
            }
        }

        transform.x += dx;
        transform.z += dz;

        if (tag.isFlying) {
            float targetHeight = attr.flightHeight;
            targetHeight += profile.verticalBias * 4.0f;
            if (shared::simulationContext().isNight) {
                targetHeight += profile.nightAffinity * 1.5f;
            }
            float heightDiff = targetHeight - transform.y;
            if (std::abs(heightDiff) > 0.1f) {
                // 使用 flightSpeed (等于 moveSpeed) * dt，不包含 speedScale
                transform.y += std::copysign(attr.moveSpeed * dt, heightDiff);
            }
            transform.y = std::clamp(transform.y, 1.0f, shared::WorldConstants::WORLD_SIZE_Y);
        } else if (tag.isAquatic) {
            // 水生生物 Y 调整由 updateSpecialBehavior 统一处理
            transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        } else {
            transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        }

        // 共享特殊行为：同一套规则在 OOP 和 ECS 中保持一致。
        float targetDistanceSq = 1.0e30f;
        bool hasTarget = ai.target != NullEntity && registry.valid(ai.target);
        if (hasTarget && registry.all_of<Transform>(ai.target)) {
            const auto& targetTransform = registry.get<Transform>(ai.target);
            float dxTarget = targetTransform.x - transform.x;
            float dyTarget = targetTransform.y - transform.y;
            float dzTarget = targetTransform.z - transform.z;
            targetDistanceSq = dxTarget * dxTarget + dyTarget * dyTarget + dzTarget * dzTarget;
        }

        shared::AIState currentState = static_cast<shared::AIState>(ai.state);
        shared::AIState specialState = currentState;

        if (shared::updateSpecialBehavior(
                entityType,
                shared::getEntityConfig(entityType),
                profile,
                behaviorState,
            specialState,
                transform.x,
                transform.y,
                transform.z,
                ai.wanderDirX,
                ai.wanderDirZ,
                dt,
                hasTarget,
                targetDistanceSq,
                attr.attackRange * attr.attackRange,
                context) == shared::SpecialBehaviorResult::Deactivate) {
            if (registry.all_of<Health>(entity)) {
                auto& health = registry.get<Health>(entity);
                health.current = 0;
                health.isDead = true;
            }
        }

        ai.state = static_cast<uint8_t>(specialState);

        // World bounds
        transform.x = std::clamp(transform.x, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        transform.z = std::clamp(transform.z, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    }
}

// AISystem
void AISystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<AI, Attributes, Health, TypeTag, BehaviorState>();

    for (auto entity : view) {
        auto& ai = view.get<AI>(entity);
        auto& attr = view.get<Attributes>(entity);
        auto& health = view.get<Health>(entity);
        auto& tag = view.get<TypeTag>(entity);
        auto& behavior = view.get<BehaviorState>(entity);

        if (health.isDead) {
            continue;
        }

        const auto entityType = static_cast<shared::EntityType>(tag.type);
        const auto& profile = shared::getBehaviorProfile(entityType);
        const auto& context = shared::simulationContext();

        shared::AIState currentState = static_cast<shared::AIState>(ai.state);
        float healthPercent = health.maximum > 0 ? static_cast<float>(health.current) / static_cast<float>(health.maximum) : 1.0f;

        shared::updateBehaviorState(
            behavior,
            profile,
            currentState,
            dt,
            currentState == shared::AIState::Wander ||
                currentState == shared::AIState::Chase ||
                currentState == shared::AIState::Flee ||
                currentState == shared::AIState::Attack,
            healthPercent,
            context);

        // Attack cooldown
        if (attr.attackCooldownTimer > 0.0f) {
            attr.attackCooldownTimer -= dt;
        }

        bool hasTarget = ai.target != NullEntity && registry.valid(ai.target);
        shared::AIState nextState = shared::chooseAIState(
            currentState,
            profile,
            behavior,
            context,
            healthPercent,
            hasTarget,
            hasTarget,
            tag.isMonster,
            tag.isAnimal,
            tag.isFlying,
            tag.isAquatic);

        if (!hasTarget && currentState == shared::AIState::Chase && nextState != shared::AIState::Chase) {
            ai.target = NullEntity;
        }

        // 状态改变时重置 roamTimer
        if (currentState != nextState) {
            behavior.roamTimer = 0.0f;
        }

        ai.state = static_cast<uint8_t>(nextState);
        ai.stateTimer += dt;
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
    (void)dt;
    auto hostiles = registry.view<Transform, AI, Attributes, Health, TypeTag, BehaviorState>();
    auto passives = registry.view<Transform, AI, TypeTag, Health>();

    const auto& context = shared::simulationContext();

    for (auto entity : hostiles) {
        auto& transform = hostiles.get<Transform>(entity);
        auto& ai = hostiles.get<AI>(entity);
        auto& attr = hostiles.get<Attributes>(entity);
        auto& health = hostiles.get<Health>(entity);
        auto& behavior = hostiles.get<BehaviorState>(entity);

        auto entityType = static_cast<shared::EntityType>(hostiles.get<TypeTag>(entity).type);

        if (health.isDead) {
            continue;
        }

        const auto& profile = shared::getBehaviorProfile(entityType);

        // Find passive targets for hostile entities
        if (shared::isMonster(entityType) && ai.target == NullEntity) {
            float rangeScale = 1.0f + profile.aggression * 0.15f + context.lightLevel * 0.05f;
            float range = ai.perceptionRange * rangeScale;
            float rangeSq = range * range;

            Entity bestTarget = NullEntity;
            float bestScore = -1.0f;

            for (auto target : passives) {
                if (entity == target) continue;

                auto& targetTransform = passives.get<Transform>(target);
                auto& tag = passives.get<TypeTag>(target);
                auto& targetHealth = passives.get<Health>(target);

                if (tag.isAnimal && !targetHealth.isDead) {
                    float dx = targetTransform.x - transform.x;
                    float dy = targetTransform.y - transform.y;
                    float dz = targetTransform.z - transform.z;
                    float distSq = dx * dx + dy * dy + dz * dz;
                    if (distSq > rangeSq) continue;

                    float targetHealthPercent = targetHealth.maximum > 0 ? static_cast<float>(targetHealth.current) / static_cast<float>(targetHealth.maximum) : 1.0f;
                    float score = shared::scoreTargetCandidate(
                        shared::getBehaviorProfile(entityType),
                        context,
                        distSq,
                        targetHealthPercent,
                        true,
                        false,
                        tag.isFlying,
                        tag.isAquatic);

                    if (score > bestScore) {
                        bestScore = score;
                        bestTarget = target;
                    }
                }
            }

            if (bestTarget != NullEntity) {
                ai.target = bestTarget;
                ai.state = static_cast<uint8_t>(shared::AIState::Chase);
                auto& targetTransform = passives.get<Transform>(bestTarget);
                shared::setHeadingFromVector(targetTransform.x - transform.x, targetTransform.z - transform.z, ai.wanderDirX, ai.wanderDirZ);
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
                        float damageScale = shared::computeAttackDamageScale(
                            shared::getBehaviorProfile(entityType),
                            behavior,
                            context,
                            distSq,
                            rangeSq);
                        int damage = std::max(1, static_cast<int>(std::round(attr.attackDamage * damageScale)));
                        targetHealth.current -= damage;
                        if (targetHealth.current <= 0) {
                            targetHealth.isDead = true;
                        }
                        attr.attackCooldownTimer = attr.attackCooldown;

                        if (targetHealth.isDead) {
                            ai.target = NullEntity;
                            ai.state = static_cast<uint8_t>(shared::AIState::Wander);
                        }
                    }
                } else if (distSq > shared::WorldConstants::DEAGGRO_RANGE * shared::WorldConstants::DEAGGRO_RANGE) {
                    ai.target = NullEntity;
                    ai.state = static_cast<uint8_t>(shared::AIState::Wander);
                }
            } else {
                ai.target = NullEntity;
                ai.state = static_cast<uint8_t>(shared::AIState::Wander);
            }
        }

    }

    for (auto entity : passives) {
        auto& transform = passives.get<Transform>(entity);
        auto& ai = passives.get<AI>(entity);
        auto& tag = passives.get<TypeTag>(entity);
        auto& health = passives.get<Health>(entity);

        if (!tag.isAnimal || health.isDead) {
            continue;
        }

        const auto& profile = shared::getBehaviorProfile(static_cast<shared::EntityType>(tag.type));
        float perceptionRange = ai.perceptionRange * (1.0f + profile.caution * 0.15f);
        float perceptionRangeSq = perceptionRange * perceptionRange;

        Entity nearestThreat = NullEntity;
        float nearestThreatDistSq = perceptionRangeSq;

        for (auto hostile : hostiles) {
            auto& hostileTransform = hostiles.get<Transform>(hostile);
            auto& hostileTag = hostiles.get<TypeTag>(hostile);
            auto& hostileHealth = hostiles.get<Health>(hostile);

            if (!hostileTag.isMonster || hostileHealth.isDead) {
                continue;
            }

            float dx = hostileTransform.x - transform.x;
            float dy = hostileTransform.y - transform.y;
            float dz = hostileTransform.z - transform.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq < nearestThreatDistSq) {
                nearestThreatDistSq = distSq;
                nearestThreat = hostile;
            }
        }

        if (nearestThreat != NullEntity || (health.maximum > 0 && static_cast<float>(health.current) / static_cast<float>(health.maximum) <= profile.fleeHealthThreshold)) {
            if (nearestThreat != NullEntity) {
                auto& threatTransform = hostiles.get<Transform>(nearestThreat);
                shared::setHeadingFromVector(transform.x - threatTransform.x, transform.z - threatTransform.z, ai.wanderDirX, ai.wanderDirZ);
            }

            ai.state = static_cast<uint8_t>(shared::AIState::Flee);
        }
    }
}

} // namespace ecs_entt
