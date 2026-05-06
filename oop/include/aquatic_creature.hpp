#pragma once

#include "creature.hpp"

namespace oop {

class AquaticCreature : public Creature {
public:
    AquaticCreature(EntityID id, int maxHealth, shared::EntityType type,
                    float swimSpeed, bool canBreathe);
    ~AquaticCreature() override = default;

    void update(float dt) override;

    // 游泳属性
    float getSwimSpeed() const { return swimSpeed_; }
    bool canBreathe() const { return canBreathe_; }

    // 类型检查
    bool isAquatic() const override { return true; }

protected:
    void updateAI(float dt) override;  // 提供默认实现
    void updateMovement(float dt) override;

    float swimSpeed_;
    bool canBreathe_;  // 是否能呼吸(离开水)
};

} // namespace oop
