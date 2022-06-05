#ifndef PLAYER_H
#define PLAYER_H

#include <box2d/box2d.h>
#include <unordered_map>
#include "Entity.h"
#include "Item.h"

class Item;
/// Controlled by the player. Most complex than most entities
class Player : public Entity {
   public:
    enum EqSlotId {
        LEFT_HAND,
        RIGHT_HAND
    };

    struct EqSlot {
        Item* item;
        b2RevoluteJoint* joint;
        Angle angle;

        EqSlot(Item*, b2RevoluteJoint*, Angle);
    };
    static const std::unordered_map<EqSlotId, Angle> defaultHoldAngles;

   protected:
    std::unordered_map<EqSlotId, EqSlot> equipment;  // contents are based on defaultHoldAngles

    static constexpr float ANGLE_FRONT = 0;
    static constexpr float ANGLE_SIDE = 45;
    static constexpr float DIST_HELD = 7;

   private:
    EqSlotId findKeyWithItem(const Item*) const;
    Angle getDefaultAngle(const EqSlotId&) const;
    void adjustJointDefToItem(const Item*);

   protected:
    Item* itemFront = nullptr;    ///< Either left or right, should be held in front
    b2RevoluteJointDef jointDef;  ///< Joint definition for hand slots. Used to create joints each time an item is picked up

   public:
    Player();

    void moveItems();       ///< Adjust item angles
    void tickItemTimers();  ///< Lower cooldown and trigger action for current tick

    void setItemAngle(Item* item, Angle angle);
    void resetItemAngle(Item* item);

    void equip(Item* const item, EqSlotId);
    void drop(EqSlotId);

    void prepareItem(EqSlotId);
    void triggerAction(EqSlotId);

    virtual std::vector<b2PolygonShape> getBaseShapes() const;
};

#endif  // PLAYER_H