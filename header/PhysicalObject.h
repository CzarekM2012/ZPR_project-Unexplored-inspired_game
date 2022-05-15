#pragma once

#include <box2d/box2d.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <memory>

/// An object in game world paired with Box2D body. Base of the class hierarchy
class PhysicalObject {
   protected:
    const int M_TO_PX = 5;  // box2d meter to SFML pixels size convertion ratio
    const float PX_TO_M = 1.0 / M_TO_PX;

    const float R_TO_DEG = 180 / b2_pi;  // Radians
    const float DEG_TO_R = b2_pi / 180;

    // Body params
    bool dynamic =
        true;  // If the object should move at all, params like Density or
               // Friction don't matter if the object is static
    float density = 1.0f;  // Density determines object's mass
    float damping = 0.9f;  // How much force object should loose with time. Used
                           // to simulate friction against the floor
    float friction =
        0.3f;  // Friction on contact with other objects, not the floor
    sf::Color color = sf::Color::Magenta;

    b2Body* body;  // box2d requires use of basic pointers, b2World takes care
                   // of bodies
    std::unique_ptr<sf::ConvexShape> view;

   public:
    PhysicalObject(){};

    b2Body* createPhysicalObject(
        b2World* world,
        float x,
        float y,
        float angle =
            0);  // creates a box2d object and a view to render it, has to
                 // provide correct shape and parameters like mass or color

    b2Body* getBodyPtr() const { return body; };
    sf::ConvexShape getView() const { return *view; };
    sf::ConvexShape* generateView(b2Body* body) const;
    void synchronize();

    void setColor(sf::Color color) {
        this->color = color;
        view->setFillColor(color);
    };

    virtual b2Shape* getShape() const {
        b2PolygonShape* shape = new b2PolygonShape();
        shape->SetAsBox(5, 5);
        return shape;
    };
    bool isStatic() const { return !dynamic; };
    bool isDynamic() const { return dynamic; };
    float getDensity() const { return density; };
    float getDamping() const { return damping; };
    float getFriction() const { return friction; };

    float getAngleDeg() {
        float ang = fmod((body->GetAngle() * R_TO_DEG) - 180, 360.0f);
        return ang < 0 ? ang + 180.0f : ang - 180.0f;
    };  // Returns angle in degrees in range (-180, 180)

    void setDynamic(bool value) { dynamic = value; };
    void setDensity(float value) { density = value; };
    void setDamping(float value) { damping = value; };
    void setFriction(float value) { friction = value; };

    void setCollision(bool on) {
        auto fixtureList = body->GetFixtureList();
        b2Filter collisionFilter;  // Default values are ~group 1, collide with
                                   // every group
        if (!on)
            collisionFilter.maskBits =
                0;  // Don't accept collisions with anything

        for (; fixtureList != nullptr; fixtureList = fixtureList->GetNext())
            fixtureList->SetFilterData(collisionFilter);
    }
};
