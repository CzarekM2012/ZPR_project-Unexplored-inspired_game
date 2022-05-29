#include "Player.h"
#include "ObjectClasses.h"

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
    if (itemLH && itemLH->isBeingUsed()) {
        itemFront = nullptr;
    } else if (itemRH && itemRH->isBeingUsed()) {
        itemFront = nullptr;
    } else if (itemLH && itemLH->holdInFrontWhenPossible && !itemLH->isOnCooldown()) {
        itemFront = itemLH;
    } else if (itemRH && itemRH->holdInFrontWhenPossible && !itemRH->isOnCooldown()) {
        itemFront = itemRH;
    } else {
        itemFront = nullptr;
    }

    if (itemLH && jointLH) {
        float angleDiff = -(jointLH->GetJointAngle() + targetAngleLH * DEG_TO_R);

        if (itemFront == itemLH)
            angleDiff = -jointLH->GetJointAngle();

        jointLH->SetMotorSpeed(angleDiff);
    }
    if (itemRH && jointRH) {
        float angleDiff = -(jointRH->GetJointAngle() - targetAngleRH * DEG_TO_R);

        if (itemFront == itemRH)
            angleDiff = -jointRH->GetJointAngle();

        jointRH->SetMotorSpeed(angleDiff);
    }
}

void Player::tickItemTimers() {
    if (itemLH) {
        itemLH->useTick();
        itemLH->lowerCooldown(1);
    }
    if (itemRH) {
        itemRH->useTick();
        itemRH->lowerCooldown(1);
    }
}

void Player::setItemAngle(Item* item, float angle) {
    if (item == itemLH)
        targetAngleLH = angle;
    if (item == itemRH)
        targetAngleRH = angle;
}

void Player::resetItemAngle(Item* item) {
    if (item == itemLH)
        targetAngleLH = ANGLE_SIDE;

    if (item == itemRH)
        targetAngleRH = ANGLE_SIDE;
}

void Player::equipLeftHand(Item* const item) {
    if (itemLH != nullptr)
        dropLeftHand();

    itemLH = item;
    item->setOwner(this);
    item->setCooldown(std::max(item->cooldownCollision, item->cooldownAction));

    jointDef.bodyA = body;
    jointDef.bodyB = item->getBodyPtr();
    jointDef.localAnchorB.Set(0, -DIST_HELD - (item->getLength() / 2 - item->getBodyPtr()->GetLocalCenter().y));  // This way items can be rotated around the player

    // Box2d anlges are form (-inf, inf), instead of (-pi, pi>
    // angle is set based on angles of 2 bodies, which can be several rotations apart
    // this formula compansates for them
    jointDef.referenceAngle = DEG_TO_R - round((body->GetAngle() - item->getBodyPtr()->GetAngle()) / (2 * b2_pi)) * 2 * b2_pi;

    jointLH = dynamic_cast<b2RevoluteJoint*>(body->GetWorld()->CreateJoint(&jointDef));

    resetItemAngle(item);
}

void Player::dropLeftHand() {
    if (itemLH == nullptr)
        return;

    itemLH->setOwner(nullptr);
    itemLH->setCollision(false);
    itemLH->resetCooldown();
    itemLH = nullptr;
    body->GetWorld()->DestroyJoint(jointLH);
}

void Player::equipRightHand(Item* const item) {
    if (itemRH != nullptr)
        dropRightHand();

    itemRH = item;
    item->setOwner(this);
    item->setCooldown(std::max(item->cooldownCollision, item->cooldownAction));

    jointDef.bodyA = body;
    jointDef.bodyB = item->getBodyPtr();
    jointDef.localAnchorB.Set(0, -DIST_HELD - (item->getLength() / 2 - item->getBodyPtr()->GetLocalCenter().y));  // This way items can be rotated around the player

    // Box2d anlges are form (-inf, inf), instead of (-pi, pi>
    // angle is set based on angles of 2 bodies, which can be several rotations apart
    // this formula compansates for them
    jointDef.referenceAngle = DEG_TO_R - round((body->GetAngle() - item->getBodyPtr()->GetAngle()) / (2 * b2_pi)) * 2 * b2_pi;

    jointRH = dynamic_cast<b2RevoluteJoint*>(body->GetWorld()->CreateJoint(&jointDef));

    resetItemAngle(item);
}

void Player::dropRightHand() {
    if (itemRH == nullptr)
        return;

    itemRH->setOwner(nullptr);
    itemRH->setCollision(false);
    itemRH->resetCooldown();
    itemRH = nullptr;
    body->GetWorld()->DestroyJoint(jointRH);
}

void Player::triggerActionLeft() {
    if (itemLH) {
        itemLH->useTrigger();
    }
}

void Player::triggerActionRight() {
    if (itemRH) {
        itemRH->useTrigger();
    }
}

void Player::prepareItemLeft() {
    if (itemLH)
        targetAngleLH = itemLH->prepareAngle;
}

void Player::prepareItemRight() {
    if (itemRH)
        targetAngleRH = itemRH->prepareAngle;
}

std::vector<b2PolygonShape> Player::getBaseShapes() {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)};
    shape.Set(pentagon, 5);
    shapeVec.push_back(shape);
    return shapeVec;
}
