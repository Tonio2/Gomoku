#ifndef GOMOKU_AI_H
#define GOMOKU_AI_H

#include "gomoku_engine.h"
#include <limits>
#include <utility>
#include <random>

struct MoveEvaluation {
    std::pair<int, int> move;
    int score;
    int neededEvalCount;
    int totalEvalCount;
    int evaluatedMoves;
    std::vector<MoveEvaluation> listMoves;
};

class GomokuAI {
private:
    GomokuGame game;
    int depth;
    Player ai_player;
    Player human_player;

    void sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer, int depth);
    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int heuristic_evaluation();
    int pseudo_heuristic_evaluation(std::pair<int, int> move);

public:
    GomokuAI(GomokuGame game, Player ai_player, int depth);
    MoveEvaluation suggest_move();
};

#endif // GOMOKU_AI_H