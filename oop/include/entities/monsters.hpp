#pragma once

#include "../monster.hpp"

namespace oop {

// Zombie - 僵尸
class Zombie : public Monster {
public:
    explicit Zombie(EntityID id);
    ~Zombie() override = default;

    void update(float dt) override;

private:
    void updateBehavior(float dt);
};

// Skeleton - 骷髅
class Skeleton : public Monster {
public:
    explicit Skeleton(EntityID id);
    ~Skeleton() override = default;

    void update(float dt) override;

private:
    void updateRangedAttack(float dt);
};

// Spider - 蜘蛛
class Spider : public Monster {
public:
    explicit Spider(EntityID id);
    ~Spider() override = default;

    void update(float dt) override;

private:
    bool isClimbing_ = false;
};

// Creeper - 苦力怕
class Creeper : public Monster {
public:
    explicit Creeper(EntityID id);
    ~Creeper() override = default;

    void update(float dt) override;

private:
    void updateExplosion(float dt);

    float fuseTime_ = 0.0f;
    bool isIgnited_ = false;
    static constexpr float MAX_FUSE = 1.5f;
};

// Enderman - 末影人
class Enderman : public Monster {
public:
    explicit Enderman(EntityID id);
    ~Enderman() override = default;

    void update(float dt) override;

private:
    void updateTeleport(float dt);

    float teleportCooldown_ = 0.0f;
    static constexpr float TELEPORT_COOLDOWN = 5.0f;
};

// Blaze - 烈焰人
class Blaze : public Monster {
public:
    explicit Blaze(EntityID id);
    ~Blaze() override = default;

    void update(float dt) override;

private:
    void updateFireballAttack(float dt);

    float fireballCooldown_ = 0.0f;
    static constexpr float FIREBALL_COOLDOWN = 2.0f;
};

// Ghast - 恶魂
class Ghast : public Monster {
public:
    explicit Ghast(EntityID id);
    ~Ghast() override = default;

    void update(float dt) override;

private:
    void updateFireballAttack(float dt);

    float fireballCooldown_ = 0.0f;
    static constexpr float FIREBALL_COOLDOWN = 3.0f;
};

// Phantom - 幻翼
class Phantom : public Monster {
public:
    explicit Phantom(EntityID id);
    ~Phantom() override = default;

    void update(float dt) override;

private:
    void updateDiveAttack(float dt);

    float diveCooldown_ = 0.0f;
    bool isDiving_ = false;
    static constexpr float DIVE_COOLDOWN = 5.0f;
};

} // namespace oop
