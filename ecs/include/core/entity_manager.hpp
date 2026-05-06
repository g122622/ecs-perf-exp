#pragma once

#include <cstdint>
#include <vector>
#include <queue>

namespace ecs {

using EntityID = uint64_t;
constexpr EntityID INVALID_ENTITY_ID = 0;

// 组件类型ID
using ComponentTypeID = size_t;

namespace detail {
    inline ComponentTypeID nextComponentTypeID() {
        static ComponentTypeID id = 0;
        return id++;
    }

    template<typename T>
    ComponentTypeID getComponentTypeID() {
        static ComponentTypeID id = nextComponentTypeID();
        return id;
    }
}

// 组件基类 (空基类，用于类型擦除)
struct IComponent {
    virtual ~IComponent() = default;
};

// 实体管理器
class EntityManager {
public:
    EntityManager(size_t initialCapacity = 10000);
    ~EntityManager() = default;

    // 实体创建/销毁
    EntityID createEntity();
    void destroyEntity(EntityID id);
    bool isEntityAlive(EntityID id) const;

    // 实体数量
    size_t getEntityCount() const { return aliveEntityCount_; }
    size_t getCapacity() const { return entityGeneration_.size(); }

    // 清空
    void clear();

private:
    std::vector<uint32_t> entityGeneration_;  // 实体代数(用于检测悬空引用)
    std::queue<EntityID> freeEntityIds_;       // 可复用的实体ID
    size_t aliveEntityCount_ = 0;
};

} // namespace ecs
