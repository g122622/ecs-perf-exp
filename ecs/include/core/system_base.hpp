#pragma once

#include "entity_manager.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace ecs {

// 系统基类
class SystemBase {
public:
    virtual ~SystemBase() = default;
    virtual void update(float dt) = 0;
    virtual const char* getName() const = 0;
};

// 系统优先级 (执行顺序)
enum class SystemPriority : int {
    First = 0,
    AI = 100,
    Movement = 200,
    Physics = 300,
    Decay = 400,
    Interaction = 500,
    Last = 1000
};

} // namespace ecs
