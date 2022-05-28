#include "PhysicalObject.h"
#include <iostream>

b2Body* PhysicalObject::createPhysicalObject(b2World* world, float x, float y, float angleDeg) {
    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->position.Set(x, y);
    bodyDef->angle = angleDeg * DEG_TO_R;
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
    // std::cout << this << " " << fixtureDef.userData.pointer << " " << reinterpret_cast<PhysicalObject*>(fixtureDef.userData.pointer) << std::endl;
    //  std::cout << shapes.size() << std::endl;
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

// auto owner = reinterpret_cast<PhysicalObject*>(body->GetUserData());

// b2Body * PhysicalObject::createPhysicalObject(b2World* world, float x, float
// y) {

// 	auto bodyDef = new b2BodyDef();
//     bodyDef->position.Set(x, y);
//     //dynamic
// 	bodyDef->type = b2_dynamicBody;
//     //static
// 	bodyDef->type = b2_staticBody;

// 	body = world->CreateBody(bodyDef);
//     //dynamic
//     body->SetLinearDamping(DAMPING);
//     body->SetAngularDamping(DAMPING);

// 	auto fixtureDef = new b2FixtureDef();
// 	fixtureDef->shape = shape;
//     //dynamic
//     fixtureDef->density = 1.0f;
// 	fixtureDef->friction = 0.3f;
// 	this->body->CreateFixture(fixtureDef);
//     //static
// 	this->body->CreateFixture(shape, 0);

// 	view = std::make_unique<sf::ConvexShape>(*generateView(body));
// 	setColor(color);
// }

void PhysicalObject::synchronize() {
    float bodyPositionX = body->GetPosition().x * M_TO_PX;
    float bodyPositionY = body->GetPosition().y * M_TO_PX;
    float bodyRotate = getAngleDeg();

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