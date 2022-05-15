#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <memory>

#define UNUSED(x) (void)(x)  // For now to disable "unused parameter" error

/// A typical object with health pool. Most can be destroyed
class Entity : public PhysicalObject {
   protected:
    bool invulnerable = false;
    int hp = 1;

   public:
    bool isInvulnerable() const { return invulnerable; };

    void damage(int value) {
        hp = hp - value;
        if (hp < 0) {
            hp = 0;
            body->DestroyFixture(body->GetFixtureList());
        }
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

    b2Shape* getShape() const {
        b2PolygonShape* shape = new b2PolygonShape();
        shape->SetAsBox(width / 2, height / 2);
        return shape;
    }
};

/// A simple object to test physics
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

    Box(int w, int h)
        : Box() {
        width = w;
        height = h;
    }
};

/// Immovable and indestructibe obstacle
class Wall : public RectangleObject {
   public:
    Wall() {
        hp = invulnerable;
        dynamic = false;
        color = sf::Color::Black;
    }

    Wall(int w, int h)
        : Wall() {
        width = w;
        height = h;
    }
};

/// Items can be equiped by players. They don't collide until equipped
class Item : public PhysicalObject {
   private:
    std::weak_ptr<Entity> owner;

   public:
    Item() {
        density = 0.001;  // For now, not to encumber the player too much
    };
    std::weak_ptr<Entity> getOwner() { return owner; };
    void setOwner(std::shared_ptr<Entity> newOwner) { owner = newOwner; };
};

/// Weapons can be used to deal damage to entities
class Weapon : public Item {
   protected:
    int damage;
};

/// A basic weapon to test game mechanics
class Sword : public Weapon {
   public:
    Sword() {
        damage = 10;
        color = sf::Color(180, 180, 180);
    };

    b2Shape* getShape() const {
        b2PolygonShape* shape = new b2PolygonShape();
        b2Vec2 triangle[] = {b2Vec2(-2, -2), b2Vec2(0, 3), b2Vec2(2, -2)};
        shape->Set(triangle, 3);
        return shape;
    }
};

/// A basic shield to test game mechanics
class Shield : public Item {
   protected:
    int defense;

   public:
    Shield() {
        defense = 7;
        color = sf::Color(180, 180, 180);
    }

    b2Shape* getShape() const {
        b2PolygonShape* shape = new b2PolygonShape();
        shape->SetAsBox(14, 1);
        return shape;
    }
};

/// Controlled by the player. Most complex than most entities
class Player : public Entity {
    std::shared_ptr<Item> item_lh;

   public:
    Player() {
        hp = 100;
        color = sf::Color::Green;
    }

    void equipLeftHand(std::shared_ptr<Item> item) {
        item_lh = item;
        updateEquipment();
    }

    void updateEquipment() {
        this->body->CreateFixture(item_lh->getShape(), item_lh->getDensity());
        this->generateViews();
    }

    b2Shape* getShape() const {
        b2PolygonShape* shape = new b2PolygonShape();
        b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)};
        shape->Set(pentagon, 5);
        return shape;
    }
};

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
