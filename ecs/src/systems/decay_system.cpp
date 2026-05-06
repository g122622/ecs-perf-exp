#include "systems/decay_system.hpp"

namespace ecs {

DecaySystem::DecaySystem(EntityManager& entities,
                          ComponentPool<HealthComponent>& health)
    : entities_(entities)
    , health_(health) {
}

void DecaySystem::update(float dt) {
    health_.forEach([&](EntityID id, HealthComponent& hp) {
        // 生命恢复
        if (hp.regenRate > 0.0f && hp.current < hp.maximum) {
            float healAmount = hp.regenRate * dt;
            hp.current = static_cast<int>(hp.current + healAmount);
            if (hp.current > hp.maximum) {
                hp.current = hp.maximum;
            }
        }
    });
}

} // namespace ecs
