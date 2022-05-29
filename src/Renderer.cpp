#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

#include "Renderer.h"

void Renderer::render(std::vector<sf::ConvexShape> shapes) {
    window->clear(sf::Color(100, 100, 100));

    // std::cout << "Before: " << shapes.size() << std::endl;

    if (shapes.empty())
        shapes = lastDrawn;
    else
        lastDrawn = shapes;

    // std::cout << "After: " << shapes.size() << std::endl;

    // if (shapes.empty()) {   // Just in case and for debug, should not happen normally
    //     window->display();  // Keeps the window displaying
    //     std::cout << "No objects to display!" << std::endl;
    //     return;
    // }

    // int i = 0;
    for (auto shape : shapes) {
        // std::cout << object->getBodyPtr()->GetPosition().x << " " << object->getBodyPtr()->GetPosition().y << std::endl;

        // Blinking objects debug
        // if (shape.getPosition().x == 0) {
        //     std::cout << "FOUND " << ++i << std::endl;
        //     continue;
        // }
        window->draw(shape);
    }

    window->display();
}