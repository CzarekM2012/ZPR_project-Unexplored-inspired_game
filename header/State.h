#ifndef STATE_H
#define STATE_H

#include <algorithm>

#include <memory>
#include <vector>

#include "PhysicalObject.h"

/// A simple vector of PhysicalObject unique pointers. Represents whole state of the game
class State {
   public:
    std::vector<std::unique_ptr<PhysicalObject>> objects;

    State(){};
    State(const State& state) = delete;
    State& operator=(const State&) = delete;

    PhysicalObject* add(std::unique_ptr<PhysicalObject> objectPtr);
    PhysicalObject* add(PhysicalObject* objectPtr);
    void remove(PhysicalObject* objectPtr);

    PhysicalObject* get(unsigned int index) const;
    PhysicalObject* getLast() const;
    int getObjectCount() const;
};

#endif  // STATE_H