#ifndef GOMOKU_AI_H
#define GOMOKU_AI_H

#include "gomoku_engine.h"
#include <limits>
#include <utility>
#include <random>
#include <thread>
#include <vector>
#include <limits>
#include <exception>

struct MoveEvaluation
{
    std::pair<int, int> move;
    int score;
    int neededEvalCount;
    int totalEvalCount;
    int evaluatedMoves;
    std::vector<MoveEvaluation> listMoves;
};

class GomokuAI
{
private:
    GomokuGame game;
    int depth;
    Player ai_player;
    Player human_player;

    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    void evaluateMove(std::pair<std::pair<int, int>, int> &move, GomokuGame game, bool maximizingPlayer);

public:
    void sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer);
    GomokuAI(GomokuGame game, Player ai_player, int depth);
    MoveEvaluation suggest_move();
    int heuristic_evaluation(GomokuGame &game);
};

#endif // GOMOKU_AI_H