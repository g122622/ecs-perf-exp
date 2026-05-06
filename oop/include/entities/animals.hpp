#pragma once

#include "../animal.hpp"

namespace oop {

// Pig - 猪
class Pig : public Animal {
public:
    explicit Pig(EntityID id);
    ~Pig() override = default;

    void update(float dt) override;
};

// Cow - 牛
class Cow : public Animal {
public:
    explicit Cow(EntityID id);
    ~Cow() override = default;

    void update(float dt) override;
};

// Sheep - 羊
class Sheep : public Animal {
public:
    explicit Sheep(EntityID id);
    ~Sheep() override = default;

    void update(float dt) override;

private:
    bool isSheared_ = false;
};

// Chicken - 鸡
class Chicken : public Animal {
public:
    explicit Chicken(EntityID id);
    ~Chicken() override = default;

    void update(float dt) override;

private:
    float eggTimer_ = 0.0f;
    static constexpr float EGG_INTERVAL = 300.0f;  // 5分钟
};

// Rabbit - 兔子
class Rabbit : public Animal {
public:
    explicit Rabbit(EntityID id);
    ~Rabbit() override = default;

    void update(float dt) override;

private:
    void updateFleeBehavior(float dt);
};

// Wolf - 狼
class Wolf : public Animal {
public:
    explicit Wolf(EntityID id);
    ~Wolf() override = default;

    void update(float dt) override;

    bool isTamed() const { return isTamed_; }
    void setTamed(bool tamed) { isTamed_ = tamed; }

private:
    void updatePackBehavior(float dt);

    bool isTamed_ = false;
    bool isAngry_ = false;
};

// Cat - 猫
class Cat : public Animal {
public:
    explicit Cat(EntityID id);
    ~Cat() override = default;

    void update(float dt) override;

private:
    void updateAvoidPlayer(float dt);
};

// Fox - 狐狸
class Fox : public Animal {
public:
    explicit Fox(EntityID id);
    ~Fox() override = default;

    void update(float dt) override;

private:
    void updateNocturnalBehavior(float dt);

    bool isSleeping_ = false;
    bool isHoldingItem_ = false;
};

} // namespace oop
