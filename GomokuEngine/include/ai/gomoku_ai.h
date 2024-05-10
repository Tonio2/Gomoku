
#pragma once

#include "engine/gomoku_engine.h"
#include "gomoku_ai_data.h"

struct GomokuAiSettings
{
    int depth = 4;
    int length = 2;
    GomokuAIData data = GomokuAIData();
};

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
    int depth;  // min max depth algorithm
    int length; // evaluated moves distance from structures
    Player ai_player;
    Player human_player;
    GomokuAIData evaluation_data; // heuristic evaluation data

    MoveEvaluation minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int score_player(Player player);

    void find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves) const;
    bool is_cell_relevant(int row, int col) const;

    int _heuristic_evaluation();
    void sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer);

public:
    int move_count;
    int move_evaluated_count;
    int evaluation_needed_count;

    GomokuAI(const GomokuAiSettings &settings);
    MoveEvaluation suggest_move(const GomokuGame &board);

    int get_heuristic_evaluation(const GomokuGame &board, Player player);

    const GomokuAIData &get_evaluation_data() const;

    std::vector<MoveHeuristic> get_relevant_moves(const GomokuGame &board);
};
