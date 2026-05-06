#pragma once

namespace ecs {

// 物理组件 - 速度+加速度
struct PhysicsComponent {
    float vx = 0.0f;  // 速度
    float vy = 0.0f;
    float vz = 0.0f;
    float ax = 0.0f;  // 加速度
    float ay = 0.0f;
    float az = 0.0f;
    float drag = 0.98f;  // 阻力系数

    PhysicsComponent() = default;
    PhysicsComponent(float dvx, float dvy, float dvz)
        : vx(dvx), vy(dvy), vz(dvz) {}
};

} // namespace ecs
