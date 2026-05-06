#pragma once

#include "entt.hpp"
#include "shared/behavior_rules.hpp"

#include <cstdint>

namespace ecs_entt {

using Entity = entt::entity;
constexpr Entity NullEntity = entt::null;

// Transform component - Position + Rotation
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// Health component
struct Health {
    int current = 0;
    int maximum = 0;
    float regenRate = 0.0f;
    bool isDead = false;
};

// AI component
struct AI {
    uint8_t state = 0;  // 0=Idle, 1=Wander, 2=Chase, 3=Flee
    Entity target = NullEntity;
    float perceptionRange = 16.0f;
    float stateTimer = 0.0f;
    float stateDuration = 0.0f;  // Auxiliary per-entity value used by some species
    float wanderDirX = 0.0f;
    float wanderDirZ = 0.0f;
};

// Attributes component
struct Attributes {
    float moveSpeed = 0.2f;
    float attackDamage = 0.0f;
    float attackRange = 0.0f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.0f;
    float flightHeight = 0.0f;
    float swimSpeed = 0.0f;
};

// Type tag component
struct TypeTag {
    uint16_t type = 0;
    bool isMonster = false;
    bool isAnimal = false;
    bool isFlying = false;
    bool isAquatic = false;
};

// 共享运行时行为状态 - 与 OOP 侧的 BehaviorState 保持一致
using BehaviorState = shared::BehaviorState;

} // namespace ecs_entt
