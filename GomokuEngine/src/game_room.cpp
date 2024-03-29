#include "game_room.h"

GameRoom::GameRoom(const GameRoomSettings &settings)
    : _settings(settings),
      _game(settings.width, settings.height),
      _actions()
{
    if (settings.p1.is_ai)
        _ai1 = new GomokuAI(settings.p1.ai_depth);
    if (settings.p2.is_ai)
        _ai2 = new GomokuAI(settings.p2.ai_depth);
}

GameRoom::~GameRoom()
{
    if (_ai1)
        delete _ai1;
    if (_ai2)
        delete _ai2;
}

GameActionResult GameRoom::perform_action_move(PlayerId player, int row, int col)
{
    GameActionResult result;

    if (gomoku_player(player) != _game.get_current_player())
    {
        result.success = false;
        result.message = "That's not your turn bro...";
        return result;
    }

    GameAction new_action;
    new_action.player = player;
    new_action.action_type = GameActionType::GAMEACTION_MOVE;
    new_action.action_value.move.row = row;
    new_action.action_value.move.col = col;

    try
    {
        new_action.action_value.move.result = _game.make_move(row, col);
    }
    catch (std::exception &e)
    {
        result.success = false;
        result.message = e.what();
        return result;
    }

    _actions.push_back(new_action);

    result.success = true;
    result.message = "Player " + std::to_string(player) + " played at " + std::to_string(row) + ";" + std::to_string(col);

    return result;
}

GameActionResult GameRoom::perform_action_swap(PlayerId player, bool do_the_swap)
{
    GameActionResult result;

    result.success = false;
    result.message = "I don't feel like it sorry...";
    return result;
}

bool GameRoom::has_pending_action() const
{
}
ActionResult Room::action(PlayerId player, Action action, ActionParameters params)
{
    if (player != next_action.first || action != next_action.second)
    {
        return {false, "Not your turn", next_action};
    }
    if (action == Action::PLAY)
    {
        if (!game.coordinates_are_valid(params.row, params.col))
        {
            return {false, "Invalid coordinates", next_action}; // TODO: remove invalid coordinates check from game_engine
        }
        if (rule_style == RuleStyle::PRO)
        {
            if (current_move == 0 && params.row != game.get_board_height() / 2 || params.col != game.get_board_width() / 2)
            {
                return {false, "First move must be in the center", next_action};
            }
            if (current_move == 2)
            {
                if (std::abs(params.row - list_moves[0].row) < 3 && std::abs(params.col - list_moves[0].col) < 3)
                {
                    return {false, "Second move must be at least 3 cells away from the first move", next_action};
                }
            }
        }

        MoveResult move_result = game.make_move(params.row, params.col);
        list_moves.push_back({params.row, params.col, move_result, next_action});
        current_move++;
        if (rule_style == RuleStyle::SWAP && current_move == 3)
        {
            next_action = {2, Action::SWAP_CHOICE};
        }
        else
        {
            next_action = {get_other_player(next_action.first), Action::PLAY};
        }
        return {true, "Move successful", next_action};
    }
    else if (action == Action::SWAP_CHOICE)
    {
        std::string message = "Player 2 takes white";
        if (params.team == Player::BLACK)
        {
            is_player1_black = false;
            message = "Player 2 takes black";
        }
        return {true, message, {is_player1_black ? 2 : 1, Action::PLAY}};
    }
}

ActionResult Room::reverse_move()
{
    if (current_move == 0)
    {
        return {false, "No moves to reverse", next_action};
    }
    if (rule_style == RuleStyle::SWAP && current_move == 3)
    {
        return {false, "Cannot reverse swap choice", next_action};
    }
    GameAction last_move = list_moves.back();
    game.reverse_move(last_move.move_result);
    current_move--;
    next_action = last_move.action;
    return {true, "Move reversed", next_action};
}

ActionResult Room::reapply_move()
{
    if (current_move == list_moves.size())
    {
        return {false, "No moves to reapply", next_action};
    }
    GameAction last_move = list_moves[current_move];
    game.reapply_move(last_move.move_result);
    current_move++;
    next_action = last_move.action;
    return {true, "Move reapplied", next_action};
}

std::pair<PlayerId, Action> Room::get_next_action()
{
    return next_action;
}

std::vector<GameAction> Room::get_list_moves()
{
    return list_moves;
}

int Room::get_current_move()
{
    return current_move;
}

bool Room::get_is_player1_black()
{
    return is_player1_black;
}

RuleStyle Room::get_rule_style()
{
    return rule_style;
}

GomokuGame Room::get_game()
{
    return game;
}

PlayerId Room::get_other_player(PlayerId player)
{
    return player == 1 ? 2 : 1;
}
