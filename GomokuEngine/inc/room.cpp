#include "room.h"

Room::Room(int rows, int cols, std::vector<bool> arePlayersHuman, RuleStyle rule_style) : game(rows, cols), ai(3), list_moves(), current_move(0), arePlayersHuman(arePlayersHuman), rule_style(rule_style), next_action(Entity::PLAYER1, Action::PLAY), is_player1_black(true)
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
        // TODO: handle error : can there be an error ? how to handle it ?
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
        if (current_move == 0 && row != game.get_board_height() / 2 || col != game.get_board_width() / 2)
        {
          return {false, "First move must be in the center", next_action};
        }
        if (current_move == 2)
        {
          if (std::abs(row - list_moves[0].row) < 3 && std::abs(col - list_moves[0].col) < 3)
          {
            return {false, "Second move must be at least 3 cells away from the first move", next_action};
          }
        }
      }
    }

    MoveResult move_result = game.make_move(row, col);
    list_moves.push_back({row, col, move_result, next_action});
    current_move++;
    if (rule_style == RuleStyle::SWAP && current_move == 3)
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
    std::string message = "Player 2 takes white";
    if (arePlayersHuman[player] == false)
    {
      // TODO: Give depth as a parameter of suggest_move to update depth
      const MoveEvaluation move_evaluation = ai.suggest_move(game, player == Entity::PLAYER1 && is_player1_black ? Player::BLACK : Player::WHITE);
      // TODO: check if depth is odd
      if (move_evaluation.score < 0)
      {
        is_player1_black = false;
        message = "Player 2 takes black";
      }
    }
    else
    {
      if (params.team == Player::BLACK)
      {
        is_player1_black = false;
        message = "Player 2 takes black";
      }
    }
    return {true, message, {Entity::PLAYER2, Action::PLAY}};
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
  MoveHistory last_move = list_moves.back();
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
  MoveHistory last_move = list_moves[current_move];
  game.reapply_move(last_move.move_result);
  current_move++;
  next_action = last_move.action;
  return {true, "Move reapplied", next_action};
}

std::pair<Entity, Action> Room::get_next_action()
{
  return next_action;
}

std::vector<MoveHistory> Room::get_list_moves()
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
