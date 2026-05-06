#pragma once

#include "shared/entity_types.hpp"

namespace ecs {

// AI状态组件
struct AIComponent {
    shared::AIState state = shared::AIState::Idle;
    EntityID targetId = INVALID_ENTITY_ID;
    float perceptionRange = 16.0f;
    float stateTimer = 0.0f;
    float stateDuration = 0.0f;

    // 随机游荡方向
    float wanderDirX = 0.0f;
    float wanderDirZ = 0.0f;

    AIComponent() = default;
    AIComponent(shared::AIState s, float perception)
        : state(s), perceptionRange(perception) {}
};

} // namespace ecs
