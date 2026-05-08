#pragma once

#include "components.hpp"
#include "hybrid_entity.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"
#include <entt.hpp>
#include <vector>
#include <memory>

namespace hybrid {

// Forward declarations
class AISystem;
class MovementSystem;
class InteractionSystem;

// Hybrid World - combines ECS components with OOP entity management
// High-frequency data stored in ECS components for cache-friendly iteration
// Low-frequency data and special behaviors in OOP classes
class HybridWorld {
public:
    explicit HybridWorld(size_t initialCapacity = 10000);
    ~HybridWorld() = default;

    // Entity management
    Entity spawnEntity(shared::EntityType type, float x, float y, float z);
    void destroyEntity(Entity entity);

    // Update all systems
    void update(float dt);

    // Queries
    size_t getEntityCount() const {
        return registry_.view<const TransformComponent>().size();
    }

    // Clear all entities
    void clear();

    // Component access (for systems)
    template<typename Component>
    Component& get(Entity entity) {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    const Component& get(Entity entity) const {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    bool has(Entity entity) const {
        return registry_.all_of<Component>(entity);
    }

    // OOP entity access (for special behaviors)
    HybridEntity* getOopEntity(uint32_t index) {
        if (index < oopEntities_.size()) {
            return oopEntities_[index].get();
        }
        return nullptr;
    }

    const HybridEntity* getOopEntity(uint32_t index) const {
        if (index < oopEntities_.size()) {
            return oopEntities_[index].get();
        }
        return nullptr;
    }

    // Registry access (for systems)
    entt::registry& registry() { return registry_; }
    const entt::registry& registry() const { return registry_; }

private:
    void createEntityComponents(Entity entity, shared::EntityType type, float x, float y, float z);

    entt::registry registry_;
    std::vector<std::unique_ptr<HybridEntity>> oopEntities_;
    uint32_t nextOopIndex_ = 0;
};

// System declarations
class AISystem {
public:
    static void update(entt::registry& registry, HybridWorld& world, float dt);
};

class MovementSystem {
public:
    static void update(entt::registry& registry, HybridWorld& world, float dt);
};

class InteractionSystem {
public:
    static void update(entt::registry& registry, HybridWorld& world, float dt);
};

} // namespace hybrid
