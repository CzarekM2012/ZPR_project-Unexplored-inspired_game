#include <catch2/catch.hpp>

#include "GameController.h"

SCENARIO("Game preparation") {
    GIVEN("Game controller is initialized") {
        std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(10));
        GameController gameController(action_q);

        WHEN("Nothing has happened yet") {
            THEN("There is nothing to draw") {
                REQUIRE(gameController.getDrawablesCopy().empty());
            }
        }

        WHEN("A game was prepared") {
            gameController.prepareGame();

            THEN("Some drawables have been created") {
                REQUIRE(!gameController.getDrawablesCopy().empty());
            }
        }
    }
}

SCENARIO("Custom game") {
    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(10));
    GameController gameController(action_q);
    auto state = gameController.getState();
    auto world = new b2World(b2Vec2(0, 0));

    GIVEN("There is a player and a sword") {
        auto player = new Player();
        state->add(player);
        player->createBody(world, b2Vec2(0, 0));

        auto sword = new Sword();
        state->add(sword);
        sword->createBody(world, b2Vec2(0, 0));

        WHEN("") {
            THEN("Player can pickup the sword") {
                REQUIRE(gameController.getFirstPickableItem(player) == sword);
            }
        }
    }
}