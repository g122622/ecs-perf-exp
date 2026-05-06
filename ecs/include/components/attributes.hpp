#pragma once

#include "shared/entity_types.hpp"

namespace ecs {

// 属性组件 - 实体属性
struct AttributesComponent {
    float moveSpeed = 0.2f;
    float attackDamage = 0.0f;
    float attackRange = 0.0f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.0f;

    // 特殊属性
    float flightHeight = 0.0f;     // 飞行高度
    float swimSpeed = 0.0f;        // 游泳速度

    shared::EntityType entityType = shared::EntityType::Pig;
    shared::BehaviorFlags behavior = shared::BehaviorFlags::None;

    AttributesComponent() = default;

    bool isHostile() const {
        return shared::hasFlag(behavior, shared::BehaviorFlags::Hostile);
    }
    bool isPassive() const {
        return shared::hasFlag(behavior, shared::BehaviorFlags::Passive);
    }
    bool isFlying() const {
        return shared::hasFlag(behavior, shared::BehaviorFlags::Flying);
    }
    bool isAquatic() const {
        return shared::hasFlag(behavior, shared::BehaviorFlags::Aquatic);
    }
    bool canAttack() const {
        return attackCooldownTimer <= 0.0f;
    }
};

} // namespace ecs
