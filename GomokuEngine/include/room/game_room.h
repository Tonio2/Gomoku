
#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai_interface.h"

/** Identifier attributed to each players. The player id will never change.
 * The case of swap will only change the player's corresponding gomoku player */
typedef uint8_t PlayerId;

/** All possible types of GameActions */
enum class GameActionType
{
    MOVE,
    SWAP,
};

/** Values of an action of type Move */
struct GameActionValue_Move
{
    int8_t row;
    int8_t col;
    MoveResult result;
};

/** Values of an action of type Swap */
struct GameActionValue_Swap
{
    bool did_swap;
};

/** Value of an action. The action goes along a GameActionType to determine wich value of the union is relevant. */
struct GameActionValue
{
    GameActionValue_Move move;
    GameActionValue_Swap swap;
};

/** An action performed on a room. The room keeps a list of those as history */
struct GameAction
{
    PlayerId player;
    double timestamp;

    GameActionType action_type;
    GameActionValue action_value;
};

/** Different rule style sets */
enum class GameRoomRuleStyle
{
    STANDARD = 0,
    PRO = 1,
    LONG_PRO = 2,
    SWAP = 3,
    SWAP2 = 4,
};

/** Result after performing an action. If it's not a success, the action is not added to the history. */
struct GameActionResult
{
    bool success;
    std::string message;
};

struct GameEntitySetting
{
    bool is_ai = false;
    std::string ai_name = "medium";

    AI::IGomokuAI *make_ai() const;
};

/** Settings of a room */
struct GameRoomSettings
{
    /** Width of the board */
    int width = 19;
    /** Height of the board */
    int height = 19;
    /** Rule styles */
    GameRoomRuleStyle rule_style = GameRoomRuleStyle::STANDARD;
    /** Toggle capture */
    bool capture = true;

    /** Is the first player an AI ? */
    GameEntitySetting p1;
    /** Is the second player an AI ? */
    GameEntitySetting p2 = {true, "default"};
};

std::vector<std::string> get_ai_names_list();

class GameRoom
{
public:
    GameRoom(const GameRoomSettings &settings);
    virtual ~GameRoom();

    std::string get_id() const { return _room_id; }

    /** Try to make the room perform a move action */
    GameActionResult perform_action_move(PlayerId player, int row, int col);
    /** Try to make the room perform a swap action */
    GameActionResult perform_action_swap(PlayerId player, bool do_the_swap);

    /** Check if the room need to perform pending actions */
    bool has_pending_action() const;
    /** Ask the room to perform the next pending action and return if there are other pending actions left. We should use this */
    GameActionResult perform_pending_action();

    PlayerId expected_player() const;
    GameActionType expected_action() const;

    const std::vector<GameAction> &get_actions_history() { return _actions; }
    int get_action_index() const { return _action_index; }
    const GameRoomSettings &get_settings() const { return _settings; }
    const GomokuGame &get_game() const { return _game; }
    int get_color_score(Player color) const { return _game.get_player_score(color); }
    double get_player_timer(PlayerId player) const;

    Player gomoku_player_from_id(PlayerId id) const;
    PlayerId id_from_gomoku_player(Player player) const;

    bool can_reverse_last_action() const;
    void reverse_last_action();

    bool can_reapply_last_action() const;
    void reapply_last_action();

    std::vector<AI::Move> suggest_move();
    int get_heuristic_evaluation();

private:
    std::string _room_id;
    GameRoomSettings _settings;
    GomokuGame _game;
    /** History of actions performed during the game */
    std::vector<GameAction> _actions;
    int _action_index;
    bool _players_swapped;

    AI::IGomokuAI *_ai1 = nullptr;
    AI::IGomokuAI *_ai2 = nullptr;
    AI::IGomokuAI *_ai_helper = nullptr;

    double _first_action_timestamp = 0;

    static std::string new_room_id();

    AI::IGomokuAI *get_player_ai(PlayerId id) const;

    void append_action(GameAction &action);

    class IGameRuleLayer
    {
    public:
        IGameRuleLayer(GameRoom &room) : _room(room) {}
        virtual ~IGameRuleLayer() {}

        virtual GameActionResult perform_action_move(PlayerId player, int row, int col) = 0;
        virtual GameActionResult perform_action_swap(PlayerId player, bool do_the_swap) = 0;

        virtual bool has_pending_action() const = 0;
        virtual GameActionResult perform_pending_action() = 0;

        virtual PlayerId expected_player() const = 0;
        virtual GameActionType expected_action() const = 0;

    protected:
        GameRoom &_room;
    };

    class GameRuleLayerStandard : public IGameRuleLayer
    {
    public:
        GameRuleLayerStandard(GameRoom &room) : IGameRuleLayer(room) {}

        virtual GameActionResult perform_action_move(PlayerId player, int row, int col) override;
        virtual GameActionResult perform_action_swap(PlayerId player, bool do_the_swap) override;

        virtual bool has_pending_action() const override;
        virtual GameActionResult perform_pending_action() override;

        virtual PlayerId expected_player() const override;
        virtual GameActionType expected_action() const override;
    };

    class GameRuleLayerPro : public GameRuleLayerStandard
    {
    public:
        GameRuleLayerPro(GameRoom &room) : GameRuleLayerStandard(room), inner_square_radius(2) {}

        virtual GameActionResult perform_action_move(PlayerId player, int row, int col) override;

        virtual bool has_pending_action() const override;
        virtual GameActionResult perform_pending_action() override;

    protected:
        int inner_square_radius;
    };

    class GameRuleLayerLongPro : public GameRuleLayerPro
    {
    public:
        GameRuleLayerLongPro(GameRoom &room) : GameRuleLayerPro(room)
        {
            inner_square_radius = 3;
        }
    };

    class GameRuleLayerSwap : public GameRuleLayerStandard
    {
    public:
        GameRuleLayerSwap(GameRoom &room) : GameRuleLayerStandard(room) {}

        virtual GameActionResult perform_action_move(PlayerId player, int row, int col) override;
        virtual GameActionResult perform_action_swap(PlayerId player, bool do_the_swap) override;

        virtual GameActionResult perform_pending_action() override;

        virtual PlayerId expected_player() const override;
        virtual GameActionType expected_action() const override;

        virtual bool is_swap_expected() const;
        virtual PlayerId player_expected_to_swap() const;
    };

    class GameRuleLayerSwap2 : public GameRuleLayerSwap
    {
    public:
        GameRuleLayerSwap2(GameRoom &room) : GameRuleLayerSwap(room) {}

        virtual PlayerId expected_player() const override;

        virtual bool is_swap_expected() const override;
        virtual PlayerId player_expected_to_swap() const override;
    };

    friend GameRuleLayerStandard;
    friend GameRuleLayerPro;
    friend GameRuleLayerLongPro;
    friend GameRuleLayerSwap;
    friend GameRuleLayerSwap2;

    IGameRuleLayer *_rule_layer;
};
