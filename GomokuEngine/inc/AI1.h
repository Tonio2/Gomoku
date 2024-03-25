#ifndef GOMOKU_AI_H
#define GOMOKU_AI_H

#include "gomoku_engine.h"
#include <limits>
#include <utility>
#include <random>

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
    int length;
    Player ai_player;
    Player human_player;

    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int pseudo_heuristic_evaluation(std::pair<int, int> move);

public:
    void sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer, int depth);

    int move_count;
    int move_evaluated_count;
    int evaluation_needed_count;
    GomokuAI(GomokuGame game, Player ai_player, int depth, int length = 2);
    MoveEvaluation suggest_move();
    int heuristic_evaluation();
};

#endif // GOMOKU_AI_H