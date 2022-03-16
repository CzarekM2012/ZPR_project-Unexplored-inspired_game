#include <SFML/Graphics.hpp>

#include "../header/Renderer.h"

Renderer::Renderer() {
    shape = sf::CircleShape(100.f);
    shape.setFillColor(sf::Color::Green);
}

void Renderer::render(sf::RenderWindow& window, State state) {

    if(state.getObjectCount() == 0)
        return;

    shape.setPosition(sf::Vector2f(state.get(0)->x, state.get(0)->y));

    window.clear();
    window.draw(shape);
    window.display();
}