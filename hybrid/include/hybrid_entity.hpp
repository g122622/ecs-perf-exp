#pragma once

#include "shared/behavior_rules.hpp"
#include "shared/entity_types.hpp"
#include <cstdint>
#include <memory>

namespace hybrid {

// Forward declaration
class HybridWorld;

// Base class for hybrid entities
// Holds low-frequency data (Health, BehaviorState) and provides virtual methods
// for type-specific special behaviors (Creeper explosion, Enderman teleport, etc.)
class HybridEntity {
public:
    explicit HybridEntity(uint32_t id, shared::EntityType type);
    virtual ~HybridEntity() = default;

    // Non-copyable, movable
    HybridEntity(const HybridEntity&) = delete;
    HybridEntity& operator=(const HybridEntity&) = delete;
    HybridEntity(HybridEntity&&) noexcept = default;
    HybridEntity& operator=(HybridEntity&&) noexcept = default;

    // Identity
    uint32_t getId() const { return id_; }
    shared::EntityType getEntityType() const { return type_; }

    // Health (low-frequency access - only during combat/healing)
    int getCurrentHealth() const { return health_.current; }
    int getMaxHealth() const { return health_.maximum; }
    float getHealthPercent() const {
        return maximumHealth() > 0 ?
            static_cast<float>(currentHealth()) / static_cast<float>(maximumHealth()) : 1.0f;
    }
    void takeDamage(int damage);
    void heal(int amount);
    bool isDead() const { return health_.isDead; }
    void setDead(bool dead) { health_.isDead = dead; }

    // Health accessors for convenience
    int currentHealth() const { return health_.current; }
    int maximumHealth() const { return health_.maximum; }

    // Behavior state (low-frequency access - stamina, alertness, timers)
    shared::BehaviorState& behaviorState() { return behaviorState_; }
    const shared::BehaviorState& behaviorState() const { return behaviorState_; }

    // Virtual method for type-specific special behaviors
    // Returns true if entity should be deactivated (e.g., Creeper exploded)
    virtual bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq);

    // Type queries - default implementation, can be overridden
    virtual bool hasSpecialBehavior() const { return false; }

protected:
    uint32_t id_;
    shared::EntityType type_;

    // Low-frequency data
    struct HealthData {
        int current = 10;
        int maximum = 10;
        bool isDead = false;
    } health_;

    shared::BehaviorState behaviorState_;
};

// Specialized entities that override behavior
class CreeperEntity : public HybridEntity {
public:
    explicit CreeperEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    bool ignited_ = false;
    float fuseTimer_ = 0.0f;
};

class EndermanEntity : public HybridEntity {
public:
    explicit EndermanEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float teleportCooldown_ = 0.0f;
};

class BatEntity : public HybridEntity {
public:
    explicit BatEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float heightBlendTimer_ = 0.0f;
};

class BeeEntity : public HybridEntity {
public:
    explicit BeeEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float angerTimer_ = 0.0f;
};

class FishEntity : public HybridEntity {
public:
    explicit FishEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float directionTimer_ = 0.0f;
};

class DolphinEntity : public HybridEntity {
public:
    explicit DolphinEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float jumpTimer_ = 0.0f;
    float jumpCooldown_ = 0.0f;
};

class PhantomEntity : public HybridEntity {
public:
    explicit PhantomEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }

private:
    float diveTimer_ = 0.0f;
    float diveCooldown_ = 0.0f;
};

class SquidEntity : public HybridEntity {
public:
    explicit SquidEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }
};

class TurtleEntity : public HybridEntity {
public:
    explicit TurtleEntity(uint32_t id);

    bool updateSpecialBehavior(
        float& x, float& y, float& z,
        float& dirX, float& dirZ,
        float dt,
        bool hasTarget,
        float targetDistanceSq,
        float targetRangeSq) override;

    bool hasSpecialBehavior() const override { return true; }
};

// Factory function to create entities
std::unique_ptr<HybridEntity> createEntity(uint32_t id, shared::EntityType type);

// Helper to check if entity type has special behavior
bool hasSpecialBehavior(shared::EntityType type);

} // namespace hybrid
