#pragma once

#include "readerwriterqueue.h"

#include "InputHandler.h"
#include "ObjectClasses.h"
#include "State.h"

/// Controls game logic and physics. Runs in its own thread
class GameController {
    const float FORCE_MOVE = 50;
    const float FORCE_LOOK = 250;
    const float FORCE_LOOK_MIN = 5;
    const float LOOK_ACC_DEG = 1;

    const float PICKUP_RANGE = 10;
    const float TIME_STEP = 0.03;  ///< In seconds. How much time should pass between each physics step()

    State state;

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;

    void processPlayerInputStates(int playerId);
    void processAction(const Action& action);

   public:
    b2World* world;
    static bool stop;

    GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);
    ~GameController() { delete world; };

    void run();

    const State* getStateCopy() const {
        // State result = state;
        return &state;
    }

    Item* getFirstPickableItem(Player* player) const;

    void signalStop() { this->stop = true; };
};
