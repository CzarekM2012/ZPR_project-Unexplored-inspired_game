#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <box2d/box2d.h>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "Action.h"
#include "Angle.h"
#include "readerwriterqueue.h"

/// Passes player input to the GameController through states and Action queue
class InputHandler {
    static constexpr float AXIS_MOVE_KEYBOARD = 1.0f;
    static constexpr float ANGLE_STEP_KEYBOARD = 0.1f;
    static constexpr float SFML_AXIS_INPUT_SCALE = 100;
    static constexpr float AXIS_DEADZONE = 0.2;

    static constexpr float UNIT_VECTOR_LENGTH = sqrt(2.f);

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;  ///< Event queue for sending commands to GameController

    bool isAnyJoystickConnected();
    void handleJoystickStates();
    void handleKeyboardState();

   public:
    static const int PLAYER_COUNT_MAX = 4;
    static const int STATE_CONTROLS_PER_PLAYER = 3;  // Movement (x and y, sums to max. 1) and angle the player should face in deg

    static const int INPUT_MOVEMENT = 0;
    static const int INPUT_LOOK_ANGLE = 1;

    // For XInput controller, default bindings may differ between gamepads
    // Named "JOYSTICK" after SFML convention
    enum JoystickButton {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LB = 4,
        RB = 5,
        LS = 6,  ///< Left system button
        RS = 7,  ///< Right system button
        L3 = 8,  ///< Left stick press
        R3 = 9   ///< Right stick press
    };

    /// This is set by InputHandler in every pass, and then checked by GameController each tick; contrary to Actions, that are sent into message queue only once, when they appear
    static std::array<std::tuple<b2Vec2, Angle>, PLAYER_COUNT_MAX> inputStateTab;

    static b2Vec2 normalizeMovement(b2Vec2 movement);

    InputHandler(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);

    void handleEvent(sf::Event event);  ///< Hanldes SFML input events. Most of them should trigger an action that is passed to action_q
    void handleStates();                ///< Sets correct values in inputStateTab based on pressed keys, buttons and other input
};

#endif  // INPUT_HANDLER_H