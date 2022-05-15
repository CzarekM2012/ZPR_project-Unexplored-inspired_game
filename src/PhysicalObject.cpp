#include "../header/PhysicalObject.h"
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

    b2Shape* shape = getShape();
    if (isStatic())
        this->body->CreateFixture(shape, 0);
    else {
        b2FixtureDef* fixtureDef = new b2FixtureDef();
        fixtureDef->shape = shape;
        fixtureDef->density = getDensity();
        fixtureDef->friction = getFriction();
        this->body->CreateFixture(fixtureDef);
    }

    view = std::make_unique<sf::ConvexShape>(*generateView(body));
    return body;
}

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

    view->setPosition(bodyPositionX, bodyPositionY);
    view->setRotation(bodyRotate);
}

sf::ConvexShape* PhysicalObject::generateView(b2Body* body) const {
    sf::ConvexShape* bodyView = new sf::ConvexShape();
    bodyView->setFillColor(color);

    for (b2Fixture* fixturePtr = body->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext()) {
        b2Shape* shapeBuffer = fixturePtr->GetShape();
        if (shapeBuffer->m_type == b2Shape::Type::e_polygon) {
            b2PolygonShape* realBodyShape = static_cast<b2PolygonShape*>(shapeBuffer);

            const int vertexCount = realBodyShape->m_count;
            bodyView->setPointCount(vertexCount);

            for (int i = 0; i < vertexCount; ++i) {
                b2Vec2 currVerts = realBodyShape->m_vertices[i];

                float posX = currVerts.x * M_TO_PX;
                float posY = currVerts.y * M_TO_PX;

                bodyView->setPoint(i, sf::Vector2f(posX, posY));
            }

            bodyView->setOrigin(0, 0);
        }
    }

    return bodyView;
}