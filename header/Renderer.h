#pragma once

#include <SFML/Graphics.hpp>

#include "State.h"

class Renderer {

    sf::CircleShape shape;

    public:
    Renderer();

    void render(sf::RenderWindow& window, State state);

};
