#pragma once

#include "gomoku_ai.h"

class Room
{
public:
    Room(int rows, int cols, std::vector<bool> arePlayersHuman, RuleStyle rule_style);
    std::string get_id();
    std::pair<Player, Action> get_action();
    ActionResult action(Player player, Action action, std::vector<int> coordinates);

private:
    GomokuGame game;
    std::vector<bool> arePlayersHuman;
    RuleStyle rule_style;
    std::pair<Player, Action> next_action;
    std::string room_id;
    std::vector<Team> teams;
};