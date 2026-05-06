#include "animal.hpp"
#include "shared/constants.hpp"

namespace oop {

Animal::Animal(EntityID id, int maxHealth, shared::EntityType type)
    : Creature(id, maxHealth, type) {
}

void Animal::update(float dt) {
    if (!active_) return;

    Creature::update(dt);
}

void Animal::flee() {
    setAIState(shared::AIState::Flee);
}

void Animal::updateAI(float dt) {
    (void)dt;
    const auto& profile = shared::getBehaviorProfile(entityType_);
    const auto& context = shared::simulationContext();

    shared::AIState nextState = shared::chooseAIState(
        aiState_,
        profile,
        behaviorState_,
        context,
        getHealthPercent(),
        hasTarget(),
        hasTarget(),
        false,
        true,
        false,
        false);

    setAIState(nextState);

    switch (aiState_) {
        case shared::AIState::Idle: {
            // 动物随机游荡
            if (stateTimer_ > shared::WorldConstants::IDLE_DURATION_MAX * 0.5f) {
                setAIState(shared::AIState::Wander);
            }
            break;
        }
        case shared::AIState::Wander: {
            // 被动生物主要在游荡
            if (stateTimer_ > shared::WorldConstants::WANDER_DURATION_MAX) {
                setAIState(shared::AIState::Idle);
            }
            break;
        }
        case shared::AIState::Flee: {
            // 逃跑状态
            if (stateTimer_ > shared::WorldConstants::FLEE_DURATION_MAX) {
                setAIState(shared::AIState::Wander);
            }
            break;
        }
        default:
            break;
    }

    // 低血量逃跑
    if (getHealthPercent() < fleeHealthThreshold_) {
        flee();
    }
}

} // namespace oop
