#include "core/entity_manager.hpp"

namespace ecs {

EntityManager::EntityManager(size_t initialCapacity) {
    entityGeneration_.resize(initialCapacity, 0);
}

EntityID EntityManager::createEntity() {
    EntityID id;

    if (!freeEntityIds_.empty()) {
        id = freeEntityIds_.front();
        freeEntityIds_.pop();
    } else {
        id = static_cast<EntityID>(entityGeneration_.size());
        entityGeneration_.push_back(0);
    }

    aliveEntityCount_++;
    return id;
}

void EntityManager::destroyEntity(EntityID id) {
    if (id >= entityGeneration_.size()) {
        return;
    }

    // 增加代数，使旧引用失效
    entityGeneration_[id]++;
    freeEntityIds_.push(id);
    aliveEntityCount_--;
}

bool EntityManager::isEntityAlive(EntityID id) const {
    if (id >= entityGeneration_.size()) {
        return false;
    }
    // 简化版本：只检查ID是否在有效范围内
    // 在完整实现中，还需要检查代数
    return true;
}

void EntityManager::clear() {
    entityGeneration_.clear();
    while (!freeEntityIds_.empty()) {
        freeEntityIds_.pop();
    }
    aliveEntityCount_ = 0;
}

} // namespace ecs
