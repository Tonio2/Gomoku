#include "room.h"

Room::Room(int rows, int cols, std::vector<bool> arePlayersHuman, RuleStyle rule_style) : game(rows, cols), ai(3), list_moves(), currentMove(0), arePlayersHuman(arePlayersHuman), rule_style(rule_style), next_action(Entity::PLAYER1, Action::PLAY), is_player1_black(true)
{
}

ActionResult Room::action(Entity player, Action action, ActionParameters params)
{
  if (player != next_action.first || action != next_action.second)
  {
    return {false, "Not your turn", next_action};
  }
  if (action == Action::PLAY)
  {
    int row;
    int col;
    if (arePlayersHuman[player] == false)
    {
      const MoveEvaluation move_evaluation = ai.suggest_move(game, player == Entity::PLAYER1 && is_player1_black ? Player::BLACK : Player::WHITE);
      auto bestMoveIter = std::max_element(move_evaluation.listMoves.begin(), move_evaluation.listMoves.end(), [](const MoveEvaluation &a, const MoveEvaluation &b)
                                           { return a.score < b.score; });
      if (bestMoveIter != move_evaluation.listMoves.end())
      {
        MoveEvaluation bestMove = *bestMoveIter;
        row = bestMove.move.first;
        col = bestMove.move.second;
      }
      else
      {
        // TODO: handle error
      }
    }
    else
    {
      row = params.row;
      col = params.col;
      if (!game.coordinates_are_valid(row, col))
      {
        return {false, "Invalid coordinates", next_action}; // TODO: remove invalid coordinates check from game_engine
      }
      if (rule_style == RuleStyle::PRO)
      {
        if (currentMove == 0 && row != game.get_board_height() / 2 || col != game.get_board_width() / 2)
        {
          return {false, "First move must be in the center", next_action};
        }
        if (currentMove == 2)
        {
          if (std::abs(row - list_moves[0].row) < 3 && std::abs(col - list_moves[0].col) < 3)
          {
            return {false, "Second move must be at least 3 cells away from the first move", next_action};
          }
        }
      }
    }

    MoveResult move_result = game.make_move(row, col);
    list_moves.push_back({row, col, move_result});
    currentMove++;
    if (rule_style == RuleStyle::SWAP && currentMove == 3)
    {
      next_action = {Entity::PLAYER2, Action::SWAP_CHOICE};
    }
    else
    {
      next_action = {next_action.first == Entity::PLAYER1 ? Entity::PLAYER2 : Entity::PLAYER1, Action::PLAY};
    }
    return {true, "Move successful", next_action};
  }
  else if (action == Action::SWAP_CHOICE)
  {
    if (arePlayersHuman[player] == false)
    {
      const MoveEvaluation MoveEvaluation = ai.suggest_move(game, player == Entity::PLAYER1 && is_player1_black ? Player::BLACK : Player::WHITE);
    }
    if (params.team == Player::BLACK)
    {
      if (player == Entity::PLAYER1)
      {
        is_player1_black = true;
      }
      else
      {
        is_player1_black = false;
      }
    }
    else
    {
      if (player == Entity::PLAYER1)
      {
        is_player1_black = false;
      }
      else
      {
        is_player1_black = true;
      }
    }
  }
}
