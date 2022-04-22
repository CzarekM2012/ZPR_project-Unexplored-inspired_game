#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include "PhysicalObject.h"

class State {

public:        
    std::vector<std::shared_ptr<PhysicalObject>> objects;

    State() {};

    void add(std::shared_ptr<PhysicalObject> object) { objects.push_back(object); };

    std::shared_ptr<PhysicalObject> get(unsigned int index) const {return objects.size() > index ? objects[index] : nullptr;}  // TODO: Rewrite as an overloaded operator []
    int getObjectCount() const {return objects.size();};
};
