
#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai.h"

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
    bool is_ai;
    int ai_depth;
};

/** Settings of a room */
struct GameRoomSettings
{
    /** Width of the board */
    int width;
    /** Height of the board */
    int height;
    /** Rule styles */
    GameRoomRuleStyle rule_style;

    /** Is the first player an AI ? */
    GameEntitySetting p1;
    /** Is the second player an AI ? */
    GameEntitySetting p2;

    GameRoomSettings() : width(19), height(19), rule_style(GameRoomRuleStyle::STANDARD), p1({false, 0}), p2({true, 4})
    {
    }
};

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
    bool perform_pending_action();

    const std::vector<GameAction> &get_actions_history() { return _actions; }
    const GameRoomSettings &get_settings() const { return _settings; }
    const GomokuGame &get_game() const { return _game; }
    int get_color_score(Player color) const { return _game.get_player_score(color); }

    Player gomoku_player_from_id(PlayerId id) const;
    PlayerId id_from_gomoku_player(Player player) const;

private:
    std::string _room_id;
    GameRoomSettings _settings;
    GomokuGame _game;
    /** History of actions performed during the game */
    std::vector<GameAction> _actions;
    bool _players_swapped;

    GomokuAI *_ai1 = nullptr;
    GomokuAI *_ai2 = nullptr;

    static std::string new_room_id();

    GomokuAI *get_player_ai(PlayerId id) const;

    bool is_swap_expected() const;
    PlayerId player_expected_to_swap() const;

    GameActionResult perform_action_move_rs_standard(PlayerId player, int row, int col);
    GameActionResult perform_action_move_rs_pro(PlayerId player, int row, int col);
    GameActionResult perform_action_move_rs_swap(PlayerId player, int row, int col);
};
