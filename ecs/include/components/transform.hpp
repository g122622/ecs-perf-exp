#pragma once

namespace ecs {

// 变换组件 - 位置+旋转
struct TransformComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float rotationZ = 0.0f;

    TransformComponent() = default;
    TransformComponent(float px, float py, float pz)
        : x(px), y(py), z(pz) {}
};

} // namespace ecs
