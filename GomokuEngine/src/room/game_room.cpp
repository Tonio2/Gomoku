
#include "room/game_room.h"
#include "utils/gomoku_utilities.h"

#include <chrono>

static double get_current_timestamp()
{
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    return static_cast<double>(milliseconds) / 1000.0f;
}

GameRoom::GameRoom(const GameRoomSettings &settings)
    : _room_id(new_room_id()),
      _settings(settings),
      _game(settings.width, settings.height, settings.capture),
      _actions(),
      _action_index(-1),
      _players_swapped(false),
      _rule_layer(nullptr)
{
    switch (settings.rule_style)
    {
    case GameRoomRuleStyle::STANDARD:
        _rule_layer = new GameRuleLayerStandard(*this);
        break;
    case GameRoomRuleStyle::PRO:
        _rule_layer = new GameRuleLayerPro(*this);
        break;
    case GameRoomRuleStyle::LONG_PRO:
        _rule_layer = new GameRuleLayerLongPro(*this);
        break;
    case GameRoomRuleStyle::SWAP:
        _rule_layer = new GameRuleLayerSwap(*this);
        break;
    case GameRoomRuleStyle::SWAP2:
        _rule_layer = new GameRuleLayerSwap2(*this);
        break;
    default:
        _rule_layer = new GameRuleLayerStandard(*this);
    }
    _ai1 = settings.p1.make_ai();
    _ai2 = settings.p2.make_ai();
    const GameEntitySetting &setting = {
        true,
        "quiescent_d6",
    };
    _ai_helper = setting.make_ai();
}

GameRoom::~GameRoom()
{
    delete _rule_layer;
    if (_ai1)
        delete _ai1;
    if (_ai2)
        delete _ai2;
    if (_ai_helper)
        delete _ai_helper;
}

GameActionResult GameRoom::perform_action_move(PlayerId player, int row, int col)
{
    return _rule_layer->perform_action_move(player, row, col);
}

GameActionResult GameRoom::perform_action_swap(PlayerId player, bool do_the_swap)
{
    return _rule_layer->perform_action_swap(player, do_the_swap);
}

bool GameRoom::has_pending_action() const
{
    return _rule_layer->has_pending_action();
}

GameActionResult GameRoom::perform_pending_action()
{
    return _rule_layer->perform_pending_action();
}

PlayerId GameRoom::expected_player() const
{
    return _rule_layer->expected_player();
}

GameActionType GameRoom::expected_action() const
{
    return _rule_layer->expected_action();
}

double GameRoom::get_player_timer(PlayerId player) const
{
    double timer = 0;
    for (int i = 1; i <= _action_index; i++)
    {
        const GameAction &action = _actions[i];
        if (action.player == player)
        {
            const double diff = action.timestamp - _actions[i - 1].timestamp;
            timer += diff;
        }
    }
    return timer;
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

bool GameRoom::can_reverse_last_action() const
{
    return _action_index >= 0;
}

void GameRoom::reverse_last_action()
{
    if (!can_reverse_last_action())
        return;

    GameAction last_action = _actions[_action_index--];

    switch (last_action.action_type)
    {
    case GameActionType::MOVE:
        _game.reverse_move(last_action.action_value.move.result);
        break;
    case GameActionType::SWAP:
        if (last_action.action_value.swap.did_swap)
            _players_swapped = !_players_swapped;
        break;
    }
}

bool GameRoom::can_reapply_last_action() const
{
    return _action_index + 1 < _actions.size();
}

void GameRoom::reapply_last_action()
{
    if (!can_reapply_last_action())
        return;

    GameAction next_action = _actions[++_action_index];

    switch (next_action.action_type)
    {
    case GameActionType::MOVE:
        _game.reapply_move(next_action.action_value.move.result);
        break;
    case GameActionType::SWAP:
        if (next_action.action_value.swap.did_swap)
            _players_swapped = !_players_swapped;
        break;
    }
}

std::vector<AI::Move> GameRoom::suggest_move()
{
    return _ai_helper->suggest_move_sequence(_game);
}

int GameRoom::get_heuristic_evaluation()
{
    return _ai_helper->get_heuristic_evaluation(_game, gomoku_player_from_id(expected_player()));
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

AI::IGomokuAI *GameRoom::get_player_ai(PlayerId id) const
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

void GameRoom::append_action(GameAction &action)
{
    action.timestamp = get_current_timestamp();
    if (_action_index == -1)
        _first_action_timestamp = action.timestamp;
    action.timestamp -= _first_action_timestamp;

    _action_index++;
    _actions.resize(_action_index + 1);
    _actions[_action_index] = action;
}
