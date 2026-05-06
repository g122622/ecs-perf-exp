#pragma once

#include "core/system_base.hpp"
#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "components/transform.hpp"
#include "components/physics.hpp"
#include "components/ai_state.hpp"
#include "components/attributes.hpp"
#include "shared/constants.hpp"
#include <random>

namespace ecs {

class MovementSystem : public SystemBase {
public:
    MovementSystem(EntityManager& entities,
                   ComponentPool<TransformComponent>& transforms,
                   ComponentPool<PhysicsComponent>& physics,
                   ComponentPool<AIComponent>& aiStates,
                   ComponentPool<AttributesComponent>& attributes);

    void update(float dt) override;
    const char* getName() const override { return "MovementSystem"; }

private:
    EntityManager& entities_;
    ComponentPool<TransformComponent>& transforms_;
    ComponentPool<PhysicsComponent>& physics_;
    ComponentPool<AIComponent>& aiStates_;
    ComponentPool<AttributesComponent>& attributes_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;

    void generateRandomDirection(float& dx, float& dz);
};

} // namespace ecs
