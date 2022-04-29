#include <SFML/Window/Event.hpp>

#pragma once

class InputHandler {
    static constexpr float INPUT_SCALE = 1;
    static constexpr float ANGLE_STEP_KEYBOARD = 5.0f;
    
    public:
    static const int PLAYER_COUNT_MAX = 4;
    static const int STATE_CONTROLS_PER_PLAYER = 3; // movement (x, y, sums to max. 1) and angle the player should face in deg
    
    static const int INPUT_MOVE_X = 0;
    static const int INPUT_MOVE_Y = 1;
    static const int INPUT_LOOK_ANGLE = 2;

    // This is set by InputHandler in every pass, and then checked by GameController each tick
    // contrary to Actions, that are sent into message queue only once, when they appear
    static float inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

    InputHandler();

    void handleInput(sf::Event event);

};