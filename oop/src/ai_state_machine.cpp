#include "ai_state_machine.hpp"
#include "shared/constants.hpp"
#include <cmath>

namespace oop {

AIStateMachine::AIStateMachine()
    : currentState_(shared::AIState::Idle)
    , stateTimer_(0.0f)
    , stateDuration_(0.0f)
    , rng_(shared::RandomConstants::DEFAULT_SEED)
    , dist_(0.0f, 1.0f) {
}

void AIStateMachine::transitionTo(shared::AIState newState) {
    if (currentState_ != newState) {
        currentState_ = newState;
        stateTimer_ = 0.0f;
        stateDuration_ = 0.0f;  // 新状态需要设置持续时间
    }
}

void AIStateMachine::update(float dt) {
    stateTimer_ += dt;
}

bool AIStateMachine::isStateExpired() const {
    if (stateDuration_ <= 0.0f) {
        return false;  // 没有设置持续时间限制
    }
    return stateTimer_ >= stateDuration_;
}

void AIStateMachine::setStateDuration(float duration) {
    stateDuration_ = duration;
}

float AIStateMachine::getRandomIdleDuration() {
    std::uniform_real_distribution<float> dist(
        shared::WorldConstants::IDLE_DURATION_MIN,
        shared::WorldConstants::IDLE_DURATION_MAX);
    return dist(rng_);
}

float AIStateMachine::getRandomWanderDuration() {
    std::uniform_real_distribution<float> dist(
        shared::WorldConstants::WANDER_DURATION_MIN,
        shared::WorldConstants::WANDER_DURATION_MAX);
    return dist(rng_);
}

float AIStateMachine::getRandomFleeDuration() {
    std::uniform_real_distribution<float> dist(
        shared::WorldConstants::FLEE_DURATION_MIN,
        shared::WorldConstants::FLEE_DURATION_MAX);
    return dist(rng_);
}

void AIStateMachine::generateWanderDirection(float& dx, float& dz) {
    // 生成随机方向
    float angle = dist_(rng_) * 2.0f * 3.14159265f;
    dx = std::cos(angle);
    dz = std::sin(angle);
}

} // namespace oop
