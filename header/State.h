#ifndef STATE_H
#define STATE_H

#include <algorithm>
#include <iostream>
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

    PhysicalObject* add(std::unique_ptr<PhysicalObject> objectPtr) {
        objects.push_back(std::move(objectPtr));
        return objects[objects.size() - 1].get();
    };

    PhysicalObject* add(PhysicalObject* objectPtr) {
        objects.push_back(std::unique_ptr<PhysicalObject>(objectPtr));
        return objects[objects.size() - 1].get();
    };

    void remove(PhysicalObject* objectPtr) {
        auto it = find_if(objects.begin(), objects.end(), [&](std::unique_ptr<PhysicalObject>& object) { return object.get() == objectPtr; });
        if (it != objects.end()) {
            it->reset();
            objects.erase(it);
        }
    }

    PhysicalObject* get(unsigned int index) const {
        return objects.size() > index ? objects[index].get() : nullptr;
    }
    PhysicalObject* getLast() {
        return objects.empty() ? nullptr : objects[objects.size() - 1].get();
    };
    int getObjectCount() const { return objects.size(); };
};

#endif  // STATE_H