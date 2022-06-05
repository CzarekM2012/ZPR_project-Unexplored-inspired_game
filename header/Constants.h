#ifndef CONSTANTS_H
#define CONSTANTS_H

const int TICKS_PER_SECOND = 100;  ///< How many times per second should GameController loop (physics step() inside) run
const float M_TO_PX = 5;           ///< box2d meter to SFML pixels size convertion ratio
const float PX_TO_M = 1.0 / M_TO_PX;

const float PICKUP_RANGE = 10;
const float DROP_RANGE = PICKUP_RANGE * 1.5;
const float DROP_RANGE_SQ = DROP_RANGE * DROP_RANGE;

#endif  // CONSTANTS_H