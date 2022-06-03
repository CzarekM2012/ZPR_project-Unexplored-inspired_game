#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

#include <box2d/box2d.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <memory>
#include "Angle.h"

#define UNUSED(x) (void)(x)  ///< For now to disable "unused parameter" error

/// An object in game world paired with Box2D body. Base of the class hierarchy
class PhysicalObject {
   protected:
    const int M_TO_PX = 5;  // box2d meter to SFML pixels size convertion ratio
    const float PX_TO_M = 1.0 / M_TO_PX;

    // Body params
    bool dynamic = true;     ///< If the object should move at all, params like Density or Friction don't matter if the object is static
    float density = 1.0f;    ///< Density (multiplied by fixture areas) determines object's mass
    float damping = 0.9f;    ///< How much force object should loose with time. Used to simulate friction against the floor
    float friction = 0.3f;   ///< Friction on contact with other objects, not the floor
    int collisionGroup = 0;  ///< Collisions are set to occur between everything that does not belong to the same group and between every object in group 0

    b2Body* body;                        ///< basic pointer, b2World takes care of bodies
    std::vector<b2PolygonShape> shapes;  ///< shapes of fixtures
    std::vector<sf::ConvexShape> views;  ///< objects displayed by the renderer
    std::vector<sf::Color> viewColors;   ///< displayed objects' colors
    sf::Color primaryColor = sf::Color::Magenta;

   public:
    bool toDestroy = false;  ///< should be destroyed by GameController

    PhysicalObject(){};
    virtual ~PhysicalObject(){};

    b2Body* createPhysicalObject(b2World* world, float x, float y, float angle = 0);  ///< creates a box2d object and a view to render it. Params like shape or primaryColor are taken from virtual functions or set in constructors

    b2Body* getBodyPtr() const { return body; };
    void clearFixtures();
    void createOwnFixtures();  ///< Create fixtures based on this object's shapes. In case of players, item fixtures are created elsewhere
    void createFixture(const b2PolygonShape& shape, PhysicalObject* owner = nullptr);
    void generateViews();
    virtual void synchronize();

    /// Re-generate all physical components and their graphical representations
    void reset() {
        resetShapes();
        clearFixtures();
        createOwnFixtures();
        resetColors();
        generateViews();
    }

    /// Cannot be triggered from collision callbacks to avoid collision with box2d step()
    void destroyBody() {
        body->GetWorld()->DestroyBody(body);
        views.clear();
    }

    /// Sets primary primaryColor and resets object colors. Base colors should be generated based on primary primaryColor
    void setPrimaryColor(sf::Color color) {
        primaryColor = color;
        resetColors();
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
        for (unsigned int i = 0; i < shapes.size(); ++i)  // By default set primaryColor of all fixtures to base primaryColor
            colors.push_back(primaryColor);
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

    Angle getAngle() const {
        return Angle(body->GetAngle(), Angle::unit::RAD);
    }

    void setDynamic(bool value) { dynamic = value; }
    void setDensity(float value) { density = value; }
    void setDamping(float value) { damping = value; }
    void setFriction(float value) { friction = value; }
    void setCollisionGroup(int value) {
        collisionGroup = value;
        setCollision(true);
    }

    void setCollision(bool on) {
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

    virtual void onContact(PhysicalObject* const other) {
        UNUSED(other);
    }
};

#endif  // PHYSICAL_OBJECT_H