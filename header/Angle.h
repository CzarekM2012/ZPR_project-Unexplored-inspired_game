#ifndef ANGLE_H
#define ANGLE_H
#define _USE_MATH_DEFINES
#include <math.h>

/// Class for consistent interpretation of angles.
/*!
All passed values will be mapped to the range (-180, 180][deg] ((-PI, PI][rad]).
Appropriate static function allows passing value interpreted as [deg] or [rad].
Appropriate method allows retrieval of mapped value as [deg] or [rad].
*/
class Angle {
   public:
    static constexpr auto FULL_ANGLE_DEG = 360.f;
    static constexpr auto HALF_ANGLE_DEG = FULL_ANGLE_DEG / 2;
    static constexpr auto RIGHT_ANGLE_DEG = HALF_ANGLE_DEG / 2;

   private:
    static constexpr float RAD_TO_DEG_MUL = HALF_ANGLE_DEG / M_PI;
    float value;
    void map();

   public:
    enum unit {
        DEG,
        RAD
    };
    Angle(float val = 0.f, unit unit = unit::DEG);
    float get(unit unit = unit::DEG);
    Angle operator+(const Angle& other) const;
    Angle operator-(const Angle& other) const;
    Angle operator-() const;
    Angle& operator+=(const Angle& other);
    Angle& operator-=(const Angle& other);
};

#endif  // ANGLE_H