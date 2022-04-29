
#include <SFML/Window/Event.hpp>

#include "../header/InputHandler.h"
#include <box2d/box2d.h>

float InputHandler::inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

InputHandler::InputHandler() {
    // TODO: Check if necessary
    for(int i = 0; i < PLAYER_COUNT_MAX; ++i)
        for(int j = 0; j < STATE_CONTROLS_PER_PLAYER; ++j)
            inputStateTab[i][j] = 0;
}

#define UNUSED(x) (void)(x) //For now to disable "unused parameter" error
void InputHandler::handleInput(sf::Event event) {
    UNUSED(event);

    b2Vec2 input(0, 0);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S)) //TODO: Simplify this mess
        input.y = -INPUT_SCALE;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        input.y = INPUT_SCALE;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        input.x = -INPUT_SCALE;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        input.x = INPUT_SCALE;
    
    input.Normalize(); // Should only be normalized for keyboard input, analog sticks have to be handled a bit differently
    inputStateTab[0][INPUT_MOVE_X] = input.x;
    inputStateTab[0][INPUT_MOVE_Y] = input.y;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        inputStateTab[0][INPUT_LOOK_ANGLE] -= ANGLE_STEP_KEYBOARD;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        inputStateTab[0][INPUT_LOOK_ANGLE] += ANGLE_STEP_KEYBOARD;
    
    if(inputStateTab[0][INPUT_LOOK_ANGLE] > 180.0f)
        inputStateTab[0][INPUT_LOOK_ANGLE] -= 360.f;
    else if(inputStateTab[0][INPUT_LOOK_ANGLE] < -180.0f)
        inputStateTab[0][INPUT_LOOK_ANGLE] += 360.f;
}
