#pragma once

#include <chrono>
#include <mutex>

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
    const std::chrono::duration<int64_t, std::milli> TIME_STEP = std::chrono::duration<int64_t, std::milli>(10);  ///< How much time (ms) should pass between each physics step() call

    State state;
    std::array<Player*, InputHandler::PLAYER_COUNT_MAX> players;

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;
    std::mutex drawableCopyMutex;

    void processPlayerInputStates(const int playerId);
    void processAction(const Action& action);

   public:
    b2World* world;
    static bool stop;
    bool running = false;

    GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);
    ~GameController() { delete world; };

    void prepareGame();  ///< Creates and populates world with objects, assigns players
    void restartGame();  ///< Destroys current world and calls prepareGame()
    void run();          ///< Processes physics and player input each tick untill 'stop' is set to true

    std::vector<sf::ConvexShape> getDrawablesCopy();

    Item* getFirstPickableItem(Player* player) const;

    void signalStop() { this->stop = true; };
};
