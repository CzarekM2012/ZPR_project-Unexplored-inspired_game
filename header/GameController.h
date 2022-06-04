#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include <chrono>
#include <mutex>

#include "readerwriterqueue.h"

#include "Constants.h"
#include "InputHandler.h"
#include "ObjectClasses.h"
#include "State.h"

/// Controls game logic and physics. Runs in its own thread
class GameController {
    const float FORCE_MOVE = 50;
    const float FORCE_LOOK = 250;
    const float FORCE_LOOK_MIN = 5;
    const float LOOK_ACC_DEG = 0.5f;
    const float PICKUP_RANGE = 10;
    const std::chrono::duration<int64_t, std::nano> TIME_STEP = std::chrono::duration<int64_t, std::nano>(1000000000 / TICKS_PER_SECOND);  ///< How much time should pass between each physics step() call

    State state;                                                       ///< State of the game. Here pointers to all objects' data are stored
    b2World* world = nullptr;                                          ///< Box2D World. All physical object representations are stored in and managed by this object. Deleting it also removes all other Box2D objects
    std::array<Player*, InputHandler::PLAYER_COUNT_MAX> players;       ///< Player list, to make sure that controller assignment doesn't change and make access easier
    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;  ///< Event queue that receives commands from InputHandler
    std::mutex drawableCopyMutex;                                      ///< Guards access to game state. Has to be locked when adding/deleting objects and copying drawables

    void processPlayerInputStates(const int playerId);
    void processAction(const Action& action);

   public:
    volatile static bool stop;  ///< Checked each run() loop, changing this to true effectively stops the game logic thread

    GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);
    ~GameController();

    void prepareGame();  ///< Creates and populates world with objects, assigns players
    void restartGame();  ///< Destroys current world and calls prepareGame()
    void run();          ///< Processes physics and player input each tick untill 'stop' is set to true

    std::vector<sf::ConvexShape> getDrawablesCopy();  ///< Used by Renderer to retrieve drawable objects

    Item* getFirstPickableItem(Player* player) const;  ///< Finds first onowned item in PICKUP_RANGE from given player

    State* getState() { return &state; };  ///< Not really safe to call, mostly for testing
};

#endif  // GAME_CONTROLER_H