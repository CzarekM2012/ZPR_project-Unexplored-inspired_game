#include <SFML/Graphics.hpp>

#include <memory>

#include "Renderer.h"

void Renderer::render(std::vector<sf::ConvexShape> shapes) {
    window->clear(COLOR_BACKGROUND);

    if (shapes.empty())
        shapes = lastDrawn;
    else
        lastDrawn = shapes;

    for (auto shape : shapes)
        window->draw(shape);

    window->display();
}