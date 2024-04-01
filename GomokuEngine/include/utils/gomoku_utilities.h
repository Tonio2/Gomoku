
#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai.h"

std::vector<std::string> split(const std::string &str, char delimiter);

int char_to_coordinate(char coordinate);
char coordinate_to_char(int coordinate);

void apply_moves(GomokuGame &game, std::vector<std::string> moves);
void apply_moves(GomokuGame &game, std::string move_str);

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0);

void logMoveEvaluation(const MoveEvaluation &eval);

void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0);

void logTooManyEvaluationsList(const MoveEvaluation &eval);

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer = true);
