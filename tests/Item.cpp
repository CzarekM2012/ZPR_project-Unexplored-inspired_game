#include <catch2/catch.hpp>

#include "Item.h"

SCENARIO("Cooldown tests") {
    GIVEN("There's an item") {
        Item item;

        WHEN("Item's set on cooldown") {
            item.setCooldown(10);

            THEN("It really is on cooldown") {
                REQUIRE(item.isOnCooldown());
            }

            THEN("It can't be used") {
                REQUIRE(!item.canBeUsed());

                item.useTrigger();
                REQUIRE(!item.isBeingUsed());
            }

            THEN("Cooldown can be lowered correctly") {
                for (int i = 0; i < 10; ++i) {
                    REQUIRE(item.isOnCooldown());
                    item.lowerCooldown();
                }
                REQUIRE(!item.isOnCooldown());
            }

            THEN("Cooldown can be reset") {
                item.resetCooldown();
                REQUIRE(!item.isOnCooldown());
            }
        }
    }
}

SCENARIO("Action tests") {
    GIVEN("There's an item") {
        Item item;

        WHEN("It has been used") {
            item.useTrigger();

            THEN("It is being used") {
                REQUIRE(item.isBeingUsed());
            }

            THEN("Correct time left is being returned") {
                REQUIRE(item.getActionTimeLeft() == item.getActionTimeTotal());
                item.useTick();
                REQUIRE(item.getActionTimeLeft() == item.getActionTimeTotal() - 1);
            }

            THEN("It can't be used again") {
                REQUIRE(!item.canBeUsed());
            }

            THEN("Action time can be lowered correctly") {
                for (int i = 0; i < item.getActionTimeTotal(); ++i) {
                    REQUIRE(item.isBeingUsed());
                    item.useTick();
                }
                REQUIRE(item.getActionTimeLeft() == 0);
                REQUIRE(item.isOnCooldown());
            }

            THEN("Cooldown can be reset") {
                item.resetCooldown();
                REQUIRE(!item.isOnCooldown());
            }
        }
    }
}
