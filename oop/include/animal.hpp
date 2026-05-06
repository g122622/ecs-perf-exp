#pragma once

#include "creature.hpp"

namespace oop {

class Animal : public Creature {
public:
    Animal(EntityID id, int maxHealth, shared::EntityType type);
    ~Animal() override = default;

    void update(float dt) override;

    // 逃跑行为
    float getFleeHealthThreshold() const { return fleeHealthThreshold_; }
    void setFleeHealthThreshold(float threshold) { fleeHealthThreshold_ = threshold; }

    void flee();

    // 类型检查
    bool isAnimal() const override { return true; }

protected:
    void updateAI(float dt) override;

    float fleeHealthThreshold_ = 0.3f;  // 30%血量开始逃跑
};

} // namespace oop
