#include <SFML/Window/Event.hpp>

#include <box2d/box2d.h>
#include <iostream>
#include "InputHandler.h"

std::array<std::tuple<b2Vec2, Angle>, InputHandler::PLAYER_COUNT_MAX> InputHandler::inputStateTab;

bool InputHandler::isAnyJoystickConnected() {
    for (unsigned int i = 0; i < PLAYER_COUNT_MAX; ++i) {
        if (sf::Joystick::isConnected(i))
            return true;
    }
    return false;
}

void InputHandler::handleJoystickStates() {
    for (int controllerId = 0; controllerId < PLAYER_COUNT_MAX; ++controllerId) {
        // Movement
        b2Vec2 movement(sf::Joystick::getAxisPosition(controllerId, sf::Joystick::X),
                        sf::Joystick::getAxisPosition(controllerId, sf::Joystick::Y));
        movement *= (1 / SFML_AXIS_INPUT_SCALE);  // no division assignment operator
        movement = normalizeMovement(movement);

        // Look angle
        Angle targetAngle = std::get<INPUT_LOOK_ANGLE>(inputStateTab.at(controllerId));
        b2Vec2 targetAngleInput(sf::Joystick::getAxisPosition(controllerId, sf::Joystick::U),
                                sf::Joystick::getAxisPosition(controllerId, sf::Joystick::V));
        targetAngleInput *= (1 / SFML_AXIS_INPUT_SCALE);

        if (targetAngleInput.Length() > AXIS_DEADZONE) {
            targetAngle = Angle(atan2(targetAngleInput.y, targetAngleInput.x), Angle::unit::RAD) -
                          Angle(Angle::RIGHT_ANGLE_DEG);  // translate angle to its equivalent in game world
        }

        inputStateTab[controllerId] = std::make_tuple(movement, targetAngle);
    }
}

void InputHandler::handleKeyboardState() {
    b2Vec2 movement(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        movement.y = -AXIS_MOVE_KEYBOARD;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
             !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        movement.y = AXIS_MOVE_KEYBOARD;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement.x = -AXIS_MOVE_KEYBOARD;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
             !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement.x = AXIS_MOVE_KEYBOARD;
    movement = normalizeMovement(movement);

    Angle targetAngle = std::get<INPUT_LOOK_ANGLE>(inputStateTab.at(0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        targetAngle -= Angle(ANGLE_STEP_KEYBOARD);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) &&
             !sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        targetAngle += Angle(ANGLE_STEP_KEYBOARD);

    inputStateTab[0] = std::make_tuple(movement, targetAngle);
}

b2Vec2 InputHandler::normalizeMovement(b2Vec2 movement) {
    // Returns 0 vector if movement is to movement vector is too short.
    // returns movement vector normalized to unit vector, if it is too long.
    if (movement.Length() < AXIS_DEADZONE)
        movement.SetZero();
    else if (movement.Length() > UNIT_VECTOR_LENGTH * 2 / 3)  // TODO: Makeshift, replace with decent scaling
        movement.Normalize();
    return movement;
}

InputHandler::InputHandler(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q)
    : action_q(q) {}

void InputHandler::handleStates() {
    if (isAnyJoystickConnected())
        handleJoystickStates();
    else
        handleKeyboardState();
}

/// Handling of one-time-pressed keys and buttons
void InputHandler::handleEvent(sf::Event event) {
    Action action;

    if (event.type == sf::Event::JoystickButtonPressed) {
        action.playerId = event.joystickButton.joystickId;
        switch (event.joystickButton.button) {
            case JoystickButton::X:
                action.type = Action::Type::PICK_LEFT;
                break;

            case JoystickButton::B:
                action.type = Action::Type::PICK_RIGHT;
                break;

            case JoystickButton::L3:
                action.type = Action::Type::DROP_LEFT;
                break;

            case JoystickButton::R3:
                action.type = Action::Type::DROP_RIGHT;
                break;

            case JoystickButton::LB:
                action.type = Action::Type::ACT_PREP_LEFT;
                break;

            case JoystickButton::RB:
                action.type = Action::Type::ACT_PREP_RIGHT;
                break;
        }
    }

    if (event.type == sf::Event::JoystickButtonReleased) {
        action.playerId = event.joystickButton.joystickId;
        switch (event.joystickButton.button) {
            case JoystickButton::LB:
                action.type = Action::Type::ACT_LEFT;
                break;

            case JoystickButton::RB:
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
