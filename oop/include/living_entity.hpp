#pragma once

#include "entity.hpp"

namespace oop {

class LivingEntity : public Entity {
public:
    explicit LivingEntity(EntityID id, int maxHealth);
    ~LivingEntity() override = default;

    // 生命值
    int getHealth() const { return health_; }
    int getMaxHealth() const { return maxHealth_; }
    float getHealthPercent() const;

    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    bool isDead() const { return health_ <= 0; }

    // 生命恢复
    float getRegenRate() const { return regenRate_; }
    void setRegenRate(float rate) { regenRate_ = rate; }

    // 类型检查覆盖
    bool isLiving() const override { return true; }

protected:
    void updateRegeneration(float dt);

    int health_;
    int maxHealth_;
    float regenRate_ = 0.0f;  // HP/秒
};

} // namespace oop
