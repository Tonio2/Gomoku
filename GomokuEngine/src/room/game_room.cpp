
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
    // TODO: check if not expecting swap

    if (_game.is_game_over())
        return false;

    PlayerId current_player = id_from_gomoku_player(_game.get_current_player());

    if (get_player_ai(current_player))
        return true;

    return false;
}

void GameRoom::perform_pending_action()
{
    PlayerId current_player = id_from_gomoku_player(_game.get_current_player());

    GomokuAI *playing_ai = get_player_ai(current_player);

    if (playing_ai)
    {
        MoveEvaluation evaluation = playing_ai->suggest_move(_game, _game.get_current_player());

        auto best_move = getBestMove(evaluation);

        perform_action_move(current_player, best_move.first, best_move.second);
    }
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
    int midw = _game.get_board_width() / 2;
    int midh = _game.get_board_height() / 2;

    // If it's first turn
    if (_actions.empty())
    {
        if (row != midh || col != midw)
        {
            return make_action_result(false, "Stone should be in the middle");
        }
    }
    else if (_actions.size() == 1 || _actions.size() == 2)
    {
        bool stone_too_far = abs(row - midh) > 3 || abs(col - midw) > 3;
        if (stone_too_far)
        {
            return make_action_result(false, "Stone too far");
        }
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
    }

    return perform_action_move_rs_standard(player, row, col);
}
