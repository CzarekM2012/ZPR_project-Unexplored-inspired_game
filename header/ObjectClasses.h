#ifndef OBJECT_CLASSES_H
#define OBJECT_CLASSES_H

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

#include "Entity.h"
#include "PhysicalObject.h"
#include "Player.h"

#define UNUSED(x) (void)(x)  // For now to disable "unused parameter" error

const int TICKS_PER_SECOND = 100;  // TODO: connect with GameController::TIME_STEP

class RectangleObject : public Entity {
   protected:
    float width;
    float height;

   public:
    void setSize(float width, float height) {
        this->width = width;
        this->height = height;
    }

    virtual std::vector<b2PolygonShape> getBaseShapes() {
        std::vector<b2PolygonShape> shapeVec;
        b2PolygonShape shape;

        shape.SetAsBox(width / 2, height / 2);
        shapeVec.push_back(shape);
        return shapeVec;
    }
};

/// A simple object to test physics
class Box : public RectangleObject {
   public:
    Box() {
        // maxHp = 100;
        // hp = maxHp;
        width = 10;
        height = 10;
        density = 0.1f;
        damping = 0.1f;
        primaryColor = sf::Color::Yellow;
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
        invulnerable = true;
        dynamic = false;
        primaryColor = sf::Color::Black;
    }

    Wall(int w, int h)
        : Wall() {
        width = w;
        height = h;
    }
};

/// Items can be equiped by players. They don't collide until equipped
class Item : public PhysicalObject {
   protected:
    Entity* owner = nullptr;
    int cooldown = 0;        ///< In ticks. While positive, disables collisions and use of item's abilities
    int actionTimeLeft = 0;  ///< In ticks. Positive means that action is being used and will continue for that many ticks

    virtual void useTick(int tick) { UNUSED(tick); }  ///< Can be overloaded to set behaviour each tick

   public:
    float cooldownCollision = 1 * TICKS_PER_SECOND;
    float cooldownAction = 2 * TICKS_PER_SECOND;
    float actionTimeTotal = 1 * TICKS_PER_SECOND;
    float prepareAngle = 0;
    bool holdInFrontWhenPossible = false;

    Item() {
        density = 0.001;  // For now, not to encumber the player too much. Cannot be set to 0, it's reserved for static objects
    };
    Entity* getOwner() { return owner; };
    void setOwner(Entity* newOwner) { owner = newOwner; };

    bool isOnCooldown() const { return cooldown > 0; }
    bool isBeingUsed() const { return getActionTimeLeft() > 0; }
    bool canBeUsed() const { return !isOnCooldown() && !isBeingUsed(); }

    void useTrigger() {  ///< Triggers action. By itself only sets action timer. useTick() takes care of rest
        if (canBeUsed()) {
            actionTimeLeft = getActionTimeTotal();
        }
    };
    void useTick() {  ///< Continuous use, called each tick from GameController
        if (isBeingUsed())
            useTick(actionTimeLeft);

        if (actionTimeLeft-- == 1) {
            setCooldown(cooldownAction);
            if (auto player = dynamic_cast<Player*>(owner)) {
                player->resetItemAngle(this);
            }
        }
    };
    virtual float getActionTimeTotal() const { return actionTimeTotal; }
    float getActionTimeLeft() const { return actionTimeLeft; };

    void setCooldown(int ticks) {
        if (isOnCooldown()) {
            cooldown = std::max(cooldown, ticks);
            return;
        }
        cooldown = ticks;
        // for_each(views.begin(), views.end(), [&](sf::ConvexShape& shape) { shape.setFillColor(sf::Color::Black); });
        setCollision(false);
    }

    void lowerCooldown(int ticks) {
        cooldown -= ticks;
        if (cooldown < 0) {
            cooldown = 0;
            removeCooldownEffects();
        }
    }

    void resetCooldown() {
        cooldown = 0;
        removeCooldownEffects();
    }

    void removeCooldownEffects() {
        if (owner != nullptr)  // Otherwise could deal damage when dropped
            setCollision(true);
        // resetColors();
        // generateViews();
    }

    void synchronize() {
        float bodyPositionX = body->GetPosition().x * M_TO_PX;
        float bodyPositionY = body->GetPosition().y * M_TO_PX;
        float bodyRotate = getAngle().get();

        auto colors = getBaseColors();
        for (unsigned int i = 0; i < views.size(); ++i) {
            auto& view = views[i];
            auto color = colors[i];

            view.setPosition(bodyPositionX, bodyPositionY);
            view.setRotation(bodyRotate);

            if (isOnCooldown())
                view.setFillColor(sf::Color::Black);
            else
                view.setFillColor(color);
        }
    }
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
        primaryColor = sf::Color(180, 180, 180);
        holdInFrontWhenPossible = true;
    };

   public:
    void useTick(int tick) {
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
        // ownerBody->SetLinearDamping(0);
    }

    virtual std::vector<b2PolygonShape> getBaseShapes() {
        std::vector<b2PolygonShape> shapeVec;
        b2PolygonShape shape;

        b2Vec2 triangle[] = {b2Vec2(-3, -1), b2Vec2(0, 8), b2Vec2(3, -1)};
        shape.Set(triangle, 3);
        shapeVec.push_back(shape);

        shape.SetAsBox(0.5, 2, b2Vec2(0, -2), 0);
        shapeVec.push_back(shape);

        return shapeVec;
    }

    void onContact(PhysicalObject* const other) {
        auto target = dynamic_cast<Entity*>(other);
        if (target) {
            if (!isOnCooldown()) {
                if (isBeingUsed())
                    target->damage(20);
                else
                    target->damage(10);
            }
            setCooldown(cooldownCollision);
            // std::cout << "Current HP: " << target->getHp() << "/" << target->getMaxHp() << std::endl;
        }
    }
};

class Axe : public Weapon {
   public:
    Axe() {
        damage = 10;
        primaryColor = sf::Color(180, 180, 180);
        holdInFrontWhenPossible = false;
        prepareAngle = 135;
    };

   public:
    void useTick(int tick) {
        switch (tick) {
            case 1:

                // case TICKS_PER_SECOND:
                //     if (auto player = dynamic_cast<Player*>(owner)) {
                //         player->setItemAngle(this, 90);
                //     }
                //     break;

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
        // ownerBody->SetLinearDamping(0);
    }

    virtual std::vector<b2PolygonShape> getBaseShapes() {
        std::vector<b2PolygonShape> shapeVec;
        b2PolygonShape shape;

        b2Vec2 triangle[] = {b2Vec2(0, 3), b2Vec2(-7, 8), b2Vec2(7, 8)};
        shape.Set(triangle, 3);
        shapeVec.push_back(shape);

        shape.SetAsBox(0.5, 6, b2Vec2(0, 3), 0);
        shapeVec.push_back(shape);

        return shapeVec;
    }

    void onContact(PhysicalObject* const other) {
        auto target = dynamic_cast<Entity*>(other);
        if (target) {
            if (!isOnCooldown()) {
                if (isBeingUsed()) {
                    target->damage(40);
                    setCooldown(cooldownCollision);
                }
            }
            // std::cout << "Current HP: " << target->getHp() << "/" << target->getMaxHp() << std::endl;
        }
    }
};

/// A basic shield to test game mechanics
class Shield : public Item {
   protected:
    int defense;

   public:
    Shield() {
        defense = 7;
        primaryColor = sf::Color(180, 180, 180);
        cooldownAction = 0;
        actionTimeTotal = 1;
    }

    // void useTick(int tick) {
    //     switch (tick) {
    //         case TICKS_PER_SECOND:
    //             std::cout << "Shield action" << std::endl;
    //             if (auto player = dynamic_cast<Player*>(owner)) {
    //                 player->getBodyPtr()->SetLinearDamping(0.99);  // TODO: Replace with a slowdown that works, maybe for all items while a button is held
    //             }
    //             break;
    //         case 1:
    //             if (auto player = dynamic_cast<Player*>(owner)) {
    //                 player->getBodyPtr()->SetLinearDamping(0.9);
    //             }
    //             break;
    //     }
    // }

    virtual std::vector<b2PolygonShape> getBaseShapes() {
        std::vector<b2PolygonShape> shapeVec;
        b2PolygonShape shape;

        shape.SetAsBox(6, 1);

        shapeVec.push_back(shape);
        return shapeVec;
    }

    void onContact(PhysicalObject* const other) {
        if (auto item = dynamic_cast<Item*>(other)) {
            if (!isOnCooldown()) {
                item->setCooldown(item->cooldownCollision);
                setCooldown(cooldownCollision);
            }
            // std::cout << "Current HP: " << target->getHp() << "/" << target->getMaxHp() << std::endl;
        }
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

#endif  // OBJECT_CLASSES_H