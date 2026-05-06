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
}

} // namespace oop
