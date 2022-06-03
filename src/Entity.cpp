#include "Entity.h"

void Entity::damage(int value) {
    if (isInvulnerable())
        return;

    hp = hp - value;
    if (hp <= 0) {
        hp = 0;
        // Cannot destroy body here, could trigger when running step(). Checked inside game loop
        toDestroy = true;
    }
}

void Entity::synchronize() {
    PhysicalObject::synchronize();

    auto colors = getBaseColors();
    auto color = colors[0];
    float scale = static_cast<float>(hp) / maxHp;
    // std::cout << scale << std::endl;
    color.r *= scale;
    color.g *= scale;
    color.b *= scale;
    // Display HP
    views.back().setFillColor(color);
}