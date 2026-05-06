#pragma once

#include "../aquatic_creature.hpp"

namespace oop {

// Fish - 鱼
class Fish : public AquaticCreature {
public:
    explicit Fish(EntityID id);
    ~Fish() override = default;

    void update(float dt) override;

private:
    void updateSwimBehavior(float dt);

    float swimDirection_ = 0.0f;
};

// Squid - 鱿鱼
class Squid : public AquaticCreature {
public:
    explicit Squid(EntityID id);
    ~Squid() override = default;

    void update(float dt) override;

private:
    void updateInkDefense(float dt);

    float inkCooldown_ = 0.0f;
    static constexpr float INK_COOLDOWN = 10.0f;
};

// Dolphin - 海豚
class Dolphin : public AquaticCreature {
public:
    explicit Dolphin(EntityID id);
    ~Dolphin() override = default;

    void update(float dt) override;

private:
    void updateJumpBehavior(float dt);

    float jumpCooldown_ = 0.0f;
    bool isJumping_ = false;
    static constexpr float JUMP_COOLDOWN = 30.0f;
};

// Turtle - 海龟
class Turtle : public AquaticCreature {
public:
    explicit Turtle(EntityID id);
    ~Turtle() override = default;

    void update(float dt) override;

private:
    void updateEggLaying(float dt);

    bool isOnLand_ = false;
    float eggTimer_ = 0.0f;
};

} // namespace oop
