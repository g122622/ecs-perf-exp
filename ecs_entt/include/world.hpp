#pragma once

#include "entt.hpp"
#include "components.hpp"
#include "shared/entity_types.hpp"
#include "shared/constants.hpp"

namespace ecs_entt {

class World {
public:
    World(size_t initialCapacity = 10000);
    ~World() = default;

    // Entity management
    Entity createEntity(shared::EntityType type, float x, float y, float z);
    void destroyEntity(Entity entity);
    bool isAlive(Entity entity) const;

    // Component access (for convenience)
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

    // System update
    void update(float dt);

    // Queries
    size_t getEntityCount() const {
        // Count entities by checking Transform component
        return registry_.view<const Transform>().size();
    }

    // Clear
    void clear() { registry_.clear(); }

    // Registry access (for views)
    entt::registry& registry() { return registry_; }
    const entt::registry& registry() const { return registry_; }

private:
    void createEntityComponents(Entity entity, shared::EntityType type, float x, float y, float z);

    entt::registry registry_;
};

class MovementSystem {
public:
    static void update(entt::registry& registry, float dt);
};

class AISystem {
public:
    static void update(entt::registry& registry, float dt);
};

class DecaySystem {
public:
    static void update(entt::registry& registry, float dt);
};

class InteractionSystem {
public:
    static void update(entt::registry& registry, float dt);
};

} // namespace ecs_entt
