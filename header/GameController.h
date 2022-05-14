#pragma once

#include "../external/readerwriterqueue/readerwriterqueue.h"

#include "InputHandler.h"
#include "State.h"
class GameController {
    const float FORCE_MOVE = 50;
    const float FORCE_LOOK = 250;
    const float FORCE_LOOK_MIN = 5;
    const float LOOK_ACC_DEG = 1;

    const float PICKUP_RANGE = 10;

    State state;

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;

    void processPlayerInputStates(int playerId);
    void processAction(const Action & action);

public:
    b2World* world;
    static bool stop;

    GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);
    ~GameController() {delete world;};

    void run();

    State getStateCopy() const {
        State result = state;
        return result;
    }

    void signalStop() {this->stop = true;};

};
