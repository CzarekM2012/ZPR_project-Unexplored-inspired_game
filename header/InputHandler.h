#include <SFML/Window/Event.hpp>

#pragma once

class InputHandler {
    static constexpr float INPUT_SCALE = 1;
    
    public:             
    static const int PLAYER_COUNT_MAX = 4;
    static const int STATE_CONTROLS_PER_PLAYER = 3; // Position (x, y) and angle
    
    // This is set by InputHandler in every pass, and then checked by GameController each tick
    // contrary to Actions, that are sent into message queue only once, when they appear
    static float inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

    InputHandler() {};

    void handleInput(sf::Event event);

};