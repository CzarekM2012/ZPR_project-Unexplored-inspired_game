#include "State.h"

PhysicalObject* State::add(std::unique_ptr<PhysicalObject> objectPtr) {
    objects.push_back(std::move(objectPtr));
    return objects[objects.size() - 1].get();
}

PhysicalObject* State::add(PhysicalObject* objectPtr) {
    objects.push_back(std::unique_ptr<PhysicalObject>(objectPtr));
    return objects[objects.size() - 1].get();
}

void State::remove(PhysicalObject* objectPtr) {
    const auto it = find_if(objects.begin(), objects.end(), [&](const std::unique_ptr<PhysicalObject>& object) { return object.get() == objectPtr; });
    if (it != objects.end()) {
        it->reset();
        objects.erase(it);
    }
}

PhysicalObject* State::get(unsigned int index) const {
    return objects.size() > index ? objects[index].get() : nullptr;
}
PhysicalObject* State::getLast() const {
    return objects.empty() ? nullptr : objects[objects.size() - 1].get();
}

int State::getObjectCount() const {
    return objects.size();
}
