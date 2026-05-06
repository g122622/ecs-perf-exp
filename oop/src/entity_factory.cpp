#include "world.hpp"
#include "entities/monsters.hpp"
#include "entities/animals.hpp"
#include "entities/flying.hpp"
#include "entities/aquatic.hpp"
#include "shared/entity_types.hpp"
#include <random>

namespace oop {

class EntityFactory {
public:
    static EntityFactory& instance() {
        static EntityFactory factory;
        return factory;
    }

    void setSeed(unsigned int seed) {
        rng_.seed(seed);
    }

    // 根据类型创建实体
    std::unique_ptr<Entity> createEntity(shared::EntityType type, EntityID id,
                                          float x, float y, float z) {
        std::unique_ptr<Entity> entity;

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
                return nullptr;
        }

        if (entity) {
            entity->setPosition(x, y, z);
        }

        return entity;
    }

    // 按比例生成随机实体类型
    shared::EntityType getRandomEntityType() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float r = dist(rng_);

        // Monster 30%, Animal 40%, Flying 15%, Aquatic 15%
        if (r < 0.30f) {
            // Monster
            std::uniform_int_distribution<int> monsterDist(0, 7);
            return static_cast<shared::EntityType>(monsterDist(rng_));
        } else if (r < 0.70f) {
            // Animal
            std::uniform_int_distribution<int> animalDist(8, 15);
            return static_cast<shared::EntityType>(animalDist(rng_));
        } else if (r < 0.85f) {
            // Flying
            std::uniform_int_distribution<int> flyingDist(16, 17);
            return static_cast<shared::EntityType>(flyingDist(rng_));
        } else {
            // Aquatic
            std::uniform_int_distribution<int> aquaticDist(18, 21);
            return static_cast<shared::EntityType>(aquaticDist(rng_));
        }
    }

    float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng_);
    }

private:
    EntityFactory() : rng_(shared::RandomConstants::DEFAULT_SEED) {}
    std::mt19937 rng_;
};

} // namespace oop
