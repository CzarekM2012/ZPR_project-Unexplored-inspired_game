#pragma once

#include "InputHandler.h"
#include "State.h"

class GameController {
    const float STEP = 50;   
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
