#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

#include <math.h>
#include <memory>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

#include "Angle.h"
#include "Constants.h"

/// An object in game world paired with Box2D body. Base of the class hierarchy
class PhysicalObject {
    static b2BodyDef bodyDef;

   protected:
    // Body params
    bool dynamic = true;     ///< If the object should move at all, params like Density or Friction don't matter if the object is static
    float density = 1.0f;    ///< Density (multiplied by fixture areas) determines object's mass
    float damping = 0.9f;    ///< How much force object should loose with time. Used to simulate friction against the floor
    float friction = 0.3f;   ///< Friction on contact with other objects, not the floor
    int collisionGroup = 0;  ///< Collisions are set to occur between everything that does not belong to the same group and between every object in group 0

    b2Body* body;                        ///< Basic pointer, b2World takes care of bodies
    std::vector<b2PolygonShape> shapes;  ///< Shapes of fixtures
    std::vector<sf::ConvexShape> views;  ///< Objects displayed by the renderer
    std::vector<sf::Color> viewColors;   ///< Displayed objects' colors
    sf::Color primaryColor = sf::Color::Magenta;

   public:
    bool toDestroy = false;  ///< Delayed destruction by GameController. Object should not destroy its body while physics step() is running, e.g. in collision callbacks

    PhysicalObject(){};
    virtual ~PhysicalObject(){};

    b2Body* createPhysicalObject(b2World* world, b2Vec2 position, Angle angle = Angle());  ///< creates a box2d object and a view to render it. Params like shape or primaryColor are taken from virtual functions or set in constructors
    b2Body* getBodyPtr() const { return body; }

    void clearFixtures();
    void createOwnFixtures();  ///< Create fixtures based on this object's shapes. In case of players, item fixtures are created elsewhere
    void createFixture(const b2PolygonShape& shape, PhysicalObject* owner = nullptr);
    void generateViews();        ///< Create rendered view objects for each body fixture
    virtual void synchronize();  ///< Synchronize rendered view position and rotation with physical object representation

    void reset();        ///< Re-generate all physical components and their graphical representations
    void destroyBody();  ///< Cannot be triggered from collision callbacks to avoid collision with box2d step()

    void setPrimaryColor(sf::Color color);  ///< Set primary primaryColor and reset object colors. Base colors should be generated based on primary primaryColor
    void resetColors();                     ///< Re-generate view colors from getBaseColors()
    void resetShapes() { shapes = getBaseShapes(); }

    virtual std::vector<sf::Color> getBaseColors() const;       ///< Overload to assign custom colors to shapes. By default all are set to primaryColor
    virtual std::vector<b2PolygonShape> getBaseShapes() const;  ///< Overload to set object shape in derived class
    std::vector<sf::ConvexShape> getViews() const { return views; };

    bool isStatic() const { return !dynamic; }
    bool isDynamic() const { return dynamic; }
    float getDensity() const { return density; }
    float getDamping() const { return damping; }
    float getFriction() const { return friction; }
    float getLength() const;
    Angle getAngle() const { return Angle(body->GetAngle(), Angle::unit::RAD); }

    void setDynamic(bool value) { dynamic = value; }
    void setDensity(float value) { density = value; }
    void setDamping(float value) { damping = value; }
    void setFriction(float value) { friction = value; }
    void setCollisionGroup(int value);  ///< Set group to determine with wich objects should this one collide. 0 means collision with everything, any other groups' members don't collide with one another
    void setCollision(bool on);         ///< Toggle collision on or off

    virtual void onContact(PhysicalObject* const /*other*/) {}
};

#endif  // PHYSICAL_OBJECT_H