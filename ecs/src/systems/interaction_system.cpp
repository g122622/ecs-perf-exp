#include "systems/interaction_system.hpp"
#include <cmath>

namespace ecs {

InteractionSystem::InteractionSystem(EntityManager& entities,
                                      ComponentPool<TransformComponent>& transforms,
                                      ComponentPool<AIComponent>& aiStates,
                                      ComponentPool<AttributesComponent>& attributes,
                                      ComponentPool<HealthComponent>& health)
    : entities_(entities)
    , transforms_(transforms)
    , aiStates_(aiStates)
    , attributes_(attributes)
    , health_(health) {
    nearbyEntities_.reserve(100);
}

void InteractionSystem::update(float dt) {
    transforms_.forEach([&](EntityID id, TransformComponent& transform) {
        AIComponent* ai = aiStates_.get(id);
        AttributesComponent* attr = attributes_.get(id);
        HealthComponent* hp = health_.get(id);

        if (!ai || !attr) return;

        // 敌对生物寻找并攻击目标
        if (attr->isHostile() && ai->targetId == INVALID_ENTITY_ID) {
            nearbyEntities_.clear();
            findEntitiesInRange(transform.x, transform.y, transform.z,
                               ai->perceptionRange, nearbyEntities_);

            // 找最近的被动生物作为目标
            for (EntityID targetId : nearbyEntities_) {
                if (targetId == id) continue;

                AttributesComponent* targetAttr = attributes_.get(targetId);
                if (targetAttr && targetAttr->isPassive()) {
                    ai->targetId = targetId;
                    ai->state = shared::AIState::Chase;
                    break;
                }
            }
        }

        // 攻击逻辑
        if (ai->targetId != INVALID_ENTITY_ID && attr->canAttack()) {
            TransformComponent* targetTransform = transforms_.get(ai->targetId);
            if (targetTransform) {
                float distSq = distanceSq(transform.x, transform.y, transform.z,
                                         targetTransform->x, targetTransform->y, targetTransform->z);

                float attackRangeSq = attr->attackRange * attr->attackRange;

                if (distSq <= attackRangeSq) {
                    // 在攻击范围内，进行攻击
                    HealthComponent* targetHp = health_.get(ai->targetId);
                    if (targetHp) {
                        targetHp->takeDamage(static_cast<int>(attr->attackDamage));
                        attr->attackCooldownTimer = attr->attackCooldown;

                        // 如果目标死亡，清除目标
                        if (targetHp->isDead) {
                            ai->targetId = INVALID_ENTITY_ID;
                            ai->state = shared::AIState::Wander;
                        }
                    }
                } else if (distSq > shared::WorldConstants::DEAGGRO_RANGE *
                                    shared::WorldConstants::DEAGGRO_RANGE) {
                    // 目标太远，放弃追踪
                    ai->targetId = INVALID_ENTITY_ID;
                    ai->state = shared::AIState::Wander;
                }
            } else {
                // 目标不存在
                ai->targetId = INVALID_ENTITY_ID;
                ai->state = shared::AIState::Wander;
            }
        }
    });
}

void InteractionSystem::findEntitiesInRange(float x, float y, float z, float range,
                                             std::vector<EntityID>& outIds) {
    float rangeSq = range * range;

    transforms_.forEach([&](EntityID id, const TransformComponent& t) {
        float distSq = distanceSq(x, y, z, t.x, t.y, t.z);
        if (distSq <= rangeSq) {
            outIds.push_back(id);
        }
    });
}

float InteractionSystem::distanceSq(float x1, float y1, float z1,
                                     float x2, float y2, float z2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    return dx * dx + dy * dy + dz * dz;
}

} // namespace ecs
