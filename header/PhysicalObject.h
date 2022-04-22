#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <memory>

class PhysicalObject {
private:
    const int M_TO_PX = 10; // Each box2d meter = 50 SFML pixels
    const float PX_TO_M = 1.0 / M_TO_PX;

    const float R_TO_PX = 180 / b2_pi; // Radians
    const float PX_TO_R = b2_pi / 180;

    const float DAMPING = 0.9; // How much force object should loose with time. Used to simulate friction

    sf::Color color = sf::Color::Magenta;
    b2Body * body; // box2d requires use of basic pointers, b2World takes care of bodies
    std::unique_ptr<sf::ConvexShape> view;

public:

    PhysicalObject(b2World* world, float x, float y, bool dynamic, b2Shape * shape, sf::Color color);

    b2Body * getBodyPtr() const {return body;};
    sf::ConvexShape getView() const {return *view;};
    sf::ConvexShape* generateView(b2Body* body) const;
    void synchronize();

    void setColor(sf::Color color) { this->color = color; view->setFillColor(color); };

};
