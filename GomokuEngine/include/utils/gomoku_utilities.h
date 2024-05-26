
#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai_minmaxv2.h"

std::vector<std::string> split(const std::string &str, char delimiter);

int char_to_coordinate(char coordinate);
char coordinate_to_char(int coordinate);

std::string to_string(const GomokuGame &game, bool with_coordinates = false, int empty_cell_structure_distance = -1);

void apply_moves(GomokuGame &game, std::vector<std::string> moves);
void apply_moves(GomokuGame &game, std::string move_str);

int get_depth_from_env(int default_depth = 3);

void writeMoveEvaluation(std::ostream &out, const AI::MoveEvaluation &eval, std::vector<std::vector<std::string>> &csvData, int depth = 0, int i = -1);

void logMoveEvaluation(const AI::MoveEvaluation &eval, std::string filename);

void writeSurplusEvaluation(std::ofstream &out, const AI::MoveEvaluation &eval, int depth = 0);

void logTooManyEvaluationsList(const AI::MoveEvaluation &eval);

int getBestMoveIndex(const AI::MoveEvaluation &eval, bool maximizingPlayer = true);
std::pair<int, int> getBestMove(const AI::MoveEvaluation &eval, bool maximizingPlayer = true);
AI::MoveEvaluation &getBestMoveEvaluation(AI::MoveEvaluation &eval, bool maximizingPlayer = true);

std::ostream &operator<<(std::ostream &stream, std::vector<std::string> moves);

std::ostream &operator<<(std::ostream &stream, std::vector<Player> structure);
