#include "Player.h"
#include "ObjectClasses.h"

/// Controlled by the player. Most complex than most entities
Player::Player() {
    maxHp = 100;
    hp = 100;
    color = sf::Color::Green;

    jointDef.collideConnected = false;
    jointDef.enableMotor = true;
    jointDef.motorSpeed = 0;  // regulated later in GameController to adjust angle
    jointDef.maxMotorTorque = 1;
    jointDef.localAnchorA.Set(0, 0);
}

void Player::moveItems() {
    if (itemLH && jointLH) {
        float angleDiff = -(jointLH->GetJointAngle() + targetAngleLH * DEG_TO_R);
        jointLH->SetMotorSpeed(angleDiff);
    }
}

void Player::tickItemTimers() {
    if (itemLH) {
        itemLH->useTick();
        itemLH->lowerCooldown(1);
    }
}

void Player::setItemAngle(Item* item, float angle) {
    if (itemLH == item)
        targetAngleLH = angle;
}

void Player::resetItemAngle(Item* item) {
    if (itemLH == item)
        targetAngleLH = ANGLE_LH;
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
    UNUSED(item);
}

void Player::dropRightHand() {
}

void Player::dropItem(Item* const item) {
    item->createPhysicalObject(this->body->GetWorld(), this->body->GetPosition().x, this->body->GetPosition().y, this->body->GetAngle() * R_TO_DEG);
    item->setCollision(false);
    item->setOwner(nullptr);
}

void Player::triggerActionLeft() {
    if (itemLH)
        itemLH->useTrigger();
}

void Player::triggerActionRight() {
    if (itemRH)
        itemRH->useTrigger();
}

std::vector<b2PolygonShape> Player::getBaseShapes() {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)};
    shape.Set(pentagon, 5);
    shapeVec.push_back(shape);
    return shapeVec;
}
