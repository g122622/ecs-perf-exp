#include "world.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include <random>

namespace ecs {

class EntityFactory {
public:
    static EntityFactory& instance() {
        static EntityFactory factory;
        return factory;
    }

    void setSeed(unsigned int seed) {
        rng_.seed(seed);
    }

    shared::EntityType getRandomEntityType() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float r = dist(rng_);

        // Monster 30%, Animal 40%, Flying 15%, Aquatic 15%
        if (r < 0.30f) {
            std::uniform_int_distribution<int> monsterDist(0, 7);
            return static_cast<shared::EntityType>(monsterDist(rng_));
        } else if (r < 0.70f) {
            std::uniform_int_distribution<int> animalDist(8, 15);
            return static_cast<shared::EntityType>(animalDist(rng_));
        } else if (r < 0.85f) {
            std::uniform_int_distribution<int> flyingDist(16, 17);
            return static_cast<shared::EntityType>(flyingDist(rng_));
        } else {
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

} // namespace ecs
