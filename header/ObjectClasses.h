#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <memory>

#define UNUSED(x) (void)(x) //For now to disable "unused parameter" error

class Entity : public PhysicalObject {
protected:
    const int HP_INF = 1000000000;
    int hp;

public:
    bool isInvulnerable() const { return hp == HP_INF; };
};

class Player : public Entity {
public:
    Player() {
        hp = 100;
        color = sf::Color::Green;
    }


    b2Shape * getShape() {
        b2PolygonShape * shape = new b2PolygonShape();
        b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)}; 
        shape->Set(pentagon, 5);
        return shape;
    }
};

class RectangleObject : public Entity {
protected:
    float width;
    float height;

public:
    void setSize(float width, float height) {
        this->width = width;
        this->height = height;
    }

    b2Shape * getShape() {
        b2PolygonShape * shape = new b2PolygonShape();
        shape->SetAsBox(width/2, height/2);
        return shape;
    }
};

class Box : public RectangleObject {
public:
    Box() {
        hp = 100;
        width = 10;
        height = 10;
        density = 0.1f;
        damping = 0.1f;
        color = sf::Color::Yellow;
    }

    Box(int w, int h) : Box() {
        width = w;
        height = h;
    }
    
};

class Wall : public RectangleObject {
public:
    Wall() {
        hp = HP_INF;
        dynamic = false;
        color = sf::Color::Black;
    }

    Wall(int w, int h) : Wall() {
        width = w;
        height = h;
    }
};

// class Item : public PhysicalObject {
// private:
//     std::weak_ptr<Entity> owner;
// };

// class Weapon : public Item {
// private:
//     int damage;
// };

// class Armor : public Item {
// private:
//     int armor;
// };

// class Example : public Other {
// public:
//     virtual b2Body * createPhysicalObject(b2World* world, float x, float y) {	
        
// 	    auto bodyDef = new b2BodyDef();
//         bodyDef->position.Set(x, y);
//         //dynamic
// 	    bodyDef->type = b2_dynamicBody;
//         //static
// 	    bodyDef->type = b2_staticBody;

// 	    body = world->CreateBody(bodyDef);
//         //dynamic
//         body->SetLinearDamping(DAMPING);
//         body->SetAngularDamping(DAMPING);

// 	    auto fixtureDef = new b2FixtureDef();
// 	    fixtureDef->shape = shape;
//         //dynamic
//         fixtureDef->density = 1.0f;
// 	    fixtureDef->friction = 0.3f;
// 	    this->body->CreateFixture(fixtureDef);
//         //static
// 	    this->body->CreateFixture(shape, 0); 
    
// 	    view = std::make_unique<sf::ConvexShape>(*generateView(body));
// 	    setColor(color);
//     };
// };

