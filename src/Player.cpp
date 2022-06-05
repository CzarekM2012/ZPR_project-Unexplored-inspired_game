#include "Player.h"
#include <algorithm>
#include "ObjectClasses.h"

#include <iostream>

const std::unordered_map<Player::EqSlotId, Angle> Player::defaultHoldAngles = {
    {Player::EqSlotId::LEFT_HAND, Angle(ANGLE_SIDE)},
    {Player::EqSlotId::RIGHT_HAND, Angle(ANGLE_SIDE)}};

Player::EqSlot::EqSlot(Item* sItem = nullptr, b2RevoluteJoint* sJoint = nullptr, Angle sAngle = Angle())
    : item(sItem), joint(sJoint), angle(sAngle) {
}

/// Controlled by the player. Most complex than most entities
Player::Player() {
    maxHp = 100;
    hp = 100;
    primaryColor = sf::Color::Green;

    jointDef.collideConnected = false;
    jointDef.enableMotor = true;
    jointDef.motorSpeed = 0;  // regulated later in GameController to adjust angle
    jointDef.maxMotorTorque = 1;
    jointDef.localAnchorA.Set(0, 0);
}

void Player::moveItems() {
    Item *itemLH = equipment.at(LEFT_HAND).item, *itemRH = equipment.at(RIGHT_HAND).item;
    if (itemLH && itemLH->isBeingUsed()) {
        itemFront = nullptr;
    } else if (itemRH && itemRH->isBeingUsed()) {
        itemFront = nullptr;
    } else if (itemLH && itemLH->holdInFrontWhenPossible && !itemLH->isOnCooldown()) {
        itemFront = itemLH;
    } else if (itemRH && itemRH->holdInFrontWhenPossible && !itemRH->isOnCooldown()) {
        itemFront = itemRH;
    } else if (itemLH && itemLH->holdInFrontWhenPossible) {
        itemFront = itemLH;  // E.g. both are on cooldown
    } else {
        itemFront = nullptr;
    }

    std::for_each(equipment.begin(), equipment.end(), [&](const auto& pair) {
        auto slot = pair.second;
        if (slot.item && slot.joint) {
            if (pair.first == RIGHT_HAND)  // Reverse all right hand angles
                slot.angle = -slot.angle;
            float angleDiff = -(slot.joint->GetJointAngle() + slot.angle.get(Angle::unit::RAD));

            if (itemFront == slot.item)
                angleDiff = -slot.joint->GetJointAngle();

            slot.joint->SetMotorSpeed(angleDiff);
        }
    });
}

void Player::tickItemTimers() {
    std::for_each(equipment.begin(), equipment.end(), [](const auto& pair) {
        auto item = pair.second.item;
        if (item) {
            item->useTick();
            item->lowerCooldown();
        }
    });
}

Player::EqSlotId Player::findKeyWithItem(const Item* item) const {
    // with small number of equipment slots, linear search is good enough
    for (auto iter = equipment.begin(); iter != equipment.end(); ++iter) {
        if (iter->second.item == item) {
            return iter->first;
        }
    }
    throw std::out_of_range("Key corresponding to given item not found");
}

void Player::setItemAngle(Item* item, Angle angle) {
    try {
        auto slotId = findKeyWithItem(item);
        equipment.at(slotId).angle = angle;
    } catch (const std::out_of_range&) {
    }
}

Angle Player::getDefaultAngle(const EqSlotId& slotId) const {
    return defaultHoldAngles.count(slotId) != 0 ? defaultHoldAngles.at(slotId) : Angle();
}

void Player::resetItemAngle(Item* item) {
    auto slotId = findKeyWithItem(item);  // key for slot where the item is or NONE
    equipment[slotId].angle = getDefaultAngle(slotId);
}

void Player::equip(Item* const item, EqSlotId slotId) {
    auto& slot = equipment[slotId];
    if (slot.item) {
        drop(slotId);
    }

    item->setOwner(this);
    item->setCollisionGroup(collisionGroup);  // Must be called before setting cooldown, because it also turns collision on to update it
    item->setCooldown(std::max(item->cooldownCollision, item->cooldownAction));
    slot.item = item;

    adjustJointDefToItem(item);
    slot.joint = dynamic_cast<b2RevoluteJoint*>(body->GetWorld()->CreateJoint(&jointDef));

    slot.angle = getDefaultAngle(slotId);
}

void Player::adjustJointDefToItem(const Item* item) {
    jointDef.bodyA = body;
    jointDef.bodyB = item->getBodyPtr();
    jointDef.localAnchorB.Set(0, -DIST_HELD - (item->getLength() / 2 - item->getBodyPtr()->GetLocalCenter().y));  // This way items can be rotated around the player

    // Box2d angles are form (-inf, inf), instead of (-pi, pi>.
    // If referenceAngle was set to 0, item would rotate around joint until angles of bodies of item and player got equal
    // to compensate for all rotations they made in reference to each other
    // In order to avoid that referenceAngle is set to <number_of_rotations>*FULL_ANGLE
    jointDef.referenceAngle = -round((body->GetAngle() - item->getBodyPtr()->GetAngle()) / (2 * b2_pi)) * 2 * b2_pi;
}

void Player::drop(EqSlotId slotId) {
    auto& slot = equipment[slotId];
    if (!slot.item)
        return;

    slot.item->setOwner(nullptr);
    slot.item->setCollision(false);
    slot.item->resetCooldown();

    body->GetWorld()->DestroyJoint(slot.joint);

    slot = EqSlot();
}

void Player::triggerAction(EqSlotId slotId) {
    auto item = equipment.at(slotId).item;
    if (item)
        item->useTrigger();
}

void Player::prepareItem(EqSlotId slotId) {
    auto& slot = equipment[slotId];
    if (slot.item && slot.item->canBeUsed())
        slot.angle = Angle(slot.item->prepareAngle);
}

std::vector<b2PolygonShape> Player::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)};
    shape.Set(pentagon, 5);
    shapeVec.push_back(shape);
    return shapeVec;
}
