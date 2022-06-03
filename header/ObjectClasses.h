#ifndef OBJECT_CLASSES_H
#define OBJECT_CLASSES_H

#include <iostream>
#include <memory>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "Item.h"
#include "PhysicalObject.h"
#include "Player.h"

#define UNUSED(x) (void)(x)  // For now to disable "unused parameter" error

/// An object with a shape of a rectangle, has adjustible width and height
class RectangleObject : public Entity {
   protected:
    float width;
    float height;

   public:
    /// Adjusts width and height, should be called before createPhysicalObject()
    void setSize(float width, float height);
    virtual std::vector<b2PolygonShape> getBaseShapes() const;
};

/// A simple, destructible object to push around
class Box : public RectangleObject {
   public:
    Box();
    Box(int w, int h);
};

/// Immovable and indestructibe obstacle
class Wall : public RectangleObject {
   public:
    Wall();
    Wall(int w, int h);
};

/// Weapons can be used to deal damage to entities
class Weapon : public Item {
   protected:
    int damage;
};

/// Sword is held automatically in front. Player can stab on contact and charge on use.
class Sword : public Weapon {
   public:
    Sword();

    void useTick(int tick);  ///< Bring sword in front of the player and charge forward
    virtual std::vector<b2PolygonShape> getBaseShapes() const;
    void onContact(PhysicalObject* const other);  ///< Deal damage on contact, more when actively used
};

/// Axe can be swinged on use. Deals severe damage but only when swinging.
class Axe : public Weapon {
   public:
    Axe();
    void useTick(int tick);
    virtual std::vector<b2PolygonShape> getBaseShapes() const;
    void onContact(PhysicalObject* const other);
};

/// Defensive item, puts colliding weapons on cooldown
class Shield : public Item {
   protected:
    int defense;

   public:
    Shield();

    virtual std::vector<b2PolygonShape> getBaseShapes() const;
    void onContact(PhysicalObject* const other);  ///< If shield colliding with another item, set that item on cooldown
};

#endif  // OBJECT_CLASSES_H