#pragma once

#include "gomoku_engine.h"
#include "gomoku_ai_data.h"
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
    GomokuAIData evaluation_data;

    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int pseudo_heuristic_evaluation(std::pair<int, int> move);
    int score_player(Player player);

    std::vector<MoveHeuristic> find_relevant_moves() const;

public:
    void sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth);

    int move_count;
    int move_evaluated_count;
    int evaluation_needed_count;
    GomokuAI(int depth, GomokuAIData data = GomokuAIData());
    MoveEvaluation suggest_move(const GomokuGame &board, Player player); // TODO: SHould't we make a copy ? What if suggest_move is interrupted or something?
    int heuristic_evaluation(const GomokuGame &board, Player heuristic_player);

    const GomokuAIData &get_evaluation_data() const;
};
