#pragma once

#include "core/system_base.hpp"
#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "components/transform.hpp"
#include "components/ai_state.hpp"
#include "components/attributes.hpp"
#include "components/health.hpp"
#include "shared/constants.hpp"
#include <vector>

namespace ecs {

class InteractionSystem : public SystemBase {
public:
    InteractionSystem(EntityManager& entities,
                      ComponentPool<TransformComponent>& transforms,
                      ComponentPool<AIComponent>& aiStates,
                      ComponentPool<AttributesComponent>& attributes,
                      ComponentPool<HealthComponent>& health);

    void update(float dt) override;
    const char* getName() const override { return "InteractionSystem"; }

private:
    EntityManager& entities_;
    ComponentPool<TransformComponent>& transforms_;
    ComponentPool<AIComponent>& aiStates_;
    ComponentPool<AttributesComponent>& attributes_;
    ComponentPool<HealthComponent>& health_;

    // 临时缓冲区用于空间查询
    std::vector<EntityID> nearbyEntities_;

    void findEntitiesInRange(float x, float y, float z, float range,
                             std::vector<EntityID>& outIds);
    float distanceSq(float x1, float y1, float z1, float x2, float y2, float z2);
};

} // namespace ecs
