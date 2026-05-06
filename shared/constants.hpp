#pragma once

namespace shared {

// 世界常量
struct WorldConstants {
    static constexpr float WORLD_SIZE_X = 256.0f;
    static constexpr float WORLD_SIZE_Y = 64.0f;   // 高度
    static constexpr float WORLD_SIZE_Z = 256.0f;

    static constexpr float WATER_LEVEL = 30.0f;    // 水面高度

    // 物理参数
    static constexpr float GRAVITY = 9.81f;
    static constexpr float AIR_DRAG = 0.98f;       // 空气阻力
    static constexpr float WATER_DRAG = 0.8f;      // 水阻力
    static constexpr float GROUND_FRICTION = 0.9f;

    // 时间步长
    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;  // 60 FPS

    // AI参数
    static constexpr float IDLE_DURATION_MIN = 1.0f;    // 最小站立时间
    static constexpr float IDLE_DURATION_MAX = 5.0f;    // 最大站立时间
    static constexpr float WANDER_DURATION_MIN = 2.0f;  // 最小游荡时间
    static constexpr float WANDER_DURATION_MAX = 8.0f;  // 最大游荡时间
    static constexpr float CHASE_DURATION_MAX = 30.0f;  // 最大追踪时间
    static constexpr float FLEE_DURATION_MIN = 3.0f;    // 最小逃跑时间
    static constexpr float FLEE_DURATION_MAX = 10.0f;   // 最大逃跑时间

    // 战斗参数
    static constexpr float ATTACK_RANGE_MELEE = 1.5f;
    static constexpr float ATTACK_RANGE_RANGED = 20.0f;
    static constexpr float AGGRO_RANGE = 16.0f;         // 仇恨范围
    static constexpr float DEAGGRO_RANGE = 32.0f;       // 脱战范围

    // 碰撞检测
    static constexpr float COLLISION_RADIUS = 0.5f;
    static constexpr float INTERACTION_RANGE = 5.0f;
};

// 游戏状态常量
struct GameConstants {
    static constexpr int MAX_ENTITIES = 200000;      // 最大实体数
    static constexpr int ENTITY_ID_INVALID = -1;

    // 实体分布比例 (用于生成)
    static constexpr float MONSTER_RATIO = 0.30f;    // 30%
    static constexpr float ANIMAL_RATIO = 0.40f;     // 40%
    static constexpr float FLYING_RATIO = 0.15f;     // 15%
    static constexpr float AQUATIC_RATIO = 0.15f;    // 15%
};

// 随机数种子
struct RandomConstants {
    static constexpr unsigned int DEFAULT_SEED = 12345;
};

} // namespace shared
