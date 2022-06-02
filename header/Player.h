#pragma once

#include "Entity.h"

class Item;

/// Controlled by the player. Most complex than most entities
class Player : public Entity {
   protected:
    const float ANGLE_FRONT = 0;
    const float ANGLE_SIDE = 45;
    const float DIST_HELD = 7;

    Item *itemLH = nullptr, *itemRH = nullptr;                     ///< Left and right hand items
    Item* itemFront = nullptr;                                     ///< Either left or right, should be held in front
    b2RevoluteJoint *jointLH = nullptr, *jointRH = nullptr;        ///< Left and right hand joint objects
    b2RevoluteJointDef jointDef;                                   ///< Joint definition for hand slots. Used to create joints each time an item is picked up
    float targetAngleLH = ANGLE_SIDE, targetAngleRH = ANGLE_SIDE;  ///< Angle at which the item should be held. Can be changed to swing items

   public:
    Player();
    ~Player();

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

    void prepareItemLeft();
    void prepareItemRight();
    void triggerActionLeft();
    void triggerActionRight();

    virtual std::vector<b2PolygonShape> getBaseShapes();
};
