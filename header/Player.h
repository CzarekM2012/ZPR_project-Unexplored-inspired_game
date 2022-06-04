#ifndef PLAYER_H
#define PLAYER_H

#include <tuple>
#include <unordered_map>
#include "Entity.h"

class Item;

/// Controlled by the player. Most complex than most entities
class Player : public Entity {
   public:
    enum EqSlot {
        NONE,
        LEFT_HAND,
        RIGHT_HAND
    };

   protected:
    static constexpr float ANGLE_FRONT = 0;
    static constexpr float ANGLE_SIDE = 45;
    static constexpr float DIST_HELD = 7;

   private:
    EqSlot findKeyWithItem(const Item*) const;
    void adjustJointDefToItem(const Item*);

    static const int SLOT_ITEM = 0;
    static const int SLOT_JOINT = 1;
    static const int SLOT_ANGLE = 2;

    static const std::unordered_map<EqSlot, Angle> defaultHoldAngles;

   protected:
    std::unordered_map<Player::EqSlot, std::tuple<Item*, b2RevoluteJoint*, Angle>> equipment;  // contents are based on defaultHoldAngles

    Item* itemFront = nullptr;    ///< Either left or right, should be held in front
    b2RevoluteJointDef jointDef;  ///< Joint definition for hand slots. Used to create joints each time an item is picked up

   public:
    Player();
    ~Player();

    void moveItems();       ///< Adjust item angles
    void tickItemTimers();  ///< Lower cooldown and trigger action for current tick

    void setItemAngle(Item* item, Angle angle);
    void resetItemAngle(Item* item);

    void equip(Item* const item, EqSlot);
    void drop(EqSlot);

    void prepareItem(EqSlot);
    void triggerAction(EqSlot);

    virtual std::vector<b2PolygonShape> getBaseShapes() const;
};

#endif  // PLAYER_H