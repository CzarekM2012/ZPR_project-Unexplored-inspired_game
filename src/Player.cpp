#include "Player.h"
#include <algorithm>
#include "ObjectClasses.h"

const std::unordered_map<Player::EqSlot, Angle> Player::defaultHoldAngles = {
    {LEFT_HAND, Angle(ANGLE_SIDE)},
    {RIGHT_HAND, Angle(ANGLE_SIDE)}};  /// Should not have EqSlot::NONE key

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

    std::for_each(defaultHoldAngles.begin(), defaultHoldAngles.end(), [&](auto const& smth) {
        equipment[smth.first] = std::make_tuple(nullptr, nullptr, smth.second);
    });
}

Player::~Player() {
    // Very similar to drop...Hand, just doesn't destroy joint (it's destroyed together with body it is attached to)
    std::for_each(equipment.begin(), equipment.end(), [](const auto& pair) {
        auto item = std::get<SLOT_ITEM>(pair.second);
        if (item) {
            item->setOwner(nullptr);
            item->setCollision(false);
            item->resetCooldown();
        }
    });
}

void Player::moveItems() {
    Item *itemLH = std::get<SLOT_ITEM>(equipment.at(LEFT_HAND)), *itemRH = std::get<SLOT_ITEM>(equipment.at(RIGHT_HAND));
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
        auto item = std::get<SLOT_ITEM>(slot);
        auto joint = std::get<SLOT_JOINT>(slot);
        if (item && joint) {
            auto angle = std::get<SLOT_ANGLE>(slot);
            float angleDiff = -(joint->GetJointAngle() + angle.get(Angle::unit::RAD));

            if (itemFront == item)
                angleDiff = -joint->GetJointAngle();

            joint->SetMotorSpeed(angleDiff);
        }
    });
}

void Player::tickItemTimers() {
    std::for_each(equipment.begin(), equipment.end(), [](const auto& pair) {
        auto item = std::get<SLOT_ITEM>(pair.second);
        if (item) {
            item->useTick();
            item->lowerCooldown();
        }
    });
}

Player::EqSlot Player::findKeyWithItem(const Item* item) const {
    // with small number of equipment slots, linear search is good enough
    for (auto iter = equipment.begin(); iter != equipment.end(); ++iter) {
        if (std::get<SLOT_ITEM>(iter->second) == item) {
            return iter->first;
        }
    }
    return EqSlot::NONE;
}

void Player::setItemAngle(Item* item, Angle angle) {
    auto slotId = findKeyWithItem(item);  // key for slot where the item is or NONE
    equipment.at(slotId) = std::make_tuple(item, std::get<SLOT_JOINT>(equipment.at(slotId)), angle);
}

void Player::resetItemAngle(Item* item) {
    auto slotId = findKeyWithItem(item);  // key for slot where the item is or NONE
    equipment.at(slotId) = std::make_tuple(item, std::get<SLOT_JOINT>(equipment.at(slotId)), defaultHoldAngles.at(slotId));
}

void Player::equip(Item* const item, EqSlot slotId) {
    auto slot = equipment.at(slotId);
    if (std::get<SLOT_ITEM>(slot))
        drop(slotId);

    item->setOwner(this);
    item->setCollisionGroup(collisionGroup);  // Must be called before setting cooldown, because it also turns collision on to update it
    item->setCooldown(std::max(item->cooldownCollision, item->cooldownAction));

    adjustJointDefToItem(item);
    auto joint = dynamic_cast<b2RevoluteJoint*>(body->GetWorld()->CreateJoint(&jointDef));

    equipment.at(slotId) = std::make_tuple(item, joint, defaultHoldAngles.at(slotId));
}

void Player::adjustJointDefToItem(const Item* item) {
    jointDef.bodyA = body;
    jointDef.bodyB = item->getBodyPtr();
    jointDef.localAnchorB.Set(0, -DIST_HELD - (item->getLength() / 2 - item->getBodyPtr()->GetLocalCenter().y));  // This way items can be rotated around the player

    // Box2d anlges are form (-inf, inf), instead of (-pi, pi>
    // angle is set based on angles of 2 bodies, which can be several rotations apart
    // this formula compansates for them
    jointDef.referenceAngle = -round((body->GetAngle() - item->getBodyPtr()->GetAngle()) / (2 * b2_pi)) * 2 * b2_pi;
}

void Player::drop(EqSlot slotId) {
    auto slot = equipment.at(slotId);
    auto item = std::get<SLOT_ITEM>(slot);
    if (!item)
        return;

    item->setOwner(nullptr);
    item->setCollision(false);
    item->resetCooldown();

    auto joint = std::get<SLOT_JOINT>(slot);
    body->GetWorld()->DestroyJoint(joint);

    equipment.at(slotId) = std::make_tuple(nullptr, nullptr, defaultHoldAngles.at(slotId));
}

void Player::triggerAction(EqSlot slotId) {
    auto item = std::get<SLOT_ITEM>(equipment.at(slotId));
    if (item)
        item->useTrigger();
}

void Player::prepareItem(EqSlot slotId) {
    auto slot = equipment.at(slotId);
    auto item = std::get<SLOT_ITEM>(slot);
    if (item && item->canBeUsed())
        equipment.at(slotId) = std::make_tuple(item, std::get<SLOT_JOINT>(slot), Angle(item->prepareAngle));
}

std::vector<b2PolygonShape> Player::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)};
    shape.Set(pentagon, 5);
    shapeVec.push_back(shape);
    return shapeVec;
}
