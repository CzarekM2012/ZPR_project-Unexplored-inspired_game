#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "State.h"

/// Draws state of game, synchronized every frame
class Renderer {
   private:
    sf::RenderWindow* window;
    std::vector<sf::ConvexShape> lastDrawn;

   public:
    Renderer(){};
    Renderer(sf::RenderWindow* render_window)
        : window(render_window){};

    void render(std::vector<sf::ConvexShape> shapes);
};