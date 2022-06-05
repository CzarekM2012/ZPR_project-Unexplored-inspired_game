#include <catch2/catch.hpp>

#include "Action.h"
#include "InputHandler.h"

// Not really much can be tested here without simulating hardware input somehow

SCENARIO("Event handling") {
    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(10));
    InputHandler InputHandler(action_q);

    GIVEN("There is an event to handle") {
        sf::Event event;

        WHEN("X was pressed on Joystick 1") {
            event.type = sf::Event::JoystickButtonPressed;
            event.joystickButton.joystickId = 1;
            event.joystickButton.button = InputHandler::JoystickButton::X;

            InputHandler.handleEvent(event);
            THEN("A correct action should be sent") {
                Action action;
                REQUIRE(action_q->try_dequeue(action));
                REQUIRE(action.playerId == 1);
                REQUIRE(action.type == Action::PICK_LEFT);
            }
        }
    }
}

TEST_CASE("Movement input normalization of (1, 0)", "[input]") {
    auto moveVec = InputHandler::normalizeMovement(b2Vec2(1, 0));
    REQUIRE(moveVec == b2Vec2(1, 0));
}

TEST_CASE("Movement input normalization of (1, 1)", "[input]") {
    auto moveVec = InputHandler::normalizeMovement(b2Vec2(1, 1));
    REQUIRE(moveVec == b2Vec2(sqrt(2) / 2, sqrt(2) / 2));  // If this ever fails due to rounding errors, add a margin of error
}