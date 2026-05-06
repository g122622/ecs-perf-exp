#include "systems/movement_system.hpp"
#include <cmath>
#include <algorithm>

namespace ecs {

MovementSystem::MovementSystem(EntityManager& entities,
                                ComponentPool<TransformComponent>& transforms,
                                ComponentPool<PhysicsComponent>& physics,
                                ComponentPool<AIComponent>& aiStates,
                                ComponentPool<AttributesComponent>& attributes)
    : entities_(entities)
    , transforms_(transforms)
    , physics_(physics)
    , aiStates_(aiStates)
    , attributes_(attributes)
    , rng_(shared::RandomConstants::DEFAULT_SEED)
    , dist_(0.0f, 1.0f) {
}

void MovementSystem::update(float dt) {
    // 遍历所有有移动能力的实体
    transforms_.forEach([&](EntityID id, TransformComponent& transform) {
        PhysicsComponent* phys = physics_.get(id);
        AIComponent* ai = aiStates_.get(id);
        AttributesComponent* attr = attributes_.get(id);

        if (!phys || !ai || !attr) return;

        float dx = 0.0f, dz = 0.0f;
        float speed = attr->moveSpeed;

        // 根据AI状态决定移动
        switch (ai->state) {
            case shared::AIState::Idle:
                // 不移动
                break;

            case shared::AIState::Wander: {
                // 随机游荡方向
                if (ai->wanderDirX == 0.0f && ai->wanderDirZ == 0.0f) {
                    generateRandomDirection(ai->wanderDirX, ai->wanderDirZ);
                }
                dx = ai->wanderDirX * speed * dt;
                dz = ai->wanderDirZ * speed * dt;
                break;
            }

            case shared::AIState::Chase: {
                // 追踪目标
                if (ai->targetId != INVALID_ENTITY_ID) {
                    TransformComponent* targetTransform = transforms_.get(ai->targetId);
                    if (targetTransform) {
                        float dirX = targetTransform->x - transform.x;
                        float dirZ = targetTransform->z - transform.z;
                        float dist = std::sqrt(dirX * dirX + dirZ * dirZ);
                        if (dist > 0.001f) {
                            dx = (dirX / dist) * speed * dt;
                            dz = (dirZ / dist) * speed * dt;
                        }
                    }
                }
                break;
            }

            case shared::AIState::Flee: {
                // 逃跑
                if (ai->wanderDirX == 0.0f && ai->wanderDirZ == 0.0f) {
                    generateRandomDirection(ai->wanderDirX, ai->wanderDirZ);
                }
                dx = ai->wanderDirX * speed * 1.5f * dt;
                dz = ai->wanderDirZ * speed * 1.5f * dt;
                break;
            }

            default:
                break;
        }

        // 飞行生物处理
        if (attr->isFlying() && attr->flightHeight > 0.0f) {
            float targetHeight = attr->flightHeight;
            float heightDiff = targetHeight - transform.y;
            if (std::abs(heightDiff) > 0.1f) {
                transform.y += std::copysign(attr->moveSpeed * dt, heightDiff);
            }
        }

        // 水生生物处理
        if (attr->isAquatic() && attr->swimSpeed > 0.0f) {
            speed = attr->swimSpeed;
        }

        // 应用移动
        transform.x += dx;
        transform.z += dz;

        // 世界边界检查
        transform.x = std::clamp(transform.x, 0.0f, shared::WorldConstants::WORLD_SIZE_X);
        transform.y = std::clamp(transform.y, 0.0f, shared::WorldConstants::WORLD_SIZE_Y);
        transform.z = std::clamp(transform.z, 0.0f, shared::WorldConstants::WORLD_SIZE_Z);
    });
}

void MovementSystem::generateRandomDirection(float& dx, float& dz) {
    float angle = dist_(rng_) * 2.0f * 3.14159265f;
    dx = std::cos(angle);
    dz = std::sin(angle);
}

} // namespace ecs
