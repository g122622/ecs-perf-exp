#pragma once

namespace ecs {

// 生命值组件
struct HealthComponent {
    int current = 0;
    int maximum = 0;
    float regenRate = 0.0f;  // 每秒恢复
    bool isDead = false;

    HealthComponent() = default;
    HealthComponent(int maxHP, float regen = 0.0f)
        : current(maxHP), maximum(maxHP), regenRate(regen) {}

    float getHealthPercent() const {
        return maximum > 0 ? static_cast<float>(current) / maximum : 0.0f;
    }

    void takeDamage(int damage) {
        current -= damage;
        if (current < 0) current = 0;
        isDead = (current <= 0);
    }

    void heal(int amount) {
        current += amount;
        if (current > maximum) current = maximum;
    }
};

} // namespace ecs
