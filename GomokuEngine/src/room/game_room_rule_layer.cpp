#include "room/game_room.h"
#include "utils/gomoku_utilities.h"

static GameActionResult make_action_result(bool success, const std::string &message)
{
    GameActionResult r;
    r.success = success;
    r.message = message;
    return r;
}

//  mmmm    m                      #                    #
// #"   " mm#mm   mmm   m mm    mmm#   mmm    m mm   mmm#
// "#mmm    #    "   #  #"  #  #" "#  "   #   #"  " #" "#
//     "#   #    m"""#  #   #  #   #  m"""#   #     #   #
// "mmm#"   "mm  "mm"#  #   #  "#m##  "mm"#   #     "#m##

GameActionResult GameRoom::GameRuleLayerStandard::perform_action_move(PlayerId player, int row, int col)
{
    if (_room._game.is_game_over())
    {
        return make_action_result(false, "The game is over, why are you even still here ?");
    }

    if (_room.gomoku_player_from_id(player) != _room._game.get_current_player())
    {
        return make_action_result(false, "That's not your turn bro...");
    }

    GameAction new_action;
    new_action.player = player;
    new_action.action_type = GameActionType::MOVE;
    new_action.action_value.move.row = row;
    new_action.action_value.move.col = col;

    try
    {
        new_action.action_value.move.result = _room._game.make_move(row, col);
    }
    catch (std::exception &e)
    {
        return make_action_result(false, e.what());
    }

    _room.append_action(new_action);

    return make_action_result(true, "Player " + std::to_string(player) + " played at " + std::to_string(row) + ";" + std::to_string(col));
}

GameActionResult GameRoom::GameRuleLayerStandard::perform_action_swap(PlayerId player, bool do_the_swap)
{
    return make_action_result(false, "Swap is not allowed in this game mode");
}

bool GameRoom::GameRuleLayerStandard::has_pending_action() const
{
    if (_room._game.is_game_over())
        return false;

    PlayerId current_player = _room.id_from_gomoku_player(_room._game.get_current_player());

    return _room.get_player_ai(current_player) != nullptr;
}

bool GameRoom::GameRuleLayerStandard::perform_pending_action()
{
    if (_room._game.is_game_over())
        return false;

    Player current_player = _room._game.get_current_player();
    PlayerId player_id = _room.id_from_gomoku_player(current_player);

    GomokuAI *ai = _room.get_player_ai(player_id);

    if (ai == nullptr)
        return false;

    MoveEvaluation evaluation = ai->suggest_move(_room._game, current_player);
    std::pair<int, int> best_move = getBestMove(evaluation);

    perform_action_move(player_id, best_move.first, best_move.second);
    return true;
}

PlayerId GameRoom::GameRuleLayerStandard::expected_player() const
{
    if (_room._game.is_game_over())
        return PlayerId(0);

    return _room.id_from_gomoku_player(_room._game.get_current_player());
}

GameActionType GameRoom::GameRuleLayerStandard::expected_action() const
{
    return GameActionType::MOVE;
}

// mmmmm
// #   "#  m mm   mmm
// #mmm#"  #"  " #" "#
// #       #     #   #
// #       #     "#m#"

GameActionResult GameRoom::GameRuleLayerPro::perform_action_move(PlayerId player, int row, int col)
{
    const int midw = _room._game.get_board_width() / 2;
    const int midh = _room._game.get_board_height() / 2;
    const bool stone_in_center = row == midh && col == midw;
    const bool stone_outside = abs(row - midh) > inner_square_radius || abs(col - midw) > inner_square_radius;

    switch (_room._action_index)
    {
    case -1:
        if (!stone_in_center)
        {
            return make_action_result(false, "Stone should be in the middle");
        }
        break;
    case 1:
        if (!stone_outside)
        {
            return make_action_result(false, "Stone should be outside the inner 5x5 square");
        }
        break;
    default:
        break;
    }

    return GameRuleLayerStandard::perform_action_move(player, row, col);
}

bool GameRoom::GameRuleLayerPro::has_pending_action() const
{
    if (_room._action_index == -1)
        return true;

    return GameRuleLayerStandard::has_pending_action();
}

bool GameRoom::GameRuleLayerPro::perform_pending_action()
{
    if (_room._action_index == -1)
    {
        perform_action_move(1, _room._game.get_board_height() / 2, _room._game.get_board_width() / 2);
        return true;
    }

    return GameRuleLayerStandard::perform_pending_action();
}

// m                                  mmmmm
// #       mmm   m mm    mmmm         #   "#  m mm   mmm
// #      #" "#  #"  #  #" "#         #mmm#"  #"  " #" "#
// #      #   #  #   #  #   #         #       #     #   #
// #mmmmm "#m#"  #   #  "#m"#         #       #     "#m#"
//                       m  #
//                        ""

//  mmmm
// #"   "m     m  mmm   mmmm
// "#mmm "m m m" "   #  #" "#
//     "# #m#m#  m"""#  #   #
// "mmm#"  # #   "mm"#  ##m#"
//                      #
//                      "

GameActionResult GameRoom::GameRuleLayerSwap::perform_action_move(PlayerId player, int row, int col)
{
    if (is_swap_expected())
    {
        return make_action_result(false, "Expected a swap action");
    }

    if (player == expected_player())
    {
        return GameRuleLayerStandard::perform_action_move(GameRuleLayerStandard::expected_player(), row, col);
    }

    return make_action_result(false, "Not your turn");
}

GameActionResult GameRoom::GameRuleLayerSwap::perform_action_swap(PlayerId player, bool do_the_swap)
{
    if (!is_swap_expected())
    {
        return make_action_result(false, "Did not expect a swap action");
    }

    if (player_expected_to_swap() != player)
    {
        return make_action_result(false, "Not your turn");
    }

    GameAction new_action;
    new_action.player = player;
    new_action.action_type = GameActionType::SWAP;
    new_action.action_value.swap.did_swap = do_the_swap;
    _room.append_action(new_action);

    _room._players_swapped = do_the_swap;

    return make_action_result(true, "Swap");
}

bool GameRoom::GameRuleLayerSwap::has_pending_action() const
{
    if (is_swap_expected() && _room.get_player_ai(player_expected_to_swap()) != nullptr)
        return true;

    return GameRuleLayerStandard::has_pending_action();
}

bool GameRoom::GameRuleLayerSwap::perform_pending_action()
{
    if (!is_swap_expected())
        return GameRuleLayerStandard::perform_pending_action();

    PlayerId player_id = player_expected_to_swap();
    GomokuAI *ai = _room.get_player_ai(player_id);
    if (ai)
    {
        perform_action_swap(player_expected_to_swap(), true); // TODO: choose if AI want to swap or not
        return true;
    }

    return false;
}

PlayerId GameRoom::GameRuleLayerSwap::expected_player() const
{
    if (is_swap_expected())
    {
        return player_expected_to_swap();
    }

    if (_room._action_index < 2)
    {
        return PlayerId(1);
    }

    return GameRuleLayerStandard::expected_player();
}

GameActionType GameRoom::GameRuleLayerSwap::expected_action() const
{
    if (is_swap_expected())
    {
        return GameActionType::SWAP;
    }
    return GameActionType::MOVE;
}

bool GameRoom::GameRuleLayerSwap::is_swap_expected() const
{
    if (_room._action_index == 2)
        return true;

    return false;
}

PlayerId GameRoom::GameRuleLayerSwap::player_expected_to_swap() const
{
    return PlayerId(2);
}

//  mmmm                        mmmm
// #"   "m     m  mmm   mmmm   "   "#
// "#mmm "m m m" "   #  #" "#      m"
//     "# #m#m#  m"""#  #   #    m"
// "mmm#"  # #   "mm"#  ##m#"  m#mmmm
//                      #
//                      "

PlayerId GameRoom::GameRuleLayerSwap2::expected_player() const
{
    return GameRuleLayerSwap::expected_player();
}

bool GameRoom::GameRuleLayerSwap2::is_swap_expected() const
{
    return GameRuleLayerSwap::is_swap_expected();
}

PlayerId GameRoom::GameRuleLayerSwap2::player_expected_to_swap() const
{
    return GameRuleLayerSwap::player_expected_to_swap();
}
