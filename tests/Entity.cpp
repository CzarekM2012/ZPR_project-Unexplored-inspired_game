#include <catch2/catch.hpp>

#include "Entity.h"

SCENARIO("Entities can be damaged and destroyed") {
    GIVEN("Entity has full hp") {
        Entity entity;

        WHEN("Entity is damaged") {
            entity.damage(10);

            THEN("Hp are correctly reduced") {
                REQUIRE(entity.getHp() == entity.getMaxHp() - 10);
            }
        }

        WHEN("Entity is damaged for more HP than it has") {
            entity.damage(entity.getMaxHp() + 10);

            THEN("Hp are correctly reduced") {
                REQUIRE(entity.getHp() == 0);
            }

            THEN("Entity signals that it should be destroyed") {
                REQUIRE(entity.toDestroy);
            }

            // FULL_GAME
            // THEN("Color of its first polygon should be set to black") {
            //     entity.synchronize();
            //     REQUIRE(entity.getViews()[0].getFillColor() == sf::Color::Black);
            // }
        }

        WHEN("Damaged entity is invulnerable") {
            entity.setInvulnerable(true);

            THEN("isInvulnerable should return correct 'true'") {
                REQUIRE(entity.isInvulnerable());
            }

            THEN("It should take no damage") {
                entity.damage(10);
                REQUIRE(entity.getHp() == entity.getMaxHp());
            }
        }
    }
}