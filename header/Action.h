/// A simple object to pass information between InputHandler and GameController
class Action {
    int playerId;
    int type;

   public:
    // TODO: Replace with an enum
    static const int TYPE_NONE = 0;
    static const int TYPE_DEBUG = 1;
    static const int TYPE_PICK_LEFT = 2;
    static const int TYPE_DROP_LEFT = 3;
    static const int TYPE_PICK_RIGHT = 4;
    static const int TYPE_DROP_RIGHT = 5;
    static const int TYPE_ACT_LEFT = 6;
    static const int TYPE_ACT_RIGHT = 7;
    static const int TYPE_ACT_PREP_LEFT = 8;
    static const int TYPE_ACT_PREP_RIGHT = 9;

    Action()
        : playerId(-1), type(TYPE_NONE){};
    Action(int player, int t)
        : playerId(player), type(t){};

    int getPlayerId() const { return playerId; };
    int getType() const { return type; };
};