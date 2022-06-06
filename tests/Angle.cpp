#include <catch2/catch.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <array>
#include <vector>

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
    float FULL_ANGLE = 360.f;
    WHEN("Angles are created with values from outside the range") {
        std::vector<std::array<float, 2>> values = {
            {-65340.f, 180.f},
            {-6574.f, -94.f},
            {687540.f, -60.f},
            {987413.f, -67.f},
            {434560.f, 40.f},
            {-180.f, 180.f}};
        THEN("Values retrieved from them fall in range and are equivalent to constructor arguments") {
            std::for_each(values.begin(), values.end(), [&](const auto& arr) {
                REQUIRE(Angle(arr[0]).get() == arr[1]);
                REQUIRE(fmod(arr[0] - Angle(arr[1]).get(), FULL_ANGLE) == 0);
            });
        }
    }
}

SCENARIO("Values of angles that are result of operations on angles fall in (-180, 180][deg] ((-PI, PI][rad]) range") {
    WHEN("Angles are added") {
        std::vector<std::array<float, 3>> values = {
            {150, 31, -179},
            {-60, -120, 180},
            {31, 34, 65},
            {129, -367, 122},
            {129, -7, 122},
            {-18, -42, -60}};
        THEN("Values retrieved from them fall in range") {
            std::for_each(values.begin(), values.end(), [&](const auto& arr) {
                Angle first = Angle(arr[0]), second = Angle(arr[1]);
                REQUIRE((first + second).get() == arr[2]);
                first += second;
                REQUIRE(first.get() == arr[2]);
            });
        }
    }
    WHEN("Angles are substracted") {
        std::vector<std::array<float, 3>> values = {
            {-150, 30, 180},
            {60, -120, 180},
            {31, 34, -3},
            {129, -367, 136},
            {129, -7, 136},
            {-12, -42, 30}};
        THEN("Values retrieved from them fall in range") {
            std::for_each(values.begin(), values.end(), [&](const auto& arr) {
                Angle first = Angle(arr[0]), second = Angle(arr[1]);
                REQUIRE((first - second).get() == arr[2]);
                first -= second;
                REQUIRE(first.get() == arr[2]);
            });
        }
    }
}