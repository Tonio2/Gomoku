#pragma once

#include "gomoku_ai.h"

typedef uint8_t PlayerId;

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
union GameActionValue
{
    GameActionValue_Move move;
    GameActionValue_Swap swap;
};

/** All possible types of GameActions */
enum GameActionType
{
    GAMEACTION_MOVE,
    GAMEACTION_SWAP,
};

/** An action performed on a room. The room keeps a list of those as history */
struct GameAction
{
    PlayerId player;

    GameActionType action_type;
    GameActionValue action_value;
};

/** Different rule style sets */
enum GameRoomRuleStyle
{
    STANDARD,
    PRO,
    SWAP,
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
    /** Is the second play an AI ? */
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

    /** Make the room perform an action that will return a result */
    GameActionResult perform_action_move(PlayerId player, int row, int col);
    GameActionResult perform_action_swap(PlayerId player, bool do_the_swap);

    /** Check if the room need to perform pending actions */
    bool has_pending_action() const;
    /** Ask the room to perform the next pending action and return if there are other pending actions left. We should use this */
    void perform_pending_action();

    const std::vector<GameAction> &get_actions_history();
    const GameRoomSettings &get_settings() const;
    const GomokuGame &get_game() const;

private:
    GameRoomSettings _settings;
    GomokuGame _game;
    /** History of actions performed during the game */
    std::vector<GameAction> _actions;

    GomokuAI *_ai1 = nullptr;
    GomokuAI *_ai2 = nullptr;

    PlayerId get_next_player(PlayerId player);
    Player gomoku_player(PlayerId player);
};
