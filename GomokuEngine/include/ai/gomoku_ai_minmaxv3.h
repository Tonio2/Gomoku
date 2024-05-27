
#pragma once

#include "gomoku_ai_data.h"
#include "gomoku_ai_interface.h"
#include <fstream>

namespace AI::MinMaxV3
{
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

struct MoveEvaluation
{
    int row;
    int col;
    std::vector<MoveEvaluation> listMoves = {};
    int score = 0;
    int cur_move_id = 0;
    int best_move_id = 0;
    int initial_score = 0;
    std::vector<MoveHeuristic> relevant_moves = {};
};

class GomokuAI : public IGomokuAI
{
public:
    GomokuAI(const GomokuAiSettings &settings);
    MoveEvaluation suggest_move_evaluation(const GomokuGame &board);
    Move suggest_move(const GomokuGame &board, int currentMove = 0) override;
    std::vector<Move> suggest_move_sequence(const GomokuGame &board) override;

    int get_heuristic_evaluation(const GomokuGame &board, Player player) override;
    const GomokuAIData &get_evaluation_data() const;

private:
    GomokuGame game;
    int depth;  // min max depth algorithm
    int length; // evaluated moves distance from structures
    Player ai_player;
    Player human_player;
    GomokuAIData evaluation_data; // heuristic evaluation data
    bool deepening;
    bool deepening_successful;
    int deepened_move_score;

    std::vector<std::pair<int, int>> killer_moves;

    void evaluateNode(const MoveHeuristic &move, int move_id, int _depth, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremEval, std::pair<int, int> &best_move, bool isFirstMove);

    void minimax(MoveEvaluation &eval, int _depth, int alpha, int beta, bool maximizingPlayer);
    int score_player(Player player);

    void compute_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves, int _depth) const;

    int _heuristic_evaluation();
    void sortMoves(MoveEvaluation &eval, bool maximizingPlayer);

    // std::vector<MoveHeuristic> get_relevant_moves(const GomokuGame &board);
};

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer = true);
const MoveEvaluation &getBestMoveEvaluation(const MoveEvaluation &eval, bool maximizingPlayer = true);

void writeMoveEvaluation(std::ostream &out, const MoveEvaluation &eval, std::vector<std::vector<std::string>> &csvData, int depth = 0, int i = -1);
void logMoveEvaluation(const MoveEvaluation &eval, std::string filename);
} // namespace AI::MinMaxV3
