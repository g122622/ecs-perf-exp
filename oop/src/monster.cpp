#include "monster.hpp"
#include "living_entity.hpp"
#include "shared/constants.hpp"

#include <algorithm>
#include <cmath>

namespace oop {

Monster::Monster(EntityID id, int maxHealth, shared::EntityType type,
                 int attackDamage, float attackRange)
    : Creature(id, maxHealth, type)
    , attackDamage_(attackDamage)
    , attackRange_(attackRange)
    , attackCooldown_(0.0f) {

    // 从配置读取
    const auto& config = shared::getEntityConfig(type);
    attackDamage_ = config.attackDamage;
    attackRange_ = config.attackRange;
    attackCooldownTime_ = config.attackCooldown;
}

void Monster::update(float dt) {
    if (!active_) return;

    // 更新攻击冷却
    updateAttackCooldown(dt);

    // 更新基础Creature逻辑
    Creature::update(dt);
}

void Monster::attack(LivingEntity* target, float distanceSq) {
    if (target && canAttack()) {
        const auto& profile = shared::getBehaviorProfile(entityType_);
        const auto& context = shared::simulationContext();
        float attackRangeSq = attackRange_ * attackRange_;
        float damageScale = shared::computeAttackDamageScale(profile, behaviorState_, context, distanceSq, attackRangeSq);
        int damage = std::max(1, static_cast<int>(std::round(static_cast<float>(attackDamage_) * damageScale)));
        target->takeDamage(damage);
        attackCooldown_ = attackCooldownTime_;
    }
}

void Monster::updateAI(float dt) {
    (void)dt;
    const auto& profile = shared::getBehaviorProfile(entityType_);
    const auto& context = shared::simulationContext();

    shared::AIState nextState = shared::chooseAIState(
        aiState_,
        profile,
        behaviorState_,
        context,
        getHealthPercent(),
        hasTarget(),
        hasTarget(),
        true,
        false,
        false,
        false);

    if (!hasTarget() && aiState_ == shared::AIState::Chase && nextState != shared::AIState::Chase) {
        clearTarget();
    }

    setAIState(nextState);
}

void Monster::updateAttackCooldown(float dt) {
    if (attackCooldown_ > 0.0f) {
        attackCooldown_ -= dt;
        if (attackCooldown_ < 0.0f) {
            attackCooldown_ = 0.0f;
        }
    }
}

} // namespace oop
