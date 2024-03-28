#include "room.h"

Room::Room(int rows, int cols, RuleStyle rule_style) : game(rows, cols), list_moves(), current_move(0), rule_style(rule_style), next_action(Entity::PLAYER1, Action::PLAY), is_player1_black(true)
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
      next_action = {Entity::PLAYER2, Action::SWAP_CHOICE};
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
    return {true, message, {is_player1_black ? Entity::PLAYER2 : Entity::PLAYER1, Action::PLAY}};
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

Entity Room::get_other_player(Entity player)
{
  return player == Entity::PLAYER1 ? Entity::PLAYER2 : Entity::PLAYER1;
}
