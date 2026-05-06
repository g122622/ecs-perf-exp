#include "entity.hpp"

namespace oop {

Entity::Entity(EntityID id)
    : id_(id)
    , x_(0.0f)
    , y_(0.0f)
    , z_(0.0f)
    , active_(true) {
}

void Entity::setPosition(float x, float y, float z) {
    x_ = x;
    y_ = y;
    z_ = z;
}

} // namespace oop
