#pragma once

#include "core/system_base.hpp"
#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "components/transform.hpp"
#include "components/physics.hpp"
#include "shared/constants.hpp"

namespace ecs {

class PhysicsSystem : public SystemBase {
public:
    PhysicsSystem(EntityManager& entities,
                  ComponentPool<TransformComponent>& transforms,
                  ComponentPool<PhysicsComponent>& physics);

    void update(float dt) override;
    const char* getName() const override { return "PhysicsSystem"; }

private:
    EntityManager& entities_;
    ComponentPool<TransformComponent>& transforms_;
    ComponentPool<PhysicsComponent>& physics_;
};

} // namespace ecs
