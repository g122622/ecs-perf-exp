#pragma once

#include "../flying_creature.hpp"

namespace oop {

// Bat - 蝙蝠
class Bat : public FlyingCreature {
public:
    explicit Bat(EntityID id);
    ~Bat() override = default;

    void update(float dt) override;

private:
    void updateRandomFlight(float dt);

    float targetX_ = 0.0f;
    float targetZ_ = 0.0f;
};

// Bee - 蜜蜂
class Bee : public FlyingCreature {
public:
    explicit Bee(EntityID id);
    ~Bee() override = default;

    void update(float dt) override;

    bool isAngry() const { return isAngry_; }
    void setAngry(bool angry) { isAngry_ = angry; }

private:
    void updatePollination(float dt);

    bool isAngry_ = false;
    bool hasPollen_ = false;
    float stingCooldown_ = 0.0f;
};

} // namespace oop
