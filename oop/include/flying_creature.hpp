#pragma once

#include "creature.hpp"

namespace oop {

class FlyingCreature : public Creature {
public:
    FlyingCreature(EntityID id, int maxHealth, shared::EntityType type,
                   float flightHeight, float flightSpeed);
    ~FlyingCreature() override = default;

    void update(float dt) override;

    // 飞行属性
    float getFlightHeight() const { return flightHeight_; }
    float getFlightSpeed() const { return flightSpeed_; }

    // 类型检查
    bool isFlying() const override { return true; }

protected:
    void updateAI(float dt) override;  // 提供默认实现
    void updateMovement(float dt) override;

    float flightHeight_;
    float flightSpeed_;
    float targetFlightHeight_;  // 目标飞行高度
};

} // namespace oop
