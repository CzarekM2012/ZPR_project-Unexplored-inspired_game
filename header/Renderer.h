#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "State.h"

/// Draws state of game, synchronized every frame 
class Renderer {
private:
    sf::RenderWindow * window;

public:
    Renderer() {};
    Renderer(sf::RenderWindow * render_window) : window(render_window) {};

    void render(const State & state) const;

};
