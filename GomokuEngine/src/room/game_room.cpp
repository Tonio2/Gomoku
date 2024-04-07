
#include "room/game_room.h"
#include "utils/gomoku_utilities.h"

static GameActionResult make_action_result(bool success, const std::string& message)
{
    GameActionResult r;
    r.success = success;
    r.message = message;
    return r;
}

GameRoom::GameRoom(const GameRoomSettings &settings)
    : _room_id(new_room_id()),
      _settings(settings),
      _game(settings.width, settings.height),
      _actions(),
      _players_swapped(false)
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
    switch (_settings.rule_style)
    {
    case ::PRO:
        return perform_action_move_rs_pro(player, row, col);
    case ::SWAP:
        return perform_action_move_rs_swap(player, row, col);
    default:
        return perform_action_move_rs_standard(player, row, col);
    }
}

GameActionResult GameRoom::perform_action_swap(PlayerId player, bool do_the_swap)
{
    if (!is_swap_expected())
    {
        return make_action_result(false, "Did not expect a swap action");
    }

    if (player_expected_to_swap() != player)
    {
        return make_action_result(false, "Wrong player");
    }

    GameAction new_action;
    new_action.player = player;
    new_action.action_type = GameActionType::GAMEACTION_SWAP;
    new_action.action_value.swap.did_swap = do_the_swap;
    _actions.push_back(new_action);

    _players_swapped = do_the_swap;

    return make_action_result(true, "Swap successful");
}

bool GameRoom::has_pending_action() const
{
    if (_game.is_game_over())
        return false;

    if (is_swap_expected() && get_player_ai(player_expected_to_swap()) != nullptr)
        return true;

    if (_actions.empty() && _settings.rule_style == ::PRO)
        return true;

    PlayerId current_player = id_from_gomoku_player(_game.get_current_player());

    return get_player_ai(current_player) != nullptr;
}

bool GameRoom::perform_pending_action()
{
    if (is_swap_expected())
    {
        PlayerId p = player_expected_to_swap();
        GomokuAI* ai = get_player_ai(p);

        if (ai == nullptr)
            return false;

        perform_action_swap(p, true); // TODO: choose if AI want to swap or not

        return true;
    }

    if (_settings.rule_style == ::PRO && _actions.empty())
    {
        perform_action_move(1, _game.get_board_height() / 2, _game.get_board_width() / 2);
        return true;
    }

    PlayerId current_player = id_from_gomoku_player(_game.get_current_player());

    GomokuAI *ai = get_player_ai(current_player);

    if (ai == nullptr)
        return false;

    MoveEvaluation evaluation = ai->suggest_move(_game, _game.get_current_player());
    std::pair<int, int> best_move = getBestMove(evaluation);

    perform_action_move(current_player, best_move.first, best_move.second);
    return true;
}

std::string GameRoom::new_room_id()
{
    std::string new_id;

    const std::string chars("abcdef0123456789");

    static int count = 0;
    count++;

    for (int i = 0; i < 5; i++)
    {
        new_id += chars[random() % chars.size()];
    }
    new_id += chars[count % chars.size()];
    return new_id;
}

Player GameRoom::gomoku_player_from_id(PlayerId id) const
{
    if (_players_swapped)
    {
        if (id > 0)
            return Player(3 - id);
    }
    return Player(id);
}

PlayerId GameRoom::id_from_gomoku_player(Player player) const
{
    if (_players_swapped)
    {
        if (player != E)
            return PlayerId(3 - player);
    }
    return PlayerId(player);
}

GomokuAI *GameRoom::get_player_ai(PlayerId id) const
{
    switch (id)
    {
    case 1:
        return _ai1;
    case 2:
        return _ai2;
    }
    return nullptr;
}

bool GameRoom::is_swap_expected() const
{
    if (_settings.rule_style == ::SWAP && _actions.size() == 3)
        return true;

    return false;
}

PlayerId GameRoom::player_expected_to_swap() const
{
    return PlayerId(2);
}

GameActionResult GameRoom::perform_action_move_rs_standard(PlayerId player, int row, int col)
{
    if (_game.is_game_over())
    {
        return make_action_result(false, "The game is over, why are you even still here ?");
    }

    if (gomoku_player_from_id(player) != _game.get_current_player())
    {
        return make_action_result(false, "That's not your turn bro...");
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
        return make_action_result(false, e.what());
    }

    _actions.push_back(new_action);

    return make_action_result(true, "Player " + std::to_string(player) + " played at " + std::to_string(row) + ";" + std::to_string(col));
}

GameActionResult GameRoom::perform_action_move_rs_pro(PlayerId player, int row, int col)
{
    const int midw = _game.get_board_width() / 2;
    const int midh = _game.get_board_height() / 2;
    const bool stone_in_center = row == midh && col == midw;
    const bool stone_outside = abs(row - midh) > 3 || abs(col - midw) > 3;

    switch (_actions.size())
    {
    case 0:
        if (!stone_in_center)
        {
            return make_action_result(false, "Stone should be in the middle");
        }
        break;
    case 2:
        if (!stone_outside)
        {
            return make_action_result(false, "Stone should be outside the inner circle");
        }
        break;
    default:
        break;
    }

    return perform_action_move_rs_standard(player, row, col);
}

GameActionResult GameRoom::perform_action_move_rs_swap(PlayerId player, int row, int col)
{
    if (is_swap_expected())
    {
        return make_action_result(false, "Expected a swap action");
    }

    if (_actions.size() < 3)
    {
        if (player == 2)
        {
            return make_action_result(false, "Expected a swap action");
        }
        PlayerId current_player = id_from_gomoku_player(_game.get_current_player());
        return perform_action_move_rs_standard(current_player, row, col);
    }

    return perform_action_move_rs_standard(player, row, col);
}
