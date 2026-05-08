#pragma once

#include "../../third_party/entt.hpp"
#include "shared/behavior_rules.hpp"
#include <cstdint>

namespace hybrid {

using Entity = entt::entity;
constexpr Entity NullEntity = entt::null;

// High-frequency ECS components - accessed every frame
// These are stored contiguously for cache-friendly iteration

// Transform - position data (updated every frame by movement system)
struct TransformComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// AI state - updated every frame by AI system
struct AIComponent {
    uint8_t state = 0;  // AIState enum
    Entity target = NullEntity;
    float perceptionRange = 16.0f;
    float stateTimer = 0.0f;
    float stateDuration = 0.0f;
    float wanderDirX = 0.0f;
    float wanderDirZ = 0.0f;
};

// Movement velocity - computed by AI, consumed by movement
struct VelocityComponent {
    float dx = 0.0f;
    float dz = 0.0f;
    float speed = 0.2f;
};

// Entity type flags - static, used for branching in systems
struct TypeTagComponent {
    uint16_t type = 0;  // EntityType enum
    bool isMonster = false;
    bool isAnimal = false;
    bool isFlying = false;
    bool isAquatic = false;
};

// Attributes - semi-static, read frequently but rarely written
struct AttributesComponent {
    float moveSpeed = 0.2f;
    float attackDamage = 0.0f;
    float attackRange = 0.0f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.0f;
    float flightHeight = 0.0f;
    float swimSpeed = 0.0f;
};

// Link to OOP entity - used to access low-frequency data
struct EntityLink {
    uint32_t oopIndex = 0;  // Index into HybridWorld's entity vector
    bool hasSpecialBehavior = false;  // Quick check for virtual dispatch
};

} // namespace hybrid
