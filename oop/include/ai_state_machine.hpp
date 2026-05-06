#pragma once

#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include <random>

namespace oop {

class AIStateMachine {
public:
    AIStateMachine();

    // 状态转换
    void transitionTo(shared::AIState newState);
    void forceState(shared::AIState state) { currentState_ = state; }

    // 更新
    void update(float dt);

    // 状态查询
    shared::AIState getCurrentState() const { return currentState_; }
    float getStateTimer() const { return stateTimer_; }
    bool isStateExpired() const;

    // 状态持续时间
    void setStateDuration(float duration);
    float getStateDuration() const { return stateDuration_; }

    // 随机行为
    float getRandomIdleDuration();
    float getRandomWanderDuration();
    float getRandomFleeDuration();

    // 生成随机方向
    void generateWanderDirection(float& dx, float& dz);

private:
    shared::AIState currentState_;
    float stateTimer_;
    float stateDuration_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;
};

} // namespace oop
