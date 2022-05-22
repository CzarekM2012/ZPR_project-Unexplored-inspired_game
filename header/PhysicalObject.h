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
    bool dynamic = true;    ///< If the object should move at all, params like Density or Friction don't matter if the object is static
    float density = 1.0f;   ///< Density (multiplied by fixture areas) determines object's mass
    float damping = 0.9f;   ///< How much force object should loose with time. Used to simulate friction against the floor
    float friction = 0.3f;  ///< Friction on contact with other objects, not the floor

    b2Body* body;                        ///< basic pointer, b2World takes care of bodies
    std::vector<b2PolygonShape> shapes;  ///< shapes of fixtures
    std::vector<sf::ConvexShape> views;  ///< objects displayed by the renderer
    std::vector<sf::Color> viewColors;   ///< displayed objects' colors
    sf::Color color = sf::Color::Magenta;

   public:
    PhysicalObject(){};

    b2Body* createPhysicalObject(b2World* world, float x, float y, float angle = 0);  ///< creates a box2d object and a view to render it. Params like shape or color are taken from virtual functions or set in constructors

    b2Body* getBodyPtr() const { return body; };
    void clearFixtures();
    void createOwnFixtures();  ///< Create fixtures based on this object's shapes. In case of players, item fixtures are created elsewhere
    void createFixture(const b2PolygonShape& shape, PhysicalObject* owner = nullptr);
    void generateViews();
    void synchronize();

    /// Re-generate all physical components and their graphical representations
    void reset() {
        resetShapes();
        clearFixtures();
        createOwnFixtures();
        resetColors();
        generateViews();
    }

    void resetColors() {
        viewColors.clear();
        auto baseColors = getBaseColors();
        viewColors.insert(viewColors.end(), baseColors.begin(), baseColors.end());
    }

    void resetShapes() {
        shapes = getBaseShapes();
    }

    virtual std::vector<sf::Color> getBaseColors() {
        std::vector<sf::Color> colors;
        // colors.push_back(sf::Color::Magenta);
        for (unsigned int i = 0; i < shapes.size(); ++i)  // By default set color of all fixtures to base color
            colors.push_back(color);
        return colors;
    }

    virtual std::vector<b2PolygonShape> getBaseShapes() {
        std::vector<b2PolygonShape> shapeVec;
        b2PolygonShape shape;

        shape.SetAsBox(5, 5);
        shapeVec.push_back(shape);
        return shapeVec;
    }

    std::vector<sf::ConvexShape> getViews() const { return views; };

    bool isStatic() const { return !dynamic; }
    bool isDynamic() const { return dynamic; }
    float getDensity() const { return density; }
    float getDamping() const { return damping; }
    float getFriction() const { return friction; }
    float getLength() const;

    float getAngleDeg() {
        float ang = fmod((body->GetAngle() * R_TO_DEG) - 180, 360.0f);
        return ang < 0 ? ang + 180.0f : ang - 180.0f;
    }  // Returns angle in degrees in range (-180, 180)

    void setDynamic(bool value) { dynamic = value; }
    void setDensity(float value) { density = value; }
    void setDamping(float value) { damping = value; }
    void setFriction(float value) { friction = value; }

    void setCollision(bool on) {
        auto fixtureList = body->GetFixtureList();
        b2Filter collisionFilter;  // Default values are ~group 1, collide with every group
        if (!on)
            collisionFilter.maskBits = 0;  // Don't accept collisions with anything

        for (; fixtureList != nullptr; fixtureList = fixtureList->GetNext())
            fixtureList->SetFilterData(collisionFilter);
    }
};