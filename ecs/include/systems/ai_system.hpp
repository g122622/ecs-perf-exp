#pragma once

#include "core/system_base.hpp"
#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "components/transform.hpp"
#include "components/ai_state.hpp"
#include "components/attributes.hpp"
#include "components/health.hpp"
#include "shared/constants.hpp"
#include <random>

namespace ecs {

class AISystem : public SystemBase {
public:
    AISystem(EntityManager& entities,
             ComponentPool<TransformComponent>& transforms,
             ComponentPool<AIComponent>& aiStates,
             ComponentPool<AttributesComponent>& attributes,
             ComponentPool<HealthComponent>& health);

    void update(float dt) override;
    const char* getName() const override { return "AISystem"; }

private:
    EntityManager& entities_;
    ComponentPool<TransformComponent>& transforms_;
    ComponentPool<AIComponent>& aiStates_;
    ComponentPool<AttributesComponent>& attributes_;
    ComponentPool<HealthComponent>& health_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;

    void updateIdle(AIComponent& ai, AttributesComponent& attr, float dt);
    void updateWander(AIComponent& ai, AttributesComponent& attr, float dt);
    void updateChase(AIComponent& ai, AttributesComponent& attr, float dt);
    void updateFlee(AIComponent& ai, AttributesComponent& attr, float dt);
};

} // namespace ecs
