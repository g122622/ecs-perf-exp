#include "living_entity.hpp"

namespace oop {

LivingEntity::LivingEntity(EntityID id, int maxHealth)
    : Entity(id)
    , health_(maxHealth)
    , maxHealth_(maxHealth)
    , regenRate_(0.0f) {
}

float LivingEntity::getHealthPercent() const {
    return static_cast<float>(health_) / static_cast<float>(maxHealth_);
}

void LivingEntity::takeDamage(int damage) {
    health_ -= damage;
    if (health_ < 0) {
        health_ = 0;
    }
}

void LivingEntity::heal(int amount) {
    health_ += amount;
    if (health_ > maxHealth_) {
        health_ = maxHealth_;
    }
}

void LivingEntity::updateRegeneration(float dt) {
    if (regenRate_ > 0.0f && health_ < maxHealth_) {
        float healAmount = regenRate_ * dt;
        health_ = static_cast<int>(health_ + healAmount);
        if (health_ > maxHealth_) {
            health_ = maxHealth_;
        }
    }
}

} // namespace oop
