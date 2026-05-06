#include "world.hpp"
#include "entities/monsters.hpp"
#include "entities/animals.hpp"
#include "entities/flying.hpp"
#include "entities/aquatic.hpp"
#include "living_entity.hpp"
#include "monster.hpp"
#include <cmath>
#include <algorithm>

namespace oop {

World::World()
    : nextId_(1) {
    shared::resetSimulationClock();
}

EntityID World::spawnEntity(shared::EntityType type, float x, float y, float z) {
    EntityID id = nextId_++;
    std::unique_ptr<Entity> entity;

    // 根据类型创建具体实体
    switch (type) {
        // Monsters
        case shared::EntityType::Zombie:
            entity = std::make_unique<Zombie>(id);
            break;
        case shared::EntityType::Skeleton:
            entity = std::make_unique<Skeleton>(id);
            break;
        case shared::EntityType::Spider:
            entity = std::make_unique<Spider>(id);
            break;
        case shared::EntityType::Creeper:
            entity = std::make_unique<Creeper>(id);
            break;
        case shared::EntityType::Enderman:
            entity = std::make_unique<Enderman>(id);
            break;
        case shared::EntityType::Blaze:
            entity = std::make_unique<Blaze>(id);
            break;
        case shared::EntityType::Ghast:
            entity = std::make_unique<Ghast>(id);
            break;
        case shared::EntityType::Phantom:
            entity = std::make_unique<Phantom>(id);
            break;

        // Animals
        case shared::EntityType::Pig:
            entity = std::make_unique<Pig>(id);
            break;
        case shared::EntityType::Cow:
            entity = std::make_unique<Cow>(id);
            break;
        case shared::EntityType::Sheep:
            entity = std::make_unique<Sheep>(id);
            break;
        case shared::EntityType::Chicken:
            entity = std::make_unique<Chicken>(id);
            break;
        case shared::EntityType::Rabbit:
            entity = std::make_unique<Rabbit>(id);
            break;
        case shared::EntityType::Wolf:
            entity = std::make_unique<Wolf>(id);
            break;
        case shared::EntityType::Cat:
            entity = std::make_unique<Cat>(id);
            break;
        case shared::EntityType::Fox:
            entity = std::make_unique<Fox>(id);
            break;

        // Flying
        case shared::EntityType::Bat:
            entity = std::make_unique<Bat>(id);
            break;
        case shared::EntityType::Bee:
            entity = std::make_unique<Bee>(id);
            break;

        // Aquatic
        case shared::EntityType::Fish:
            entity = std::make_unique<Fish>(id);
            break;
        case shared::EntityType::Squid:
            entity = std::make_unique<Squid>(id);
            break;
        case shared::EntityType::Dolphin:
            entity = std::make_unique<Dolphin>(id);
            break;
        case shared::EntityType::Turtle:
            entity = std::make_unique<Turtle>(id);
            break;

        default:
            return INVALID_ENTITY_ID;
    }

    if (entity) {
        entity->setPosition(x, y, z);
        if (auto* creature = dynamic_cast<Creature*>(entity.get()); creature != nullptr) {
            creature->initializeBehaviorState(x, y, z);
        }
        entities_.push_back(std::move(entity));
        return id;
    }

    return INVALID_ENTITY_ID;
}

void World::destroyEntity(EntityID id) {
    auto it = std::remove_if(entities_.begin(), entities_.end(),
        [id](const std::unique_ptr<Entity>& e) {
            return e->getId() == id;
        });
    entities_.erase(it, entities_.end());
}

Entity* World::getEntity(EntityID id) {
    for (auto& entity : entities_) {
        if (entity->getId() == id) {
            return entity.get();
        }
    }
    return nullptr;
}

const Entity* World::getEntity(EntityID id) const {
    for (const auto& entity : entities_) {
        if (entity->getId() == id) {
            return entity.get();
        }
    }
    return nullptr;
}

void World::update(float dt) {
    shared::advanceSimulationClock(dt);

    // Update all entities
    for (auto& entity : entities_) {
        if (entity && entity->isActive()) {
            entity->update(dt);
        }
    }

    // Interaction system: hostile monsters find and attack passive animals
    updateInteractions(dt);
}

void World::updateInteractions(float dt) {
    (void)dt;
    // Collect hostile monsters and passive animals
    std::vector<Monster*> monsters;
    std::vector<Creature*> animals;

    for (auto& entity : entities_) {
        if (!entity || !entity->isActive()) continue;

        if (entity->isMonster()) {
            auto* monster = dynamic_cast<Monster*>(entity.get());
            if (monster) {
                monsters.push_back(monster);
            }
        } else if (entity->isAnimal()) {
            auto* animal = dynamic_cast<Creature*>(entity.get());
            if (animal) {
                animals.push_back(animal);
            }
        }
    }

    // Process monster interactions
    for (auto* monster : monsters) {
        const auto& profile = shared::getBehaviorProfile(monster->getEntityType());
        float px = monster->getX();
        float py = monster->getY();
        float pz = monster->getZ();
        float rangeScale = 1.0f + profile.aggression * 0.15f + shared::simulationContext().lightLevel * 0.05f;
        float range = monster->getPerceptionRange() * rangeScale;
        float rangeSq = range * range;

        // 先根据更细致的评分选择最近、最脆弱的猎物。
        if (!monster->hasTarget()) {
            Creature* bestTarget = nullptr;
            float bestScore = -1.0f;

            for (auto* animal : animals) {
                if (!animal->isActive()) continue;

                float dx = animal->getX() - px;
                float dy = animal->getY() - py;
                float dz = animal->getZ() - pz;
                float distSq = dx * dx + dy * dy + dz * dz;
                if (distSq > rangeSq) continue;

                auto* livingTarget = dynamic_cast<LivingEntity*>(animal);
                float targetHealthPercent = livingTarget != nullptr ? livingTarget->getHealthPercent() : 1.0f;
                float score = shared::scoreTargetCandidate(
                    profile,
                    shared::simulationContext(),
                    distSq,
                    targetHealthPercent,
                    true,
                    false,
                    shared::isFlying(animal->getEntityType()),
                    shared::isAquatic(animal->getEntityType()));

                if (score > bestScore) {
                    bestScore = score;
                    bestTarget = animal;
                }
            }

            if (bestTarget != nullptr) {
                monster->setTargetId(bestTarget->getId());
                // 计算目标距离
                float dx = bestTarget->getX() - px;
                float dy = bestTarget->getY() - py;
                float dz = bestTarget->getZ() - pz;
                monster->setTargetDistanceSq(dx * dx + dy * dy + dz * dz);
                monster->setAIState(shared::AIState::Chase);
                monster->setWanderDirection(bestTarget->getX() - px, bestTarget->getZ() - pz);
            }
        }

        // Attack logic
        if (monster->hasTarget() && monster->canAttack()) {
            EntityID targetId = monster->getTargetId();
            Entity* targetEntity = getEntity(targetId);

            if (targetEntity && targetEntity->isActive() && targetEntity->isLiving()) {
                auto* targetLiving = dynamic_cast<LivingEntity*>(targetEntity);
                if (targetLiving) {
                    float dx = targetEntity->getX() - monster->getX();
                    float dy = targetEntity->getY() - monster->getY();
                    float dz = targetEntity->getZ() - monster->getZ();
                    float distSq = dx * dx + dy * dy + dz * dz;

                    // 更新目标距离
                    monster->setTargetDistanceSq(distSq);

                    float attackRangeSq = monster->getAttackRange() * monster->getAttackRange();

                    if (distSq <= attackRangeSq) {
                        // In attack range - attack
                        monster->setAIState(shared::AIState::Attack);
                        monster->setWanderDirection(dx, dz);
                        monster->attack(targetLiving, distSq);

                        if (targetLiving->isDead()) {
                            monster->clearTarget();
                            monster->setAIState(shared::AIState::Wander);
                        }
                    } else if (distSq > shared::WorldConstants::DEAGGRO_RANGE * shared::WorldConstants::DEAGGRO_RANGE) {
                        // Too far - deaggro
                        monster->clearTarget();
                        monster->setAIState(shared::AIState::Wander);
                    } else {
                        monster->setAIState(shared::AIState::Chase);
                        monster->setWanderDirection(dx, dz);
                    }
                }
            } else {
                // Target no longer valid
                monster->clearTarget();
                monster->setAIState(shared::AIState::Wander);
            }
        }
    }

    // 被动生物在低血量或被敌对目标逼近时更倾向逃跑。
    for (auto* animal : animals) {
        if (!animal->isActive()) continue;

        const auto& profile = shared::getBehaviorProfile(animal->getEntityType());
        float ax = animal->getX();
        float ay = animal->getY();
        float az = animal->getZ();
        float perceptionRange = animal->getPerceptionRange() * (1.0f + profile.caution * 0.15f);
        float perceptionRangeSq = perceptionRange * perceptionRange;

        Monster* nearestThreat = nullptr;
        float nearestThreatDistSq = perceptionRangeSq;

        for (auto* monster : monsters) {
            if (!monster->isActive()) continue;

            float dx = monster->getX() - ax;
            float dy = monster->getY() - ay;
            float dz = monster->getZ() - az;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq < nearestThreatDistSq) {
                nearestThreatDistSq = distSq;
                nearestThreat = monster;
            }
        }

        if (nearestThreat != nullptr) {
            animal->setAIState(shared::AIState::Flee);
            animal->setWanderDirection(ax - nearestThreat->getX(), az - nearestThreat->getZ());
            animal->clearTarget();
        } else if (dynamic_cast<LivingEntity*>(animal)->getHealthPercent() <= profile.fleeHealthThreshold) {
            animal->setAIState(shared::AIState::Flee);
        }
    }
}

void World::forEachEntity(const std::function<void(Entity*)>& func) {
    for (auto& entity : entities_) {
        if (entity) {
            func(entity.get());
        }
    }
}

void World::forEachEntity(const std::function<void(const Entity*)>& func) const {
    for (const auto& entity : entities_) {
        if (entity) {
            func(entity.get());
        }
    }
}

void World::findEntitiesInRange(float x, float y, float z, float range,
                                 std::vector<EntityID>& outIds) const {
    float rangeSq = range * range;
    for (const auto& entity : entities_) {
        if (!entity || !entity->isActive()) continue;

        float dx = entity->getX() - x;
        float dy = entity->getY() - y;
        float dz = entity->getZ() - z;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq <= rangeSq) {
            outIds.push_back(entity->getId());
        }
    }
}

EntityID World::findNearestEntity(float x, float y, float z, float maxRange,
                                   std::function<bool(const Entity*)> filter) const {
    EntityID nearestId = INVALID_ENTITY_ID;
    float nearestDistSq = maxRange * maxRange;

    for (const auto& entity : entities_) {
        if (!entity || !entity->isActive()) continue;
        if (filter && !filter(entity.get())) continue;

        float dx = entity->getX() - x;
        float dy = entity->getY() - y;
        float dz = entity->getZ() - z;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearestId = entity->getId();
        }
    }

    return nearestId;
}

void World::clear() {
    entities_.clear();
    nextId_ = 1;
}

bool World::isInsideWorld(float x, float y, float z) const {
    return x >= 0.0f && x <= shared::WorldConstants::WORLD_SIZE_X &&
           y >= 0.0f && y <= shared::WorldConstants::WORLD_SIZE_Y &&
           z >= 0.0f && z <= shared::WorldConstants::WORLD_SIZE_Z;
}

} // namespace oop
