/// A simple object to pass information between InputHandler and GameController
class Action {
    int playerId;
    int type;

    public:
    // TODO: Replace with an enum
    static const int TYPE_NONE = 0;
    static const int TYPE_DEBUG = 1;
    static const int TYPE_PICK_LEFT = 2;

    Action() : playerId(-1), type(TYPE_NONE) {};
    Action(int player, int t) : playerId(player), type(t) {};

    int getPlayerId() const { return playerId; };
    int getType() const { return type; };
};