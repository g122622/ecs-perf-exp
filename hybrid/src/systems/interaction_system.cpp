#include "world.hpp"
#include "shared/spatial_grid.hpp"
#include <cmath>

namespace hybrid {

void InteractionSystem::update(entt::registry& registry, HybridWorld& world, [[maybe_unused]] float dt) {
    const auto& context = shared::simulationContext();

    // Build spatial grid
    shared::SpatialGrid grid;

    auto allEntities = registry.view<TransformComponent, TypeTagComponent, EntityLink>();
    for (auto entity : allEntities) {
        auto& transform = allEntities.get<TransformComponent>(entity);
        auto& tag = allEntities.get<TypeTagComponent>(entity);
        auto& link = allEntities.get<EntityLink>(entity);

        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity) continue;

        shared::SpatialGrid::EntityEntry entry{};
        entry.id = static_cast<uint32_t>(entity);
        entry.x = transform.x;
        entry.y = transform.y;
        entry.z = transform.z;
        entry.isMonster = tag.isMonster;
        entry.isAnimal = tag.isAnimal;
        entry.isFlying = tag.isFlying;
        entry.isAquatic = tag.isAquatic;
        entry.isDead = oopEntity->isDead();
        entry.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(link.oopIndex));

        grid.insert(entry);
    }

    // Decay (health regeneration)
    for (auto entity : registry.view<EntityLink>()) {
        auto& link = registry.get<EntityLink>(entity);
        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity || oopEntity->isDead()) continue;

        const auto& config = shared::getEntityConfig(
            static_cast<shared::EntityType>(registry.get<TypeTagComponent>(entity).type));

        if (config.regenRate > 0.0f && oopEntity->currentHealth() < oopEntity->maximumHealth()) {
            float healAmount = config.regenRate * dt;
            oopEntity->heal(static_cast<int>(healAmount));
        }
    }

    // Process monster interactions
    auto hostiles = registry.view<TransformComponent, AIComponent, AttributesComponent, TypeTagComponent, EntityLink>();
    for (auto entity : hostiles) {
        auto& transform = hostiles.get<TransformComponent>(entity);
        auto& ai = hostiles.get<AIComponent>(entity);
        auto& attr = hostiles.get<AttributesComponent>(entity);
        auto& tag = hostiles.get<TypeTagComponent>(entity);
        auto& link = hostiles.get<EntityLink>(entity);

        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity || oopEntity->isDead()) continue;

        auto entityType = static_cast<shared::EntityType>(tag.type);
        if (!shared::isMonster(entityType)) continue;

        const auto& profile = shared::getBehaviorProfile(entityType);

        // Find targets
        if (ai.target == NullEntity) {
            float rangeScale = 1.0f + profile.aggression * 0.15f + context.lightLevel * 0.05f;
            float range = ai.perceptionRange * rangeScale;
            float rangeSq = range * range;

            Entity bestTarget = NullEntity;
            float bestScore = -1.0f;

            auto nearby = grid.query(transform.x, transform.z, range);
            for (const auto& candidate : nearby) {
                if (candidate.id == static_cast<uint32_t>(entity)) continue;
                if (!candidate.isAnimal || candidate.isDead) continue;

                float dx = candidate.x - transform.x;
                float dy = candidate.y - transform.y;
                float dz = candidate.z - transform.z;
                float distSq = dx * dx + dy * dy + dz * dz;
                if (distSq > rangeSq) continue;

                // Get target health
                entt::entity targetEntity = static_cast<entt::entity>(candidate.id);
                float targetHealthPercent = 1.0f;
                if (registry.all_of<EntityLink>(targetEntity)) {
                    auto& targetLink = registry.get<EntityLink>(targetEntity);
                    auto* targetOop = world.getOopEntity(targetLink.oopIndex);
                    if (targetOop) {
                        targetHealthPercent = targetOop->getHealthPercent();
                    }
                }

                float score = shared::scoreTargetCandidate(
                    profile, context, distSq, targetHealthPercent,
                    true, false, candidate.isFlying, candidate.isAquatic);

                if (score > bestScore) {
                    bestScore = score;
                    bestTarget = targetEntity;
                }
            }

            if (bestTarget != NullEntity) {
                ai.target = bestTarget;
                ai.state = static_cast<uint8_t>(shared::AIState::Chase);
                if (registry.all_of<TransformComponent>(bestTarget)) {
                    auto& targetTransform = registry.get<TransformComponent>(bestTarget);
                    shared::setHeadingFromVector(
                        targetTransform.x - transform.x,
                        targetTransform.z - transform.z,
                        ai.wanderDirX, ai.wanderDirZ);
                }
            }
        }

        // Attack logic
        if (ai.target != NullEntity && attr.attackCooldownTimer <= 0.0f) {
            if (registry.valid(ai.target)) {
                auto& targetTransform = registry.get<TransformComponent>(ai.target);
                float dx = targetTransform.x - transform.x;
                float dy = targetTransform.y - transform.y;
                float dz = targetTransform.z - transform.z;
                float distSq = dx * dx + dy * dy + dz * dz;

                float rangeSq = attr.attackRange * attr.attackRange;

                if (distSq <= rangeSq) {
                    if (registry.all_of<EntityLink>(ai.target)) {
                        auto& targetLink = registry.get<EntityLink>(ai.target);
                        auto* targetOop = world.getOopEntity(targetLink.oopIndex);
                        if (targetOop && !targetOop->isDead()) {
                            float damageScale = shared::computeAttackDamageScale(
                                profile, oopEntity->behaviorState(), context, distSq, rangeSq);
                            int damage = std::max(1, static_cast<int>(std::round(attr.attackDamage * damageScale)));
                            targetOop->takeDamage(damage);

                            attr.attackCooldownTimer = attr.attackCooldown;
                            shared::setHeadingFromVector(dx, dz, ai.wanderDirX, ai.wanderDirZ);

                            if (targetOop->isDead()) {
                                ai.target = NullEntity;
                                ai.state = static_cast<uint8_t>(shared::AIState::Wander);
                            }
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

    // Process animal fleeing
    auto passives = registry.view<TransformComponent, AIComponent, TypeTagComponent, EntityLink>();
    for (auto entity : passives) {
        auto& transform = passives.get<TransformComponent>(entity);
        auto& ai = passives.get<AIComponent>(entity);
        auto& tag = passives.get<TypeTagComponent>(entity);
        auto& link = passives.get<EntityLink>(entity);

        if (!tag.isAnimal) continue;

        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity || oopEntity->isDead()) continue;

        const auto& profile = shared::getBehaviorProfile(static_cast<shared::EntityType>(tag.type));
        float perceptionRange = ai.perceptionRange * (1.0f + profile.caution * 0.15f);
        float perceptionRangeSq = perceptionRange * perceptionRange;

        Entity nearestThreat = NullEntity;
        float nearestThreatDistSq = perceptionRangeSq;

        auto nearby = grid.query(transform.x, transform.z, perceptionRange);
        for (const auto& candidate : nearby) {
            if (!candidate.isMonster || candidate.isDead) continue;

            float dx = candidate.x - transform.x;
            float dy = candidate.y - transform.y;
            float dz = candidate.z - transform.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq < nearestThreatDistSq) {
                nearestThreatDistSq = distSq;
                nearestThreat = static_cast<entt::entity>(candidate.id);
            }
        }

        float healthPercent = oopEntity->getHealthPercent();
        if (nearestThreat != NullEntity || healthPercent <= profile.fleeHealthThreshold) {
            if (nearestThreat != NullEntity && registry.all_of<TransformComponent>(nearestThreat)) {
                auto& threatTransform = registry.get<TransformComponent>(nearestThreat);
                shared::setHeadingFromVector(
                    transform.x - threatTransform.x,
                    transform.z - threatTransform.z,
                    ai.wanderDirX, ai.wanderDirZ);
            }
            ai.state = static_cast<uint8_t>(shared::AIState::Flee);
        }
    }
}

} // namespace hybrid
