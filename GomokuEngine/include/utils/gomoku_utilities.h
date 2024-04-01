
#pragma once

#include "engine/gomoku_engine.h"

std::vector<std::string> split(const std::string &str, char delimiter);

int char_to_coordinate(char coordinate);

void apply_moves(GomokuGame &game, std::vector<std::string> moves);
void apply_moves(GomokuGame &game, std::string move_str);
