#pragma once

#include "entt.hpp"
#include <cstdint>

namespace ecs_entt {

using Entity = entt::entity;
constexpr Entity NullEntity = entt::null;

// Transform component - Position + Rotation
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
};

// Physics component - Velocity + Acceleration
struct Physics {
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
    float drag = 0.98f;
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
    float stateDuration = 0.0f;  // Duration for current state
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
    uint16_t entityType = 0;
    uint8_t behaviorFlags = 0;
};

// Type tag component
struct TypeTag {
    uint16_t type = 0;
    bool isMonster = false;
    bool isAnimal = false;
    bool isFlying = false;
    bool isAquatic = false;
};

} // namespace ecs_entt
