#pragma once

#include <cstdint>

namespace oop {

using EntityID = uint64_t;
constexpr EntityID INVALID_ENTITY_ID = 0;

class Entity {
public:
    explicit Entity(EntityID id);
    virtual ~Entity() = default;

    // 禁止拷贝
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // 允许移动
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    // 核心更新函数 - 纯虚函数
    virtual void update(float dt) = 0;

    // 位置访问
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getZ() const { return z_; }
    void setPosition(float x, float y, float z);

    // 标识
    EntityID getId() const { return id_; }
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    // 类型检查 - 虚函数用于RTTI替代
    virtual bool isLiving() const { return false; }
    virtual bool isCreature() const { return false; }
    virtual bool isMonster() const { return false; }
    virtual bool isAnimal() const { return false; }
    virtual bool isFlying() const { return false; }
    virtual bool isAquatic() const { return false; }

protected:
    EntityID id_;
    float x_ = 0.0f;
    float y_ = 0.0f;
    float z_ = 0.0f;
    bool active_ = true;
};

} // namespace oop
