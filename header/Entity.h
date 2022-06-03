#pragma once

#include "PhysicalObject.h"

/// A typical object with health pool. Most can be destroyed
class Entity : public PhysicalObject {
   protected:
    bool invulnerable = false;
    int maxHp = 100;
    int hp = 100;

   public:
    bool isInvulnerable() const { return invulnerable; }

    int getHp() { return hp; }

    int getMaxHp() { return maxHp; }

    void damage(int value) {
        if (isInvulnerable())
            return;

        hp = hp - value;
        if (hp <= 0) {
            hp = 0;
            // Cannot destroy body here, could trigger when running step(). Checked inside game loop
            toDestroy = true;
        }
    }

    void synchronize() {
        float bodyPositionX = body->GetPosition().x * M_TO_PX;
        float bodyPositionY = body->GetPosition().y * M_TO_PX;
        float bodyRotate = getAngle().get();

        for (auto& view : views) {
            view.setPosition(bodyPositionX, bodyPositionY);
            view.setRotation(bodyRotate);
        }

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
};