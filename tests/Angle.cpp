#include <catch2/catch.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "Angle.h"

SCENARIO("One value retrieved from object can be converted to another with good accuracy") {
    GIVEN("Degree to radian conversion") {
        auto NEGLIGIBLE_COMPUTATIONAL_ERROR = M_PI / 180 / 60 / 60;  // 1 degree second
        auto DEG_TO_RAD = M_PI / 180.f;
        WHEN("Angle has value given in degrees") {
            std::array<float, 5> values = {-179.f, -90.f, 0.f, 90.f, 180.f};
            THEN("Difference between value in radians and converted value in degrees is within margin of error") {
                std::for_each(values.begin(), values.end(), [&](auto value) {
                    Angle angle(value);
                    REQUIRE(abs(angle.get() * DEG_TO_RAD - angle.get(Angle::unit::RAD)) < NEGLIGIBLE_COMPUTATIONAL_ERROR);
                });
            }
        }

        WHEN("Angle has value given in radians") {
            std::array<float, 5> values = {-M_PI * 0.999f, -M_PI * 0.5f, 0.f, M_PI * 0.5f, M_PI};
            THEN("Difference between value in radians and converted value in degrees is within margin of error") {
                std::for_each(values.begin(), values.end(), [&](auto value) {
                    Angle angle(value, Angle::unit::RAD);
                    REQUIRE(abs(angle.get() * DEG_TO_RAD - angle.get(Angle::unit::RAD)) < NEGLIGIBLE_COMPUTATIONAL_ERROR);
                });
            }
        }
    }

    GIVEN("Radian to degree conversion") {
        auto NEGLIGIBLE_COMPUTATIONAL_ERROR = 1.f / 60 / 60;  // 1 degree second
        auto RAD_TO_DEG = 180.f / M_PI;
        WHEN("Angle has value given in degrees") {
            std::array<float, 5> values = {-179.f, -90.f, 0.f, 90.f, 180.f};
            THEN("Difference between value in radians and converted value in degrees is within margin of error") {
                std::for_each(values.begin(), values.end(), [&](auto value) {
                    Angle angle(value);
                    REQUIRE(abs(angle.get() - angle.get(Angle::unit::RAD) * RAD_TO_DEG) < NEGLIGIBLE_COMPUTATIONAL_ERROR);
                });
            }
        }

        WHEN("Angle has value given in radians") {
            std::array<float, 5> values = {-M_PI * 0.999f, -M_PI * 0.5f, 0.f, M_PI * 0.5f, M_PI};
            THEN("Difference between value in radians and converted value in degrees is within margin of error") {
                std::for_each(values.begin(), values.end(), [&](auto value) {
                    Angle angle(value, Angle::unit::RAD);
                    REQUIRE(abs(angle.get() - angle.get(Angle::unit::RAD) * RAD_TO_DEG) < NEGLIGIBLE_COMPUTATIONAL_ERROR);
                });
            }
        }
    }
}

SCENARIO("Passed values are mapped to their equivalents in (-180, 180][deg] ((-PI, PI][rad]) range") {
    NEGLIGIBLE_COMPUTATIONAL_ERROR = 1.f / 60 / 60;
}