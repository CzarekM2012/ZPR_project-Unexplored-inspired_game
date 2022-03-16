#pragma once

#include <vector>

#include "PhysicalObject.h"

class State {
    std::vector<PhysicalObject*> objects;

    public:             
    State() {};
    // TODO: Check for possible memory leaks, maybe switch to unique_ptr or sth similar

    void add(PhysicalObject * object) {objects.push_back(object);};

    PhysicalObject* get(int index) {return objects.size() > index ? objects[index] : nullptr;}  // TODO: Rewrite as an overloaded operator []
    //std::vector<PhysicalObject*> getObjects() {return objects;}
    int getObjectCount() {return objects.size();};
};
