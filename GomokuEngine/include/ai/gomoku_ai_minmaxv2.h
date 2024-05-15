
#pragma once

#include "gomoku_ai_interface.h"
#include "gomoku_ai_data.h"

namespace AI::MinMaxV2
{

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

    class GomokuAI : public IGomokuAI
    {
    public:
        Move suggest_move(const GomokuGame &board) override;
        std::vector<Move> suggest_move_sequence(const GomokuGame &board) override;

    private:
        GomokuGame game;
        int depth;  // min max depth algorithm
        int length; // evaluated moves distance from structures
        Player ai_player;
        Player human_player;
        GomokuAIData evaluation_data; // heuristic evaluation data

        std::pair<int, int> killer_move;

        void evaluateNode(const MoveHeuristic &move, int depth, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremEval, std::pair<int, int> &best_move, bool isFirstMove);

        void minimax(MoveEvaluation &eval, int depth, int alpha, int beta, bool maximizingPlayer, int row, int col);
        int score_player(Player player);

        void find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves) const;
        bool is_cell_relevant(int row, int col) const;

        int _heuristic_evaluation();
        void sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer);

    public:
        GomokuAI(const GomokuAiSettings &settings);
        MoveEvaluation suggest_move_evaluation(const GomokuGame &board);

        int get_heuristic_evaluation(const GomokuGame &board, Player player);

        const GomokuAIData &get_evaluation_data() const;

        std::vector<MoveHeuristic> get_relevant_moves(const GomokuGame &board);
    };

} // namespace AI::MinMaxV2
