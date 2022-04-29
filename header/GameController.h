#pragma once

#include "InputHandler.h"
#include "State.h"

class GameController {
    const float FORCE_MOVE = 50;
    const float FORCE_LOOK = 250;
    const float FORCE_LOOK_MIN = 5;
    const float LOOK_ACC_DEG = 1;
    State state;

public:
    b2World* world;
    static bool stop;

    GameController();
    ~GameController() {delete world;};

    void run();

    State getStateCopy() const {
        State result = state;
        return result;
    }

    void signalStop() {this->stop = true;};

};
