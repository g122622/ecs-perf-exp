#pragma once

#include "entity_manager.hpp"
#include <vector>
#include <memory>
#include <cstring>
#include <cassert>

namespace ecs {

// 组件池 - 使用内存池存储组件
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void remove(EntityID id) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
};

// 类型化组件池
template<typename T>
class ComponentPool : public IComponentPool {
public:
    ComponentPool(size_t initialCapacity = 1000);
    ~ComponentPool() override = default;

    // 添加/获取组件
    template<typename... Args>
    T& add(EntityID id, Args&&... args);

    T* get(EntityID id);
    const T* get(EntityID id) const;

    bool has(EntityID id) const;

    // 删除组件
    void remove(EntityID id) override;

    // 遍历所有组件
    template<typename Func>
    void forEach(Func&& func);

    template<typename Func>
    void forEach(Func&& func) const;

    // 清空
    void clear() override;

    size_t size() const override { return componentCount_; }

    // 获取所有有效实体ID
    const std::vector<EntityID>& getEntityIds() const { return entityIds_; }

private:
    std::vector<T> components_;           // 组件数据
    std::vector<EntityID> entityIds_;     // 对应的实体ID
    std::vector<size_t> entityToIndex_;   // 实体ID -> 组件索引
    size_t componentCount_ = 0;
    size_t capacity_ = 0;
};

// ==================== 实现 ====================

template<typename T>
ComponentPool<T>::ComponentPool(size_t initialCapacity)
    : components_(initialCapacity)
    , entityIds_(initialCapacity)
    , entityToIndex_(initialCapacity * 4, static_cast<size_t>(-1))  // 稀疏数组
    , capacity_(initialCapacity) {
}

template<typename T>
template<typename... Args>
T& ComponentPool<T>::add(EntityID id, Args&&... args) {
    // 扩展索引数组
    if (id >= entityToIndex_.size()) {
        entityToIndex_.resize(id * 2 + 1, static_cast<size_t>(-1));
    }

    // 如果已存在，返回现有组件
    size_t index = entityToIndex_[id];
    if (index != static_cast<size_t>(-1)) {
        return components_[index];
    }

    // 需要扩容
    if (componentCount_ >= capacity_) {
        capacity_ *= 2;
        components_.resize(capacity_);
        entityIds_.resize(capacity_);
    }

    // 添加新组件
    index = componentCount_++;
    new (&components_[index]) T(std::forward<Args>(args)...);
    entityIds_[index] = id;
    entityToIndex_[id] = index;

    return components_[index];
}

template<typename T>
T* ComponentPool<T>::get(EntityID id) {
    if (id >= entityToIndex_.size()) {
        return nullptr;
    }
    size_t index = entityToIndex_[id];
    if (index == static_cast<size_t>(-1)) {
        return nullptr;
    }
    return &components_[index];
}

template<typename T>
const T* ComponentPool<T>::get(EntityID id) const {
    if (id >= entityToIndex_.size()) {
        return nullptr;
    }
    size_t index = entityToIndex_[id];
    if (index == static_cast<size_t>(-1)) {
        return nullptr;
    }
    return &components_[index];
}

template<typename T>
bool ComponentPool<T>::has(EntityID id) const {
    if (id >= entityToIndex_.size()) {
        return false;
    }
    return entityToIndex_[id] != static_cast<size_t>(-1);
}

template<typename T>
void ComponentPool<T>::remove(EntityID id) {
    if (id >= entityToIndex_.size()) {
        return;
    }

    size_t index = entityToIndex_[id];
    if (index == static_cast<size_t>(-1)) {
        return;  // 组件不存在
    }

    // 与最后一个组件交换，保持紧凑
    size_t lastIndex = componentCount_ - 1;
    if (index != lastIndex) {
        components_[index] = std::move(components_[lastIndex]);
        entityIds_[index] = entityIds_[lastIndex];
        entityToIndex_[entityIds_[index]] = index;
    }

    entityToIndex_[id] = static_cast<size_t>(-1);
    componentCount_--;
}

template<typename T>
template<typename Func>
void ComponentPool<T>::forEach(Func&& func) {
    for (size_t i = 0; i < componentCount_; ++i) {
        func(entityIds_[i], components_[i]);
    }
}

template<typename T>
template<typename Func>
void ComponentPool<T>::forEach(Func&& func) const {
    for (size_t i = 0; i < componentCount_; ++i) {
        func(entityIds_[i], components_[i]);
    }
}

template<typename T>
void ComponentPool<T>::clear() {
    componentCount_ = 0;
    std::fill(entityToIndex_.begin(), entityToIndex_.end(), static_cast<size_t>(-1));
}

} // namespace ecs
