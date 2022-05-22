#include <SFML/Window/Event.hpp>

#include <box2d/box2d.h>
#include <iostream>
#include "InputHandler.h"

float InputHandler::inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

InputHandler::InputHandler(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q)
    : action_q(q) {
    // TODO: Check if necessary
    for (int i = 0; i < PLAYER_COUNT_MAX; ++i)
        for (int j = 0; j < STATE_CONTROLS_PER_PLAYER; ++j)
            inputStateTab[i][j] = 0;
}

#define UNUSED(x) (void)(x)  // For now to disable "unused parameter" error
void InputHandler::handleInput(sf::Event event) {
    UNUSED(event);

    b2Vec2 input(0, 0);

    // Is joystick #0 connected?
    bool padConnected = sf::Joystick::isConnected(0);

    // Movement
    if (padConnected) {
        input.x = sf::Joystick::getAxisPosition(0, sf::Joystick::X) /
                  SFML_AXIS_INPUT_SCALE;
        input.y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y) /
                  SFML_AXIS_INPUT_SCALE;
        // std::cout << "Connected: " << padConnected << " Button count: " <<
        // sf::Joystick::getButtonCount(0) << " Pressed: " <<
        // sf::Joystick::isButtonPressed(0, 2) << " Pos: " <<
        // sf::Joystick::getAxisPosition(0, sf::Joystick::Y) << std::endl;
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&
            !sf::Keyboard::isKeyPressed(
                sf::Keyboard::S))  // TODO: Simplify this mess
            input.y = -INPUT_SCALE;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
                 !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            input.y = INPUT_SCALE;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            input.x = -INPUT_SCALE;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
                 !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            input.x = INPUT_SCALE;
    }

    if (input.Length() < AXIS_DEADZONE)
        input.SetZero();
    else if (input.Length() > 1.0f)
        input.Normalize();

    inputStateTab[0][INPUT_MOVE_X] = input.x;
    inputStateTab[0][INPUT_MOVE_Y] = input.y;

    // Look angle
    if (padConnected) {
        input.x = sf::Joystick::getAxisPosition(0, sf::Joystick::U) /
                  SFML_AXIS_INPUT_SCALE;
        input.y = sf::Joystick::getAxisPosition(0, sf::Joystick::V) /
                  SFML_AXIS_INPUT_SCALE;

        if (input.Length() > AXIS_DEADZONE)
            inputStateTab[0][INPUT_LOOK_ANGLE] =
                atan2(input.y, input.x) / b2_pi * 180 - 90;
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            inputStateTab[0][INPUT_LOOK_ANGLE] -= ANGLE_STEP_KEYBOARD;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) &&
                 !sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            inputStateTab[0][INPUT_LOOK_ANGLE] += ANGLE_STEP_KEYBOARD;
    }

    if (inputStateTab[0][INPUT_LOOK_ANGLE] > 180.0f)
        inputStateTab[0][INPUT_LOOK_ANGLE] -= 360.f;
    else if (inputStateTab[0][INPUT_LOOK_ANGLE] < -180.0f)
        inputStateTab[0][INPUT_LOOK_ANGLE] += 360.f;

    // Handling of one-time-pressed keys and buttons
    if (event.type == sf::Event::EventType::KeyPressed) {
        Action action(-1, 0);

        switch (event.key.code) {
            case sf::Keyboard::Tilde:
                action = Action(-1, Action::TYPE_DEBUG);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num1:
                action = Action(0, Action::TYPE_PICK_LEFT);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num2:
                action = Action(0, Action::TYPE_DROP_LEFT);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num3:
                action = Action(0, Action::TYPE_PICK_RIGHT);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num4:
                action = Action(0, Action::TYPE_DROP_RIGHT);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num5:
                action = Action(0, Action::TYPE_ACT_LEFT);
                action_q->enqueue(action);
                break;

            case sf::Keyboard::Num6:
                action = Action(0, Action::TYPE_ACT_RIGHT);
                action_q->enqueue(action);
                break;

            default:
                break;
        }
    }
}
