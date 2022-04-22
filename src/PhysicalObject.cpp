#include "../header/PhysicalObject.h"
#include <iostream>
PhysicalObject::PhysicalObject(b2World* world, float x, float y, bool dynamic, b2Shape * shape, sf::Color color) {	
    
	auto bodyDef = new b2BodyDef();
	if (dynamic) {
		bodyDef->type = b2_dynamicBody;
	}
	else {
		bodyDef->type = b2_staticBody;
	}
	bodyDef->position.Set(x, y);

	body = world->CreateBody(bodyDef);
    if (dynamic) {
        body->SetLinearDamping(DAMPING);
        body->SetAngularDamping(DAMPING);
    }
	auto fixtureDef = new b2FixtureDef();
	fixtureDef->shape = shape;

	if (dynamic) {
		fixtureDef->density = 1.0f;
		fixtureDef->friction = 0.3f;
		this->body->CreateFixture(fixtureDef);
	}
	else {
		this->body->CreateFixture(shape, 0);
	}

	view = std::make_unique<sf::ConvexShape>(*generateView(body));
	setColor(color);
}

void PhysicalObject::synchronize() {
    float bodyPositionX = body->GetPosition().x * M_TO_PX;
    float bodyPositionY = body->GetPosition().y * M_TO_PX;
    float bodyRotate = body->GetAngle() * R_TO_PX;

    view->setPosition(bodyPositionX, bodyPositionY);
    view->setRotation(bodyRotate);
}

sf::ConvexShape* PhysicalObject::generateView(b2Body* body) const {

    sf::ConvexShape* bodyView = new sf::ConvexShape();
    bodyView->setFillColor(color);

    for (b2Fixture* fixturePtr = body->GetFixtureList();
            fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
    {

        b2Shape* shapeBuffer = fixturePtr->GetShape();
        if(shapeBuffer->m_type == b2Shape::Type::e_polygon)
        {
            b2PolygonShape* realBodyShape = static_cast<b2PolygonShape*>(shapeBuffer);

            const int vertexCount = realBodyShape->m_count;
            bodyView->setPointCount(vertexCount);

            for (int i = 0; i < vertexCount; ++i)
            {
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