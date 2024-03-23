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

struct MoveHeuristic
{
    uint8_t row;
    uint8_t col;

    int score;
};

class GomokuAI
{
private:
    GomokuGame game;
    int depth;
    Player ai_player;
    Player human_player;

    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int pseudo_heuristic_evaluation(std::pair<int, int> move);

    std::vector<MoveHeuristic> find_relevant_moves() const;

public:
    void sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth);

    int move_count;
    int move_evaluated_count;
    int evaluation_needed_count;
    GomokuAI(GomokuGame game, Player ai_player, int depth);
    MoveEvaluation suggest_move();
    int heuristic_evaluation();
};

#endif // GOMOKU_AI_H