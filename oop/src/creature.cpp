#include "creature.hpp"
#include "shared/constants.hpp"
#include <cmath>
#include <algorithm>

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
}

void Creature::update(float dt) {
    if (!active_) return;

    // 更新AI
    updateAI(dt);

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
        onAIStateEnter(state);
    }
}

void Creature::updateMovement(float dt) {
    // 基础移动逻辑
    float dx = 0.0f, dz = 0.0f;

    switch (aiState_) {
        case shared::AIState::Wander: {
            // 随机游荡
            dx = wanderDirX_ * moveSpeed_ * dt;
            dz = wanderDirZ_ * moveSpeed_ * dt;
            break;
        }
        case shared::AIState::Chase: {
            // 追踪目标 (目标位置需要在子类中设置)
            // 这里只是占位，实际逻辑在子类中实现
            break;
        }
        case shared::AIState::Flee: {
            // 逃跑 (方向在子类中设置)
            dx = wanderDirX_ * moveSpeed_ * 1.5f * dt;
            dz = wanderDirZ_ * moveSpeed_ * 1.5f * dt;
            break;
        }
        default:
            break;
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
}

void Creature::onAIStateExit(shared::AIState oldState) {
    // 子类可以覆盖
}

} // namespace oop
