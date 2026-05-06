#include "systems/ai_system.hpp"
#include <cmath>

namespace ecs {

AISystem::AISystem(EntityManager& entities,
                   ComponentPool<TransformComponent>& transforms,
                   ComponentPool<AIComponent>& aiStates,
                   ComponentPool<AttributesComponent>& attributes,
                   ComponentPool<HealthComponent>& health)
    : entities_(entities)
    , transforms_(transforms)
    , aiStates_(aiStates)
    , attributes_(attributes)
    , health_(health)
    , rng_(shared::RandomConstants::DEFAULT_SEED)
    , dist_(0.0f, 1.0f) {
}

void AISystem::update(float dt) {
    transforms_.forEach([&](EntityID id, TransformComponent& transform) {
        AIComponent* ai = aiStates_.get(id);
        AttributesComponent* attr = attributes_.get(id);
        HealthComponent* hp = health_.get(id);

        if (!ai || !attr) return;

        // 更新状态计时器
        ai->stateTimer += dt;

        // 根据当前状态更新
        switch (ai->state) {
            case shared::AIState::Idle:
                updateIdle(*ai, *attr, dt);
                break;
            case shared::AIState::Wander:
                updateWander(*ai, *attr, dt);
                break;
            case shared::AIState::Chase:
                updateChase(*ai, *attr, dt);
                break;
            case shared::AIState::Flee:
                updateFlee(*ai, *attr, dt);
                break;
            default:
                break;
        }

        // 低血量逃跑
        if (hp && hp->getHealthPercent() < 0.3f && attr->isPassive()) {
            ai->state = shared::AIState::Flee;
        }

        // 更新攻击冷却
        if (attr->attackCooldownTimer > 0.0f) {
            attr->attackCooldownTimer -= dt;
        }
    });
}

void AISystem::updateIdle(AIComponent& ai, AttributesComponent& attr, float dt) {
    // 空闲一段时间后开始游荡
    if (ai.stateTimer > shared::WorldConstants::IDLE_DURATION_MAX) {
        ai.state = shared::AIState::Wander;
        ai.stateTimer = 0.0f;
        ai.stateDuration = shared::WorldConstants::WANDER_DURATION_MIN +
                          dist_(rng_) * (shared::WorldConstants::WANDER_DURATION_MAX -
                                        shared::WorldConstants::WANDER_DURATION_MIN);
    }
}

void AISystem::updateWander(AIComponent& ai, AttributesComponent& attr, float dt) {
    // 游荡一段时间后可能空闲
    if (ai.stateTimer > ai.stateDuration) {
        ai.state = shared::AIState::Idle;
        ai.stateTimer = 0.0f;
        ai.wanderDirX = 0.0f;
        ai.wanderDirZ = 0.0f;
    }

    // 敌对生物寻找目标
    if (attr.isHostile() && ai.targetId == INVALID_ENTITY_ID) {
        // 目标在InteractionSystem中设置
    }
}

void AISystem::updateChase(AIComponent& ai, AttributesComponent& attr, float dt) {
    // 追踪目标
    if (ai.targetId == INVALID_ENTITY_ID) {
        ai.state = shared::AIState::Wander;
        ai.stateTimer = 0.0f;
        return;
    }

    // 追踪超时
    if (ai.stateTimer > shared::WorldConstants::CHASE_DURATION_MAX) {
        ai.targetId = INVALID_ENTITY_ID;
        ai.state = shared::AIState::Wander;
        ai.stateTimer = 0.0f;
    }
}

void AISystem::updateFlee(AIComponent& ai, AttributesComponent& attr, float dt) {
    // 逃跑一段时间后恢复游荡
    if (ai.stateTimer > shared::WorldConstants::FLEE_DURATION_MAX) {
        ai.state = shared::AIState::Wander;
        ai.stateTimer = 0.0f;
        ai.wanderDirX = 0.0f;
        ai.wanderDirZ = 0.0f;
    }
}

} // namespace ecs
