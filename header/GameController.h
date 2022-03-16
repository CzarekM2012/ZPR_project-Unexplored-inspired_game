#pragma once

#include "InputHandler.h"
#include "State.h"

class GameController {
    const float STEP = 1;    
    State state;

    public:             
    static bool stop;

    GameController();

    void run();

    State getStateCopy() {
        State result = state; // TODO: Check it this really copies
        return result;
    }

    void signalStop() {this->stop = true;};

};
