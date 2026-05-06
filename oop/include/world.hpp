#pragma once

#include "entity.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <random>

namespace oop {

class World {
public:
    World();
    ~World() = default;

    // 实体管理
    EntityID spawnEntity(shared::EntityType type, float x, float y, float z);
    void destroyEntity(EntityID id);
    Entity* getEntity(EntityID id);
    const Entity* getEntity(EntityID id) const;

    // 更新所有实体
    void update(float dt);

    // 查询
    size_t getEntityCount() const { return entities_.size(); }
    void forEachEntity(const std::function<void(Entity*)>& func);
    void forEachEntity(const std::function<void(const Entity*)>& func) const;

    // 空间查询
    void findEntitiesInRange(float x, float y, float z, float range,
                            std::vector<EntityID>& outIds) const;

    // 查找最近实体
    EntityID findNearestEntity(float x, float y, float z, float maxRange,
                               std::function<bool(const Entity*)> filter = nullptr) const;

    // 清空世界
    void clear();

    // 随机数生成
    std::mt19937& getRNG() { return rng_; }

    // 世界边界
    bool isInsideWorld(float x, float y, float z) const;

private:
    // 交互系统：怪物寻找和攻击动物
    void updateInteractions(float dt);

    std::vector<std::unique_ptr<Entity>> entities_;
    EntityID nextId_ = 1;
    std::mt19937 rng_;
};

} // namespace oop
