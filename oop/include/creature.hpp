#pragma once

#include "living_entity.hpp"
#include "shared/behavior_rules.hpp"
#include "shared/entity_types.hpp"

namespace oop {

class Creature : public LivingEntity {
public:
    Creature(EntityID id, int maxHealth, shared::EntityType type);
    ~Creature() override = default;

    void update(float dt) override;
    void initializeBehaviorState(float x, float y, float z);
    void setWanderDirection(float dirX, float dirZ);

    // AI状态
    shared::AIState getAIState() const { return aiState_; }
    void setAIState(shared::AIState state);

    // 感知
    float getPerceptionRange() const { return perceptionRange_; }
    void setPerceptionRange(float range) { perceptionRange_ = range; }

    // 目标
    EntityID getTargetId() const { return targetId_; }
    void setTargetId(EntityID id) { targetId_ = id; }
    bool hasTarget() const { return targetId_ != INVALID_ENTITY_ID; }
    void clearTarget() { targetId_ = INVALID_ENTITY_ID; targetDistanceSq_ = 0.0f; }

    // 目标距离
    float getTargetDistanceSq() const { return targetDistanceSq_; }
    void setTargetDistanceSq(float distSq) { targetDistanceSq_ = distSq; }

    // 移动速度
    float getMoveSpeed() const { return moveSpeed_; }
    void setMoveSpeed(float speed) { moveSpeed_ = speed; }

    // 类型检查
    bool isCreature() const override { return true; }

    // 实体类型
    shared::EntityType getEntityType() const { return entityType_; }

protected:
    virtual void updateAI(float dt) = 0;
    virtual void updateMovement(float dt);

    // AI状态转换
    virtual void onAIStateEnter(shared::AIState newState);
    virtual void onAIStateExit(shared::AIState oldState);

    shared::AIState aiState_;
    shared::EntityType entityType_;

    float perceptionRange_ = 16.0f;
    float moveSpeed_ = 0.2f;
    EntityID targetId_ = INVALID_ENTITY_ID;
    float targetDistanceSq_ = 0.0f;  // 目标距离的平方

    // AI状态计时器
    float stateTimer_ = 0.0f;
    float stateDuration_ = 0.0f;

    // 随机游荡方向
    float wanderDirX_ = 0.0f;
    float wanderDirZ_ = 0.0f;

    shared::BehaviorState behaviorState_;
};

} // namespace oop
