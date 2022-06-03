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
    static constexpr float ANGLE_STEP_KEYBOARD = 0.1f;  // TODO: Change interpretation to something framerate-independent
    static constexpr float SFML_AXIS_INPUT_SCALE = 100;
    static constexpr float AXIS_DEADZONE = 0.2;

    const float UNIT_VECTOR_LENGTH = sqrt(2.f);

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q;

    bool isAnyJoystickConnected();
    void handleJoystickStates();
    void handleKeyboardState();
    b2Vec2 normalizeMovement(b2Vec2 movement);

   public:
    static const int PLAYER_COUNT_MAX = 4;
    static const int STATE_CONTROLS_PER_PLAYER = 3;  // movement (x, y, sums to max. 1) and angle the player should face in deg

    static const int INPUT_MOVEMENT = 0;
    static const int INPUT_LOOK_ANGLE = 1;

    // For XInput controller, default bindings may differ between gamepads
    // Named "JOYSTICK" after SFML convention
    static const int JOYSTICK_BUTTON_A = 0;
    static const int JOYSTICK_BUTTON_B = 1;
    static const int JOYSTICK_BUTTON_X = 2;
    static const int JOYSTICK_BUTTON_Y = 3;
    static const int JOYSTICK_BUTTON_LB = 4;
    static const int JOYSTICK_BUTTON_RB = 5;
    static const int JOYSTICK_BUTTON_LS = 6;  ///< Left stick press
    static const int JOYSTICK_BUTTON_RS = 7;  ///< Right stick press
    static const int JOYSTICK_BUTTON_L3 = 8;
    static const int JOYSTICK_BUTTON_R3 = 9;

    // This is set by InputHandler in every pass, and then checked by GameController each tick
    // contrary to Actions, that are sent into message queue only once, when they appear
    static std::array<std::tuple<b2Vec2, Angle>, PLAYER_COUNT_MAX> inputStateTab;

    InputHandler(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q);

    void handleEvent(sf::Event event);
    void handleStates();
};

#endif  // INPUT_HANDLER_H