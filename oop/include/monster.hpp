#pragma once

#include "creature.hpp"

namespace oop {

class Monster : public Creature {
public:
    Monster(EntityID id, int maxHealth, shared::EntityType type,
            int attackDamage, float attackRange);
    ~Monster() override = default;

    void update(float dt) override;

    // 攻击
    int getAttackDamage() const { return attackDamage_; }
    float getAttackRange() const { return attackRange_; }

    // 攻击冷却
    bool canAttack() const { return attackCooldown_ <= 0.0f; }
    void attack(LivingEntity* target, float distanceSq);

    // 类型检查
    bool isMonster() const override { return true; }

protected:
    void updateAI(float dt) override;
    void updateAttackCooldown(float dt);

    int attackDamage_;
    float attackRange_;
    float attackCooldown_ = 0.0f;
    float attackCooldownTime_ = 1.0f;  // 默认1秒冷却
};

} // namespace oop
