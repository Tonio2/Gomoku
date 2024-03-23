#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cassert>

#include "gomoku_ai.h"

#ifndef DEPTH
#define DEPTH 3 // Default depth value
#endif

extern std::vector<char> boardCoordinates;

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0);

void logMoveEvaluation(const MoveEvaluation &eval);

void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0);

void logTooManyEvaluationsList(const MoveEvaluation &eval);

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer);

std::vector<std::string> split(const std::string &str, char delimiter);

int getCoordinate(char coordinate);

void display_moves(std::vector<std::string> moves);

void apply_moves(GomokuGame &game, std::vector<std::string> moves);

void print_list(std::vector<double> list);

#endif