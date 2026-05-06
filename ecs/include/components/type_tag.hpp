#pragma once

#include "shared/entity_types.hpp"

namespace ecs {

// 类型标签组件 - 用于快速类型判断
struct TypeTagComponent {
    shared::EntityType type = shared::EntityType::Pig;
    bool isMonster = false;
    bool isAnimal = false;
    bool isFlying = false;
    bool isAquatic = false;

    TypeTagComponent() = default;
    TypeTagComponent(shared::EntityType t)
        : type(t) {
        isMonster = shared::isMonster(t);
        isAnimal = shared::isAnimal(t);
        isFlying = shared::isFlying(t);
        isAquatic = shared::isAquatic(t);
    }
};

} // namespace ecs
