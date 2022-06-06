#include <catch2/catch.hpp>

#include "ObjectClasses.h"
#include "State.h"

SCENARIO("Simple state operations") {
    GIVEN("There's a state") {
        State state;

        WHEN("State is empty") {
            THEN("Object count is 0") {
                REQUIRE(state.getObjectCount() == 0);
                REQUIRE(state.get(0) == nullptr);
            }
        }

        WHEN("Two objects were added") {
            auto sword = new Sword();
            auto shield = new Shield();
            auto shieldPtr = std::unique_ptr<PhysicalObject>(shield);
            state.add(sword);
            state.add(std::move(shieldPtr));

            THEN("Object count is 2") {
                REQUIRE(state.getObjectCount() == 2);
            }

            THEN("Objects can be retrieved using correct indexes") {
                REQUIRE(state.get(0) == sword);
                REQUIRE(state.get(1) == shield);
            }

            THEN("Last object is retrieved correctly") {
                REQUIRE(state.getLast() == shield);
            }

            THEN("Objects can be removed") {
                state.remove(sword);
                REQUIRE(state.getObjectCount() == 1);
                REQUIRE(state.get(0) == shield);

                state.remove(shield);
                REQUIRE(state.getObjectCount() == 0);
                REQUIRE(state.get(0) == nullptr);
            }

            THEN("An object can't be removed twice") {
                state.remove(sword);
                state.remove(sword);
                REQUIRE(state.getObjectCount() == 1);
                REQUIRE(state.get(0) == shield);
            }
        }
    }
}