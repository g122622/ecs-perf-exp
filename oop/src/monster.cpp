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

    switch (aiState_) {
        case shared::AIState::Idle: {
            // 随机切换到游荡
            if (stateTimer_ > shared::WorldConstants::IDLE_DURATION_MAX) {
                setAIState(shared::AIState::Wander);
            }
            break;
        }
        case shared::AIState::Wander: {
            // 检测目标
            if (hasTarget()) {
                setAIState(shared::AIState::Chase);
            } else if (stateTimer_ > shared::WorldConstants::WANDER_DURATION_MAX) {
                setAIState(shared::AIState::Idle);
            }
            break;
        }
        case shared::AIState::Chase: {
            // 追踪目标
            if (!hasTarget()) {
                setAIState(shared::AIState::Wander);
            } else if (stateTimer_ > shared::WorldConstants::CHASE_DURATION_MAX) {
                // 追踪超时，放弃
                clearTarget();
                setAIState(shared::AIState::Wander);
            }
            break;
        }
        case shared::AIState::Flee: {
            if (stateTimer_ > shared::WorldConstants::FLEE_DURATION_MAX) {
                setAIState(shared::AIState::Wander);
            }
            break;
        }
        case shared::AIState::Attack: {
            // 攻击状态处理在子类中
            break;
        }
        default:
            break;
    }
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
