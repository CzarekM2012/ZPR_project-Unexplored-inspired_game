#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

#include "Renderer.h"

void Renderer::render(const State* state) const {
    window->clear(sf::Color(100, 100, 100));

    if (state->getObjectCount() == 0) {  // Just in case and for debug, should not happen normally
        window->display();               // Keeps the window displaying
        std::cout << "No objects to display!" << std::endl;
        return;
    }

    for (auto&& object : state->objects) {
        // std::cout << object->getBodyPtr()->GetPosition().x << " " << object->getBodyPtr()->GetPosition().y << std::endl;
        object->synchronize();  // TODO: This probably sould be called on GameController's side and state copy put in a critical section

        for (auto view : object->getViews()) {
            window->draw(view);
        }
    }

    window->display();
}