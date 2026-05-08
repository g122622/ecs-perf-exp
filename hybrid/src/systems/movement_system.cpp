#include "world.hpp"
#include <cmath>
#include <algorithm>

namespace hybrid {

void MovementSystem::update(entt::registry& registry, HybridWorld& world, float dt) {
    auto view = registry.view<TransformComponent, AIComponent, VelocityComponent, AttributesComponent, TypeTagComponent, EntityLink>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& ai = view.get<AIComponent>(entity);
        auto& velocity = view.get<VelocityComponent>(entity);
        auto& attr = view.get<AttributesComponent>(entity);
        auto& tag = view.get<TypeTagComponent>(entity);
        auto& link = view.get<EntityLink>(entity);

        // Get OOP entity
        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity || oopEntity->isDead()) {
            continue;
        }

        auto entityType = static_cast<shared::EntityType>(tag.type);
        const auto& profile = shared::getBehaviorProfile(entityType);
        const auto& context = shared::simulationContext();
        shared::BehaviorState& behaviorState = oopEntity->behaviorState();
        float healthPercent = oopEntity->getHealthPercent();

        // Calculate speed
        float speed = attr.moveSpeed;
        if (tag.isAquatic && transform.y < shared::WorldConstants::WATER_LEVEL) {
            speed = attr.swimSpeed;
        }

        float speedScale = shared::computeActivityMultiplier(
            profile, behaviorState, context,
            static_cast<shared::AIState>(ai.state), healthPercent,
            tag.isFlying, tag.isAquatic);
        speed *= speedScale;

        // Movement based on AI state
        float dx = 0.0f, dz = 0.0f;

        switch (static_cast<shared::AIState>(ai.state)) {
            case shared::AIState::Wander:
                dx = ai.wanderDirX * speed * dt;
                dz = ai.wanderDirZ * speed * dt;
                break;
            case shared::AIState::Chase: {
                float chaseCoeff = (tag.isFlying || tag.isAquatic) ? 1.5f : 1.35f;
                dx = ai.wanderDirX * speed * chaseCoeff * dt;
                dz = ai.wanderDirZ * speed * chaseCoeff * dt;
                break;
            }
            case shared::AIState::Flee: {
                float fleeCoeff = (tag.isFlying || tag.isAquatic) ? 1.5f : 1.6f;
                dx = ai.wanderDirX * speed * fleeCoeff * dt;
                dz = ai.wanderDirZ * speed * fleeCoeff * dt;
                break;
            }
            case shared::AIState::Attack: {
                float attackCoeff = (tag.isFlying || tag.isAquatic) ? 0.4f : 0.35f;
                dx = ai.wanderDirX * speed * attackCoeff * dt;
                dz = ai.wanderDirZ * speed * attackCoeff * dt;
                break;
            }
            default:
                break;
        }

        // Home bias
        if (profile.homeBias > 0.0f) {
            float homeDx = behaviorState.homeX - transform.x;
            float homeDz = behaviorState.homeZ - transform.z;
            float homeDistSq = homeDx * homeDx + homeDz * homeDz;
            if (homeDistSq > 1.0f) {
                float homeDist = std::sqrt(homeDistSq);
                float homeBiasCoeff = (tag.isFlying || tag.isAquatic) ? 0.08f : 0.12f;
                float homeStep = speed * profile.homeBias * homeBiasCoeff * dt;
                dx += (homeDx / homeDist) * homeStep;
                dz += (homeDz / homeDist) * homeStep;
            }
        }

        // Apply movement
        transform.x += dx;
        transform.z += dz;

        // Height adjustments for flying/aquatic
        if (tag.isFlying) {
            float targetHeight = attr.flightHeight;
            targetHeight += profile.verticalBias * 4.0f;
            if (context.isNight) {
                targetHeight += profile.nightAffinity * 1.5f;
            }
            float heightDiff = targetHeight - transform.y;
            if (std::abs(heightDiff) > 0.1f) {
                transform.y += std::copysign(attr.moveSpeed * dt, heightDiff);
            }
            transform.y = std::clamp(transform.y, 1.0f, shared::WorldConstants::WORLD_SIZE_Y);
        } else if (tag.isAquatic) {
            transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        } else {
            transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        }

        // Calculate target distance for special behaviors
        float targetDistanceSq = 1.0e30f;
        bool hasTarget = ai.target != NullEntity && registry.valid(ai.target);
        if (hasTarget && registry.all_of<TransformComponent>(ai.target)) {
            const auto& targetTransform = registry.get<TransformComponent>(ai.target);
            float dxTarget = targetTransform.x - transform.x;
            float dyTarget = targetTransform.y - transform.y;
            float dzTarget = targetTransform.z - transform.z;
            targetDistanceSq = dxTarget * dxTarget + dyTarget * dyTarget + dzTarget * dzTarget;
        }

        // Special behavior dispatch
        if (link.hasSpecialBehavior) {
            shared::AIState currentState = static_cast<shared::AIState>(ai.state);
            shared::AIState specialState = currentState;

            // Use shared special behavior logic for entities without custom OOP behavior
            // For entities with OOP behavior, call virtual method
            auto result = oopEntity->updateSpecialBehavior(
                transform.x, transform.y, transform.z,
                ai.wanderDirX, ai.wanderDirZ,
                dt, hasTarget, targetDistanceSq, attr.attackRange * attr.attackRange);

            if (result) {
                // Entity should be deactivated (e.g., Creeper exploded)
                oopEntity->takeDamage(oopEntity->maximumHealth());
                oopEntity->setDead(true);
            }
        } else {
            // Use shared behavior for non-special entities
            shared::AIState currentState = static_cast<shared::AIState>(ai.state);
            shared::AIState specialState = currentState;

            auto result = shared::updateSpecialBehavior(
                entityType,
                shared::getEntityConfig(entityType),
                profile,
                behaviorState,
                specialState,
                transform.x, transform.y, transform.z,
                ai.wanderDirX, ai.wanderDirZ,
                dt, hasTarget, targetDistanceSq, attr.attackRange * attr.attackRange,
                context);

            if (result == shared::SpecialBehaviorResult::Deactivate) {
                oopEntity->takeDamage(oopEntity->maximumHealth());
                oopEntity->setDead(true);
            }

            ai.state = static_cast<uint8_t>(specialState);
        }

        // World bounds
        transform.x = std::clamp(transform.x, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        transform.z = std::clamp(transform.z, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    }
}

} // namespace hybrid
