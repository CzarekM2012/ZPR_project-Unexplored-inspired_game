#ifndef RENDERER_H
#define RENDERER_H

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "State.h"

/// Draws state of game, synchronized every frame
class Renderer {
    const sf::Color COLOR_BACKGROUND = sf::Color(100, 100, 100);

    sf::RenderWindow* window;
    std::vector<sf::ConvexShape> lastDrawn;  ///< A copy of last drawn object views, used in case they can't be retrieved next time

   public:
    Renderer(sf::RenderWindow* render_window)
        : window(render_window){};

    void render(std::vector<sf::ConvexShape> shapes);  ///< Draw all given shapes
};

#endif  // RENDERER_H