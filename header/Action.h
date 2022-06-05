#ifndef ACTION_H
#define ACTION_H
/// A simple object to pass information between InputHandler and GameController
struct Action {
    enum Type {
        NONE,
        DEBUG,
        RESTART,
        PICK_LEFT,
        DROP_LEFT,
        PICK_RIGHT,
        DROP_RIGHT,
        ACT_LEFT,
        ACT_RIGHT,
        ACT_PREP_LEFT,
        ACT_PREP_RIGHT,
        SWITCH_HANDS
    };

    int playerId = -1;
    Type type = NONE;
};

#endif  // ACTION_H