
#include "ObjectClasses.h"

void RectangleObject::setSize(float width, float height) {
    this->width = width;
    this->height = height;
}

std::vector<b2PolygonShape> RectangleObject::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    shape.SetAsBox(width / 2, height / 2);
    shapeVec.push_back(shape);
    return shapeVec;
}

Box::Box() {
    width = 10;
    height = 10;
    density = 0.1f;
    damping = 0.1f;
    primaryColor = sf::Color::Yellow;
}

Box::Box(int w, int h)
    : Box() {
    width = w;
    height = h;
}

Wall::Wall() {
    invulnerable = true;
    dynamic = false;
    primaryColor = sf::Color::Black;
}

Wall::Wall(int w, int h)
    : Wall() {
    width = w;
    height = h;
}

Sword::Sword() {
    damage = 10;
    primaryColor = sf::Color(180, 180, 180);
    holdInFrontWhenPossible = true;
}

void Sword::useTick(int tick) {
    switch (tick) {
        case TICKS_PER_SECOND:
            std::cout << "Sword action" << std::endl;
            if (auto player = dynamic_cast<Player*>(owner)) {
                player->setItemAngle(this, 0);
            }
            break;
    }
    auto ownerBody = owner->getBodyPtr();
    float angle = ownerBody->GetAngle();
    int force = 100;
    b2Vec2 vec = b2Vec2(-force * sin(angle), force * cos(angle));
    ownerBody->ApplyLinearImpulseToCenter(vec, true);
}

std::vector<b2PolygonShape> Sword::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 triangle[] = {b2Vec2(-3, 3), b2Vec2(0, 12), b2Vec2(3, 3)};
    shape.Set(triangle, 3);
    shapeVec.push_back(shape);

    shape.SetAsBox(0.5, 2, b2Vec2(0, 2), 0);
    shapeVec.push_back(shape);

    return shapeVec;
}

void Sword::onContact(PhysicalObject* const other) {
    auto target = dynamic_cast<Entity*>(other);
    if (target) {
        if (!isOnCooldown()) {
            if (isBeingUsed())
                target->damage(20);
            else
                target->damage(10);
        }
        setCooldown(cooldownCollision);
    }
}

Axe::Axe() {
    damage = 10;
    primaryColor = sf::Color(180, 180, 180);
    holdInFrontWhenPossible = false;
    prepareAngle = 135;
}

void Axe::useTick(int tick) {
    switch (tick) {
        case TICKS_PER_SECOND:
            std::cout << "Axe action" << std::endl;
            if (auto player = dynamic_cast<Player*>(owner)) {
                player->setItemAngle(this, -135);
            }
            break;
    }

    if (tick > TICKS_PER_SECOND / 2) {
        auto ownerBody = owner->getBodyPtr();
        float angle = ownerBody->GetAngle();
        int force = 50;
        b2Vec2 vec = b2Vec2(-force * sin(angle), force * cos(angle));
        ownerBody->ApplyLinearImpulseToCenter(vec, true);
    }
}

std::vector<b2PolygonShape> Axe::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    b2Vec2 triangle[] = {b2Vec2(0, 3), b2Vec2(-7, 8), b2Vec2(7, 8)};
    shape.Set(triangle, 3);
    shapeVec.push_back(shape);

    shape.SetAsBox(0.5, 6, b2Vec2(0, 3), 0);
    shapeVec.push_back(shape);

    return shapeVec;
}

void Axe::onContact(PhysicalObject* const other) {
    auto target = dynamic_cast<Entity*>(other);
    if (target && !isOnCooldown() && isBeingUsed()) {
        target->damage(40);
        setCooldown(cooldownCollision);
    }
}

Shield::Shield() {
    primaryColor = sf::Color(180, 180, 180);
    cooldownCollision = 1.5 * TICKS_PER_SECOND;
    cooldownAction = 0;
    actionTimeTotal = 1;
}

std::vector<b2PolygonShape> Shield::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    shape.SetAsBox(6, 1);

    shapeVec.push_back(shape);
    return shapeVec;
}

void Shield::onContact(PhysicalObject* const other) {
    if (auto item = dynamic_cast<Item*>(other)) {
        if (!isOnCooldown()) {
            item->setCooldown(item->cooldownCollision);
            setCooldown(cooldownCollision);
        }
    }
}
