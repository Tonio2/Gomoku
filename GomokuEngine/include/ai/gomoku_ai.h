
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
    int score = 0;
#ifndef NDEBUG
    int neededEvalCount = 0;
    int totalEvalCount = 0;
    int evaluatedMoves = 0;
    int initialScore = 0;
#endif
    std::vector<MoveEvaluation> listMoves = {};
};

struct MoveHeuristic
{
    uint8_t row;
    uint8_t col;

    int score;

    bool operator==(const MoveHeuristic &other) const
    {
        return (row == other.row && col == other.col && score == other.score);
    }
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

    std::pair<int, int> killer_move;

    void minimax(MoveEvaluation &eval, int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
    int score_player(Player player);

    void find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves) const;
    bool is_cell_relevant(int row, int col) const;

    int _heuristic_evaluation();
    void sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer);

public:
#ifndef NDEBUG
    int move_count = 0;
    int move_evaluated_count = 0;
    int evaluation_needed_count = 0;
#endif

    GomokuAI(const GomokuAiSettings &settings);
    MoveEvaluation suggest_move(const GomokuGame &board);

    int get_heuristic_evaluation(const GomokuGame &board, Player player);

    const GomokuAIData &get_evaluation_data() const;

    std::vector<MoveHeuristic> get_relevant_moves(const GomokuGame &board);
};
