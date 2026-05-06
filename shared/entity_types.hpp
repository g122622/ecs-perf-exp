#pragma once

#include <cstdint>
#include <array>

namespace shared {

// AI状态枚举
enum class AIState : uint8_t {
    Idle,       // 站立不动
    Wander,     // 游荡
    Chase,      // 追踪
    Flee,       // 逃跑
    Attack,     // 攻击
    Dead        // 死亡
};

// 实体类型枚举
enum class EntityType : uint16_t {
    // Monster (8种)
    Zombie,
    Skeleton,
    Spider,
    Creeper,
    Enderman,
    Blaze,
    Ghast,
    Phantom,

    // Animal (8种)
    Pig,
    Cow,
    Sheep,
    Chicken,
    Rabbit,
    Wolf,
    Cat,
    Fox,

    // Flying (2种)
    Bat,
    Bee,

    // Aquatic (4种)
    Fish,
    Squid,
    Dolphin,
    Turtle,

    Count
};

// 行为标志位
enum class BehaviorFlags : uint8_t {
    None = 0,
    Hostile = 1 << 0,       // 敌对
    Passive = 1 << 1,       // 被动
    Neutral = 1 << 2,       // 中立
    Flying = 1 << 3,        // 飞行
    Aquatic = 1 << 4,       // 水生
    Nocturnal = 1 << 5,     // 夜行
    Ranged = 1 << 6,        // 远程攻击
};

inline BehaviorFlags operator|(BehaviorFlags a, BehaviorFlags b) {
    return static_cast<BehaviorFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool hasFlag(BehaviorFlags flags, BehaviorFlags flag) {
    return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(flag)) != 0;
}

// 实体属性配置
struct EntityConfig {
    EntityType type = EntityType::Pig;
    const char* name = "";

    int maxHealth = 10;
    float moveSpeed = 0.2f;
    float perceptionRange = 10.0f;

    int attackDamage = 0;
    float attackRange = 0.0f;
    float attackCooldown = 1.0f;

    float flightHeight = 0.0f;
    float swimSpeed = 0.0f;
    float regenRate = 0.0f;

    BehaviorFlags behavior = BehaviorFlags::None;
    AIState defaultState = AIState::Idle;
};

// 实体配置表 (22种实体)
inline const EntityConfig& getEntityConfig(EntityType type) {
    static const EntityConfig configs[] = {
        // Monster类型 (0-7)
        {EntityType::Zombie, "Zombie", 20, 0.23f, 35.0f, 3, 1.0f, 2.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Wander},
        {EntityType::Skeleton, "Skeleton", 20, 0.25f, 16.0f, 2, 15.0f, 1.5f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Wander},
        {EntityType::Spider, "Spider", 16, 0.3f, 16.0f, 2, 1.0f, 1.5f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Idle},
        {EntityType::Creeper, "Creeper", 20, 0.2f, 16.0f, 15, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Wander},
        {EntityType::Enderman, "Enderman", 40, 0.3f, 64.0f, 7, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Neutral, AIState::Idle},
        {EntityType::Blaze, "Blaze", 20, 0.23f, 48.0f, 6, 20.0f, 1.0f, 5.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Wander},
        {EntityType::Ghast, "Ghast", 10, 0.1f, 100.0f, 17, 50.0f, 3.0f, 10.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Wander},
        {EntityType::Phantom, "Phantom", 20, 0.9f, 64.0f, 2, 1.0f, 0.5f, 15.0f, 0.0f, 0.0f, BehaviorFlags::Hostile, AIState::Idle},

        // Animal类型 (8-15)
        {EntityType::Pig, "Pig", 10, 0.25f, 12.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Cow, "Cow", 10, 0.2f, 10.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Sheep, "Sheep", 8, 0.23f, 10.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Chicken, "Chicken", 4, 0.25f, 8.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Rabbit, "Rabbit", 3, 0.5f, 8.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Wolf, "Wolf", 8, 0.3f, 20.0f, 2, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Neutral, AIState::Idle},
        {EntityType::Cat, "Cat", 10, 0.35f, 12.0f, 1, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Idle},
        {EntityType::Fox, "Fox", 10, 0.3f, 16.0f, 2, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, BehaviorFlags::Neutral, AIState::Idle},

        // Flying类型 (16-17)
        {EntityType::Bat, "Bat", 6, 0.2f, 5.0f, 0, 0.0f, 0.0f, 8.0f, 0.0f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Bee, "Bee", 10, 0.3f, 22.0f, 2, 1.0f, 0.5f, 5.0f, 0.0f, 0.0f, BehaviorFlags::Neutral, AIState::Wander},

        // Aquatic类型 (18-21)
        {EntityType::Fish, "Fish", 3, 0.2f, 5.0f, 0, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Squid, "Squid", 10, 0.15f, 8.0f, 0, 0.0f, 0.0f, 0.0f, 0.3f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
        {EntityType::Dolphin, "Dolphin", 10, 0.4f, 15.0f, 0, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, BehaviorFlags::Neutral, AIState::Wander},
        {EntityType::Turtle, "Turtle", 30, 0.08f, 10.0f, 0, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, BehaviorFlags::Passive, AIState::Wander},
    };

    size_t index = static_cast<size_t>(type);
    if (index < 22) {
        return configs[index];
    }
    return configs[0];
}

// 获取实体类型分类
inline bool isMonster(EntityType type) {
    return static_cast<size_t>(type) < 8;
}

inline bool isAnimal(EntityType type) {
    size_t idx = static_cast<size_t>(type);
    return idx >= 8 && idx < 16;
}

inline bool isFlying(EntityType type) {
    size_t idx = static_cast<size_t>(type);
    return idx >= 16 && idx < 18;
}

inline bool isAquatic(EntityType type) {
    size_t idx = static_cast<size_t>(type);
    return idx >= 18 && idx < 22;
}

} // namespace shared
