#pragma once

#include "core/entity_manager.hpp"
#include "core/component_pool.hpp"
#include "core/system_base.hpp"
#include "components/transform.hpp"
#include "components/physics.hpp"
#include "components/health.hpp"
#include "components/ai_state.hpp"
#include "components/attributes.hpp"
#include "components/type_tag.hpp"
#include "shared/entity_types.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <random>

namespace ecs {

class World {
public:
    World(size_t initialCapacity = 10000);
    ~World() = default;

    // 实体管理
    EntityID createEntity(shared::EntityType type, float x, float y, float z);
    void destroyEntity(EntityID id);
    bool isEntityAlive(EntityID id) const;

    // 组件访问
    TransformComponent* getTransform(EntityID id);
    PhysicsComponent* getPhysics(EntityID id);
    HealthComponent* getHealth(EntityID id);
    AIComponent* getAI(EntityID id);
    AttributesComponent* getAttributes(EntityID id);
    TypeTagComponent* getTypeTag(EntityID id);

    // 系统更新
    void update(float dt);

    // 查询
    size_t getEntityCount() const { return entities_.getEntityCount(); }

    // 遍历所有实体
    template<typename Func>
    void forEachEntity(Func&& func);

    // 空间查询
    void findEntitiesInRange(float x, float y, float z, float range,
                             std::vector<EntityID>& outIds) const;

    EntityID findNearestEntity(float x, float y, float z, float maxRange,
                               std::function<bool(EntityID)> filter = nullptr) const;

    // 清空
    void clear();

    // 随机数生成
    std::mt19937& getRNG() { return rng_; }
    void setSeed(unsigned int seed) { rng_.seed(seed); }

private:
    void initializeSystems();
    void createEntityComponents(EntityID id, shared::EntityType type,
                                float x, float y, float z);

    EntityManager entities_;

    // 组件池
    ComponentPool<TransformComponent> transforms_;
    ComponentPool<PhysicsComponent> physics_;
    ComponentPool<HealthComponent> health_;
    ComponentPool<AIComponent> aiStates_;
    ComponentPool<AttributesComponent> attributes_;
    ComponentPool<TypeTagComponent> typeTags_;

    // 系统
    std::vector<std::unique_ptr<SystemBase>> systems_;

    std::mt19937 rng_;
};

// 模板实现
template<typename Func>
void World::forEachEntity(Func&& func) {
    const auto& ids = transforms_.getEntityIds();
    for (EntityID id : ids) {
        func(id);
    }
}

} // namespace ecs
