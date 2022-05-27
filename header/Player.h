#pragma once

#include "Entity.h"

class Item;

/// Controlled by the player. Most complex than most entities
class Player : public Entity {
   protected:
    const float ANGLE_FRONT = 0;
    const float ANGLE_LH = -30;
    const float ANGLE_RH = 30;
    const float DIST_HELD = 7;

    Item *itemLH, *itemRH;                       ///< Left and right hand items
    b2RevoluteJoint *jointLH, *jointRH;          ///< Left and right hand joint objects
    b2RevoluteJointDef jointDef;                 ///< Joint definition for hand slots. Used to create joints each time an item is picked up
    float targetAngleLH = 0, targetAngleRH = 0;  ///< Angle at which the item should be held. Can be changed to swing items

   public:
    Player();

    b2RevoluteJoint* getJointLeft() { return jointLH; }
    Item* getItemLH() { return itemLH; }

    void moveItems();
    void tickItemTimers();

    void setItemAngle(Item* item, float angle);
    void resetItemAngle(Item* item);

    void equipLeftHand(Item* const item);
    void equipRightHand(Item* const item);
    void dropLeftHand();
    void dropRightHand();
    void dropItem(Item* const item);

    void triggerActionLeft();
    void triggerActionRight();

    virtual std::vector<b2PolygonShape> getBaseShapes();
};