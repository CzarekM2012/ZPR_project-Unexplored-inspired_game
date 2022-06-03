#include "Item.h"

void Item::useTrigger() {
    if (canBeUsed()) {
        actionTimeLeft = getActionTimeTotal();
    }
}

void Item::useTick() {
    if (isBeingUsed())
        useTick(actionTimeLeft);

    if (actionTimeLeft-- == 1) {
        setCooldown(cooldownAction);
        if (auto player = dynamic_cast<Player*>(owner)) {
            player->resetItemAngle(this);
        }
    }
}

void Item::setCooldown(int ticks) {
    if (isOnCooldown()) {
        cooldown = std::max(cooldown, ticks);
        return;
    }
    cooldown = ticks;

    setCollision(false);
}

void Item::lowerCooldown() {
    if (--cooldown <= 0) {
        cooldown = 0;
        removeCooldownEffects();
    }
}

void Item::resetCooldown() {
    cooldown = 0;
    removeCooldownEffects();
}

void Item::removeCooldownEffects() {
    if (owner != nullptr)  // Otherwise could deal damage when dropped while on cooldown, after cooldown passes
        setCollision(true);
}

void Item::synchronize() {
    PhysicalObject::synchronize();

    auto colors = getBaseColors();
    for (unsigned int i = 0; i < views.size(); ++i) {
        auto& view = views[i];
        auto color = colors[i];

        if (isOnCooldown())
            view.setFillColor(sf::Color::Black);
        else
            view.setFillColor(color);
    }
}