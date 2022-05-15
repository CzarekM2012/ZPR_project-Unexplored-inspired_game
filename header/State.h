#pragma once

#include <algorithm>
#include <memory>
#include <vector>


#include "PhysicalObject.h"

/// A simple vector of PhysicalObject. Represents whole state of the game
class State {
   public:
    std::vector<std::shared_ptr<PhysicalObject>> objects;

    State(){};

    std::shared_ptr<PhysicalObject> add(
        std::shared_ptr<PhysicalObject> object) {
        objects.push_back(object);
        return objects[objects.size() - 1];
    };

    std::shared_ptr<PhysicalObject> get(unsigned int index) const {
        return objects.size() > index ? objects[index] : nullptr;
    }  // TODO: Rewrite as an overloaded operator []
    std::shared_ptr<PhysicalObject> getLast() {
        return objects.empty() ? nullptr : objects[objects.size() - 1];
    };
    int getObjectCount() const { return objects.size(); };
};
