#pragma once

#include "gomoku_ai.h"

class Room
{
public:
    Room(int rows, int cols, std::vector<bool> arePlayersHuman, RuleStyle rule_style);
    ActionResult action(Entity player, Action action, ActionParameters params);
    ActionResult reverse_move();
    ActionResult reapply_move();

    std::pair<Entity, Action> get_next_action();
    std::vector<MoveHistory> get_list_moves();
    int get_current_move();
    bool get_is_player1_black();
    RuleStyle get_rule_style();
    GomokuGame get_game();

private:
    GomokuGame game;
    GomokuAI ai;
    std::vector<MoveHistory> list_moves;
    int currentMove;
    std::vector<bool> arePlayersHuman;
    RuleStyle rule_style;
    std::pair<Entity, Action> next_action;
    bool is_player1_black;
};