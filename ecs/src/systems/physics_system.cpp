#include "systems/physics_system.hpp"
#include <cmath>

namespace ecs {

PhysicsSystem::PhysicsSystem(EntityManager& entities,
                              ComponentPool<TransformComponent>& transforms,
                              ComponentPool<PhysicsComponent>& physics)
    : entities_(entities)
    , transforms_(transforms)
    , physics_(physics) {
}

void PhysicsSystem::update(float dt) {
    transforms_.forEach([&](EntityID id, TransformComponent& transform) {
        PhysicsComponent* phys = physics_.get(id);
        if (!phys) return;

        // 应用加速度
        phys->vx += phys->ax * dt;
        phys->vy += phys->ay * dt;
        phys->vz += phys->az * dt;

        // 应用速度到位置
        transform.x += phys->vx * dt;
        transform.y += phys->vy * dt;
        transform.z += phys->vz * dt;

        // 应用阻力
        phys->vx *= phys->drag;
        phys->vy *= phys->drag;
        phys->vz *= phys->drag;

        // 重置加速度
        phys->ax = 0.0f;
        phys->ay = 0.0f;
        phys->az = 0.0f;
    });
}

} // namespace ecs
