
#include <SFML/Window/Event.hpp>

#include "../header/InputHandler.h"

const int InputHandler::PLAYER_COUNT_MAX;
const int InputHandler::STATE_CONTROLS_PER_PLAYER;
float InputHandler::inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

void InputHandler::handleInput(sf::Event event) {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        inputStateTab[0][1] = -INPUT_SCALE;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
       inputStateTab[0][1] = INPUT_SCALE;
    else
        inputStateTab[0][1] = 0;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        inputStateTab[0][0] = -INPUT_SCALE;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        inputStateTab[0][0] = INPUT_SCALE;
    else
        inputStateTab[0][0] = 0;
}