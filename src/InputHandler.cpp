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
void InputHandler::handleStates() {
    b2Vec2 input(0, 0);

    // Is joystick #0 connected?
    bool noController = true;

    for (int controllerId = 0; controllerId < PLAYER_COUNT_MAX; ++controllerId) {
        if (!sf::Joystick::isConnected(controllerId))
            continue;

        noController = false;

        // Movement
        input.x = sf::Joystick::getAxisPosition(controllerId, sf::Joystick::X) / SFML_AXIS_INPUT_SCALE;
        input.y = sf::Joystick::getAxisPosition(controllerId, sf::Joystick::Y) / SFML_AXIS_INPUT_SCALE;

        if (input.Length() < AXIS_DEADZONE)
            input.SetZero();
        else if (input.Length() > 1.0f)  // TODO: Makeshift, replace with decent scaling
            input.Normalize();

        inputStateTab[controllerId][INPUT_MOVE_X] = input.x;
        inputStateTab[controllerId][INPUT_MOVE_Y] = input.y;

        // Look angle
        input.x = sf::Joystick::getAxisPosition(controllerId, sf::Joystick::U) / SFML_AXIS_INPUT_SCALE;
        input.y = sf::Joystick::getAxisPosition(controllerId, sf::Joystick::V) / SFML_AXIS_INPUT_SCALE;

        if (input.Length() > AXIS_DEADZONE)
            inputStateTab[controllerId][INPUT_LOOK_ANGLE] =
                atan2(input.y, input.x) / b2_pi * 180 - 90;

        if (inputStateTab[controllerId][INPUT_LOOK_ANGLE] > 180.0f)
            inputStateTab[controllerId][INPUT_LOOK_ANGLE] -= 360.f;
        else if (inputStateTab[controllerId][INPUT_LOOK_ANGLE] < -180.0f)
            inputStateTab[controllerId][INPUT_LOOK_ANGLE] += 360.f;
    }

    if (noController) {
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

        if (input.Length() < AXIS_DEADZONE)
            input.SetZero();
        else if (input.Length() > 1.0f)  // TODO: Makeshift, replace with decent scaling
            input.Normalize();

        inputStateTab[0][INPUT_MOVE_X] = input.x;
        inputStateTab[0][INPUT_MOVE_Y] = input.y;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            inputStateTab[0][INPUT_LOOK_ANGLE] -= ANGLE_STEP_KEYBOARD;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) &&
                 !sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            inputStateTab[0][INPUT_LOOK_ANGLE] += ANGLE_STEP_KEYBOARD;

        if (inputStateTab[0][INPUT_LOOK_ANGLE] > 180.0f)
            inputStateTab[0][INPUT_LOOK_ANGLE] -= 360.f;
        else if (inputStateTab[0][INPUT_LOOK_ANGLE] < -180.0f)
            inputStateTab[0][INPUT_LOOK_ANGLE] += 360.f;
    }
}

/// Handling of one-time-pressed keys and buttons
void InputHandler::handleEvent(sf::Event event) {
    Action action;

    if (event.type == sf::Event::JoystickButtonPressed) {
        action.playerId = event.joystickButton.joystickId;
        switch (event.joystickButton.button) {
            case JOYSTICK_BUTTON_X:
                action.type = Action::Type::PICK_LEFT;
                break;

            case JOYSTICK_BUTTON_Y:
                action.type = Action::Type::DROP_LEFT;
                break;

            case JOYSTICK_BUTTON_A:
                action.type = Action::Type::PICK_RIGHT;
                break;

            case JOYSTICK_BUTTON_B:
                action.type = Action::Type::DROP_RIGHT;
                break;

            case JOYSTICK_BUTTON_LB:
                action.type = Action::Type::ACT_PREP_LEFT;
                break;

            case JOYSTICK_BUTTON_RB:
                action.type = Action::Type::ACT_PREP_RIGHT;
                break;
        }
    }

    if (event.type == sf::Event::JoystickButtonReleased) {
        action.playerId = event.joystickButton.joystickId;
        switch (event.joystickButton.button) {
            case JOYSTICK_BUTTON_LB:
                action.type = Action::Type::ACT_LEFT;
                break;

            case JOYSTICK_BUTTON_RB:
                action.type = Action::Type::ACT_RIGHT;
                break;
        }
    }

    if (event.type == sf::Event::EventType::KeyPressed) {
        action.playerId = 0;  // Keyboard is for debug, always controls player 1
        switch (event.key.code) {
            case sf::Keyboard::Tilde:
                action.type = Action::Type::DEBUG;
                break;

            case sf::Keyboard::R:
                action.type = Action::Type::RESTART;
                break;

            case sf::Keyboard::Num1:
                action.type = Action::Type::PICK_LEFT;
                break;

            case sf::Keyboard::Num2:
                action.type = Action::Type::DROP_LEFT;
                break;

            case sf::Keyboard::Num3:
                action.type = Action::Type::PICK_RIGHT;
                break;

            case sf::Keyboard::Num4:
                action.type = Action::Type::DROP_RIGHT;
                break;

            case sf::Keyboard::Num5:
                action.type = Action::Type::ACT_PREP_LEFT;
                break;

            case sf::Keyboard::Num6:
                action.type = Action::Type::ACT_PREP_RIGHT;
                break;

            default:
                break;
        }
    }

    if (event.type == sf::Event::EventType::KeyReleased) {
        action.playerId = 0;
        switch (event.key.code) {
            case sf::Keyboard::Num5:
                action.type = Action::Type::ACT_LEFT;
                break;

            case sf::Keyboard::Num6:
                action.type = Action::Type::ACT_RIGHT;
                break;

            default:
                break;
        }
    }

    if (action.type != Action::Type::NONE) {
        action_q->enqueue(action);
    }
}
