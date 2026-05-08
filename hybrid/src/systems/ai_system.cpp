#include "world.hpp"
#include <cmath>

namespace hybrid {

void AISystem::update(entt::registry& registry, HybridWorld& world, float dt) {
    auto view = registry.view<AIComponent, AttributesComponent, TypeTagComponent, EntityLink>();

    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        auto& attr = view.get<AttributesComponent>(entity);
        auto& tag = view.get<TypeTagComponent>(entity);
        auto& link = view.get<EntityLink>(entity);

        // Get OOP entity for health and behavior state
        auto* oopEntity = world.getOopEntity(link.oopIndex);
        if (!oopEntity || oopEntity->isDead()) {
            continue;
        }

        auto entityType = static_cast<shared::EntityType>(tag.type);
        const auto& profile = shared::getBehaviorProfile(entityType);
        const auto& context = shared::simulationContext();

        shared::BehaviorState& behaviorState = oopEntity->behaviorState();
        shared::AIState currentState = static_cast<shared::AIState>(ai.state);
        float healthPercent = oopEntity->getHealthPercent();

        // Update behavior state
        shared::updateBehaviorState(
            behaviorState,
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

        // Choose next AI state
        bool hasTarget = ai.target != NullEntity && registry.valid(ai.target);
        shared::AIState nextState = shared::chooseAIState(
            currentState,
            profile,
            behaviorState,
            context,
            healthPercent,
            hasTarget,
            hasTarget,
            tag.isMonster,
            tag.isAnimal,
            tag.isFlying,
            tag.isAquatic);

        // Clear target if losing chase
        if (!hasTarget && currentState == shared::AIState::Chase && nextState != shared::AIState::Chase) {
            ai.target = NullEntity;
        }

        // Reset roam timer on state change
        if (currentState != nextState) {
            behaviorState.roamTimer = 0.0f;
        }

        ai.state = static_cast<uint8_t>(nextState);
        ai.stateTimer += dt;

        // Initialize wander direction if needed
        if (currentState == shared::AIState::Idle && nextState == shared::AIState::Wander) {
            if (std::abs(ai.wanderDirX) < 0.001f && std::abs(ai.wanderDirZ) < 0.001f) {
                shared::setHeadingFromStableAngle(entityType, behaviorState, context.elapsedTime, ai.wanderDirX, ai.wanderDirZ);
            }
        }
    }
}

} // namespace hybrid
