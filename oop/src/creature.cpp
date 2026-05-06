#include "creature.hpp"
#include "shared/constants.hpp"
#include <algorithm>
#include <cmath>

namespace oop {

Creature::Creature(EntityID id, int maxHealth, shared::EntityType type)
    : LivingEntity(id, maxHealth)
    , aiState_(shared::AIState::Idle)
    , entityType_(type)
    , perceptionRange_(16.0f)
    , moveSpeed_(0.2f)
    , targetId_(INVALID_ENTITY_ID)
    , stateTimer_(0.0f)
    , stateDuration_(0.0f) {

    // 从配置读取默认值
    const auto& config = shared::getEntityConfig(type);
    perceptionRange_ = config.perceptionRange;
    moveSpeed_ = config.moveSpeed;
    regenRate_ = config.regenRate;
    aiState_ = config.defaultState;
    shared::initializeBehaviorState(behaviorState_, 0.0f, 0.0f, 0.0f);
}

void Creature::update(float dt) {
    if (!active_) return;

    // 更新AI
    updateAI(dt);

    const auto& context = shared::simulationContext();
    const auto& profile = shared::getBehaviorProfile(entityType_);
    shared::updateBehaviorState(
        behaviorState_,
        profile,
        aiState_,
        dt,
        aiState_ == shared::AIState::Wander ||
            aiState_ == shared::AIState::Chase ||
            aiState_ == shared::AIState::Flee ||
            aiState_ == shared::AIState::Attack,
        getHealthPercent(),
        context);

    // 更新移动
    updateMovement(dt);

    // 更新生命恢复
    updateRegeneration(dt);

    // 更新状态计时器
    stateTimer_ += dt;
}

void Creature::setAIState(shared::AIState state) {
    if (aiState_ != state) {
        onAIStateExit(aiState_);
        aiState_ = state;
        stateTimer_ = 0.0f;
        behaviorState_.roamTimer = 0.0f;
        onAIStateEnter(state);
    }
}

void Creature::initializeBehaviorState(float x, float y, float z) {
    shared::initializeBehaviorState(behaviorState_, x, y, z);
    shared::setHeadingFromStableAngle(entityType_, behaviorState_, shared::simulationContext().elapsedTime, wanderDirX_, wanderDirZ_);
}

void Creature::setWanderDirection(float dirX, float dirZ) {
    shared::setHeadingFromVector(dirX, dirZ, wanderDirX_, wanderDirZ_);
}

void Creature::updateMovement(float dt) {
    // 基础移动逻辑
    float dx = 0.0f, dz = 0.0f;
    const auto& context = shared::simulationContext();
    const auto& profile = shared::getBehaviorProfile(entityType_);
    float activityMultiplier = shared::computeActivityMultiplier(profile, behaviorState_, context, aiState_, getHealthPercent(), false, false);
    float speed = moveSpeed_ * activityMultiplier;

    switch (aiState_) {
        case shared::AIState::Wander: {
            // 随机游荡
            dx = wanderDirX_ * speed * dt;
            dz = wanderDirZ_ * speed * dt;
            break;
        }
        case shared::AIState::Chase: {
            dx = wanderDirX_ * speed * 1.35f * dt;
            dz = wanderDirZ_ * speed * 1.35f * dt;
            break;
        }
        case shared::AIState::Flee: {
            // 逃跑 (方向在子类中设置)
            dx = wanderDirX_ * speed * 1.6f * dt;
            dz = wanderDirZ_ * speed * 1.6f * dt;
            break;
        }
        case shared::AIState::Attack: {
            dx = wanderDirX_ * speed * 0.35f * dt;
            dz = wanderDirZ_ * speed * 0.35f * dt;
            break;
        }
        default:
            break;
    }

    if (profile.homeBias > 0.0f) {
        float homeDx = behaviorState_.homeX - x_;
        float homeDz = behaviorState_.homeZ - z_;
        float homeDistSq = homeDx * homeDx + homeDz * homeDz;
        if (homeDistSq > 1.0f) {
            float homeDist = std::sqrt(homeDistSq);
            float homeStep = speed * profile.homeBias * 0.12f * dt;
            dx += (homeDx / homeDist) * homeStep;
            dz += (homeDz / homeDist) * homeStep;
        }
    }

    // 应用移动
    x_ += dx;
    z_ += dz;

    // 世界边界检查
    x_ = std::clamp(x_, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
    z_ = std::clamp(z_, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
}

void Creature::onAIStateEnter(shared::AIState newState) {
    // 子类可以覆盖
    (void)newState;
}

void Creature::onAIStateExit(shared::AIState oldState) {
    // 子类可以覆盖
    (void)oldState;
}

} // namespace oop
