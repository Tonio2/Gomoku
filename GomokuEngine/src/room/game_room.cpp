
#include "room/game_room.h"

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
    // TODO: Check if not expecting a swap action
    GameActionResult result;

    if (_game.is_game_over())
    {
        result.success = false;
        result.message = "The game is over, why are you even still here ?";
        return result;
    }

    if (gomoku_player_from_id(player) != _game.get_current_player())
    {
        result.success = false;
        result.message = "That's not your turn bro...";
        return result;
    }

    if (_settings.rule_style == GameRoomRuleStyle::PRO)
    {
        int midw = _game.get_board_width() / 2;
        int midh = _game.get_board_height() / 2;

        // If it's first turn
        if (_actions.empty())
        {
            if (row != midh || col != midw)
            {
                result.success = false;
                result.message = "Stone should be in the middle";
                return result;
            }
        }
        else if (_actions.size() == 1)
        {
            bool stone_too_far = abs(row - midh) > 3 || abs(col - midw) > 3;
            if (stone_too_far)
            {
                result.success = false;
                result.message = "Stone too far";
                return result;
            }
        }
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
        result.message = std::string(e.what());
        return result;
    }

    _actions.push_back(new_action);

    result.success = true;
    result.message = "Player " + std::to_string(player) + " played at " + std::to_string(row) + ";" + std::to_string(col);

    return result;
}

GameActionResult GameRoom::perform_action_swap(PlayerId player, bool do_the_swap)
{
    // TODO: implement it
    GameActionResult result;

    result.success = false;
    result.message = "I don't feel like it sorry...";
    return result;
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

        auto best_move = std::max_element(evaluation.listMoves.begin(), evaluation.listMoves.end(), [](const MoveEvaluation &a, const MoveEvaluation &b)
                                          { return a.score < b.score; });

        perform_action_move(current_player, best_move->move.first, best_move->move.second);
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
