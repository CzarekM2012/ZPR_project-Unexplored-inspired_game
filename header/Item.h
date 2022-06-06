#ifndef ITEM_H
#define ITEM_H

#include "Constants.h"
#include "Player.h"

/// Items can be equiped by players. They don't collide on the ground
class Item : public PhysicalObject {
   protected:
    Entity* owner = nullptr;
    int cooldown = 0;        ///< In ticks. While positive, disables collisions and use of item's abilities
    int actionTimeLeft = 0;  ///< In ticks. Positive means that action is being used and will continue for that many ticks

    /// Can be overloaded to set item behaviour in each action tick. Ticks count down from actionTimeTotal to 1
    virtual void useTick(int /*tick*/) {}

   public:
    int cooldownCollision = 1 * TICKS_PER_SECOND;
    int cooldownAction = 2 * TICKS_PER_SECOND;
    int actionTimeTotal = 1 * TICKS_PER_SECOND;
    float prepareAngle = 0;                ///< Angle at which this item should be held when being prepared for action
    bool holdInFrontWhenPossible = false;  ///< One of the items can be held in front of the player if it has this flag

    Item() { density = 0.001; };  // Density is set so low not to encumber the player too much. Cannot be set to 0 though, as it's reserved for static objects
    Entity* getOwner() { return owner; };
    void setOwner(Entity* newOwner) { owner = newOwner; };

    bool isOnCooldown() const { return cooldown > 0; }
    bool isBeingUsed() const { return getActionTimeLeft() > 0; }
    bool canBeUsed() const { return !isOnCooldown() && !isBeingUsed(); }

    void useTrigger();  ///< Trigger action. This function only sets action timer. useTick() takes care of rest
    void useTick();     ///< Continuous use, called each tick from GameController

    virtual float getActionTimeTotal() const { return actionTimeTotal; }
    float getActionTimeLeft() const { return actionTimeLeft; };

    void setCooldown(int ticks);  ///< Sets cooldown period and triggers its effects. If given value is larger than currnet cooldown timer, increases it to match
    void lowerCooldown();         ///< Lowers cooldown by 1 tick
    void resetCooldown();
    void removeCooldownEffects();  ///< Was more complex earlier on. Now it only turns collisions back on

    void synchronize();  ///< Custom synchronize function to switch color to black when on cooldown
};

#endif  // ITEM_H