#ifndef ENTITY_H
#define ENTITY_H

#include "PhysicalObject.h"

/// A typical object with health pool. Most can be destroyed
class Entity : public PhysicalObject {
   protected:
    bool invulnerable = false;
    int maxHp = 100;
    int hp = 100;

   public:
    bool isInvulnerable() const { return invulnerable; }

    int getHp() { return hp; }
    int getMaxHp() { return maxHp; }

    void damage(int value);  ///< Deal damage, set toDestroy when hp fall to 0

    void synchronize();  ///< Custom synchronization function to display hp as color intensity of the first polygon
};
#endif  // ENTITY_H