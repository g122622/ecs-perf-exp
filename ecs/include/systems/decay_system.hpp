#pragma once

#include "core/system_base.hpp"
#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "components/health.hpp"

namespace ecs {

class DecaySystem : public SystemBase {
public:
    DecaySystem(EntityManager& entities,
                ComponentPool<HealthComponent>& health);

    void update(float dt) override;
    const char* getName() const override { return "DecaySystem"; }

private:
    EntityManager& entities_;
    ComponentPool<HealthComponent>& health_;
};

} // namespace ecs
