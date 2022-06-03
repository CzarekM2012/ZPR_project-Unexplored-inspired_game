#include "PhysicalObject.h"
#include <iostream>

b2Body* PhysicalObject::createPhysicalObject(b2World* world, float x, float y, float angleDeg) {
    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->position.Set(x, y);
    bodyDef->angle = Angle(angleDeg).get(Angle::unit::RAD);
    if (isStatic())
        bodyDef->type = b2_staticBody;
    else
        bodyDef->type = b2_dynamicBody;

    body = world->CreateBody(bodyDef);
    body->SetLinearDamping(getDamping());
    body->SetAngularDamping(getDamping());

    reset();

    return body;
}

void PhysicalObject::createOwnFixtures() {
    b2FixtureDef fixtureDef;
    fixtureDef.friction = getFriction();
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    for (b2PolygonShape shape : shapes)
        createFixture(shape);
}

void PhysicalObject::createFixture(const b2PolygonShape& shape, PhysicalObject* owner) {
    if (owner == nullptr)
        owner = this;

    b2FixtureDef fixtureDef;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(owner);

    if (owner->isStatic())
        fixtureDef.density = 0;
    else
        fixtureDef.density = owner->getDensity();
    fixtureDef.friction = owner->getFriction();

    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
}

void PhysicalObject::clearFixtures() {
    b2Fixture* lastFixture;
    if (body->GetFixtureList() == nullptr)
        return;
    for (auto fixtureList = body->GetFixtureList(); fixtureList != nullptr;) {
        lastFixture = fixtureList;
        fixtureList = fixtureList->GetNext();
        body->DestroyFixture(lastFixture);
    }
}

void PhysicalObject::synchronize() {
    float bodyPositionX = body->GetPosition().x * M_TO_PX;
    float bodyPositionY = body->GetPosition().y * M_TO_PX;
    float bodyRotate = getAngle().get();

    for (auto& view : views) {
        view.setPosition(bodyPositionX, bodyPositionY);
        view.setRotation(bodyRotate);
    }
}

void PhysicalObject::generateViews() {
    views.clear();

    auto color = viewColors.begin();
    for (b2Fixture* fixturePtr = body->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext()) {
        b2Shape* shapeBuffer = fixturePtr->GetShape();
        sf::ConvexShape bodyView;

        bodyView.setFillColor(*(color++));

        if (shapeBuffer->m_type == b2Shape::Type::e_polygon) {
            b2PolygonShape* realBodyShape = static_cast<b2PolygonShape*>(shapeBuffer);

            const int vertexCount = realBodyShape->m_count;
            bodyView.setPointCount(vertexCount);

            for (int i = 0; i < vertexCount; ++i) {
                b2Vec2 currVerts = realBodyShape->m_vertices[i];

                float posX = currVerts.x * M_TO_PX;
                float posY = currVerts.y * M_TO_PX;

                bodyView.setPoint(i, sf::Vector2f(posX, posY));
            }

            bodyView.setOrigin(0, 0);
        }
        views.push_back(bodyView);
    }
}

void PhysicalObject::reset() {
    resetShapes();
    clearFixtures();
    createOwnFixtures();
    resetColors();
    generateViews();
}

void PhysicalObject::destroyBody() {  ///< Cannot be triggered from collision callbacks to avoid collision with box2d step()
    body->GetWorld()->DestroyBody(body);
    views.clear();
}

float PhysicalObject::getLength() const {
    float max = 0, min = 0;
    for (auto shape : shapes) {
        for (int i = 0; i < shape.m_count; ++i) {
            float value = shape.m_vertices[i].y;
            if (value > max)
                max = value;

            if (value < min)
                min = value;
        }
    }
    return max - min;
}

void PhysicalObject::setPrimaryColor(sf::Color color) {
    primaryColor = color;
    resetColors();
}

void PhysicalObject::resetColors() {
    viewColors.clear();
    auto baseColors = getBaseColors();
    viewColors.insert(viewColors.end(), baseColors.begin(), baseColors.end());
}

std::vector<sf::Color> PhysicalObject::getBaseColors() const {
    std::vector<sf::Color> colors;
    // colors.push_back(sf::Color::Magenta);
    for (unsigned int i = 0; i < shapes.size(); ++i)  // By default set primaryColor of all fixtures to base primaryColor
        colors.push_back(primaryColor);
    return colors;
}

std::vector<b2PolygonShape> PhysicalObject::getBaseShapes() const {
    std::vector<b2PolygonShape> shapeVec;
    b2PolygonShape shape;

    shape.SetAsBox(5, 5);
    shapeVec.push_back(shape);
    return shapeVec;
}

void PhysicalObject::setCollision(bool on) {
    auto fixtureList = body->GetFixtureList();
    b2Filter collisionFilter;  // Default values are ~group 0, collide with every group
    if (on) {
        if (collisionGroup != 0) {
            collisionFilter.groupIndex = -collisionGroup;  // Negative groupIndex means that objects from this group should not collide with each other
        }
    } else
        collisionFilter.maskBits = 0;  // Don't accept collisions with anything

    for (; fixtureList != nullptr; fixtureList = fixtureList->GetNext())
        fixtureList->SetFilterData(collisionFilter);
}

void PhysicalObject::setCollisionGroup(int value) {
    collisionGroup = value;
    setCollision(true);
}