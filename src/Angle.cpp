#include "Angle.h"

/// map Angle to its equivalent in (-180, 180] range
void Angle::map() {
    value = fmod(value, FULL_ANGLE_DEG);
    // value in range (-360, 360)
    // range (-360, -180] is equivalent to (0, 180], and (180, 360) to (-180, 0)
    if (value <= -HALF_ANGLE_DEG)
        value += FULL_ANGLE_DEG;
    else if (value > HALF_ANGLE_DEG)
        value -= FULL_ANGLE_DEG;
}

Angle::Angle(float val, unit unit)
    : value(val) {
    if (unit == unit::RAD)
        value *= RAD_TO_DEG_MUL;
    map();
}

float Angle::get(unit unit) {
    float returnValue = value;
    if (unit == unit::RAD)
        returnValue /= RAD_TO_DEG_MUL;
    return returnValue;
}

Angle Angle::operator+(const Angle& other) {
    return Angle(value + other.value);
}

Angle Angle::operator-(const Angle& other) {
    return Angle(value - other.value);
}

Angle& Angle::operator+=(const Angle& other) {
    value += other.value;
    map();
    return *this;
}

Angle& Angle::operator-=(const Angle& other) {
    value -= other.value;
    map();
    return *this;
}