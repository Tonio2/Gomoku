
#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai_minmaxv2.h"
#include "ai/gomoku_ai_minmaxv3.h"

std::vector<std::string> split(const std::string &str, char delimiter);

int char_to_coordinate(char coordinate);
char coordinate_to_char(int coordinate);

std::string to_string(const GomokuGame &game, bool with_coordinates = false, int empty_cell_structure_distance = -1);

void apply_moves(GomokuGame &game, std::vector<std::string> moves);
void apply_moves(GomokuGame &game, std::string move_str);

int get_depth_from_env(int default_depth = 3);

void writeMoveEvaluation(std::ostream &out, const AI::MinMaxV2::MoveEvaluation &eval, std::vector<std::vector<std::string>> &csvData, int depth = 0, int i = -1);

void logMoveEvaluation(const AI::MinMaxV2::MoveEvaluation &eval, std::string filename);
void writeSurplusEvaluation(std::ofstream &out, const AI::MinMaxV2::MoveEvaluation &eval, int depth = 0);
void logTooManyEvaluationsList(const AI::MinMaxV2::MoveEvaluation &eval);
int getBestMoveIndexV2(const AI::MinMaxV2::MoveEvaluation &eval, bool maximizingPlayer = true);
std::pair<int, int> getBestMove(const AI::MinMaxV2::MoveEvaluation &eval, bool maximizingPlayer = true);
AI::MinMaxV2::MoveEvaluation &getBestMoveEvaluation(AI::MinMaxV2::MoveEvaluation &eval, bool maximizingPlayer = true);

void logMoveEvaluation(const AI::MinMaxV3::MoveEvaluation &eval, std::string filename);
void writeSurplusEvaluation(std::ofstream &out, const AI::MinMaxV3::MoveEvaluation &eval, int depth = 0);
void logTooManyEvaluationsList(const AI::MinMaxV3::MoveEvaluation &eval);
int getBestMoveIndexV3(const AI::MinMaxV3::MoveEvaluation &eval, bool maximizingPlayer = true);
std::pair<int, int> getBestMove(const AI::MinMaxV3::MoveEvaluation &eval, bool maximizingPlayer = true);
AI::MinMaxV3::MoveEvaluation &getBestMoveEvaluation(AI::MinMaxV3::MoveEvaluation &eval, bool maximizingPlayer = true);

std::ostream &operator<<(std::ostream &stream, std::vector<std::string> moves);

std::ostream &operator<<(std::ostream &stream, std::vector<Player> structure);
