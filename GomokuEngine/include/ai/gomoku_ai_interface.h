#pragma once

#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai_data.h"

namespace AI
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

#ifdef LOGGING
        int neededEvalCount = 1;
        int evaluatedEvalCount = 1;
        int totalEvalCount = 1;
        int initialScore = 0;
        int killerMoveHasBeenEvaluated = 0;
#endif
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

    using Move = Matrix<Player>::Index;

    class IGomokuAI
    {
    public:
        virtual Move suggest_move(const GomokuGame &board, int currentMove = 0) = 0;
        virtual std::vector<Move> suggest_move_sequence(const GomokuGame &board) = 0;

        virtual int get_heuristic_evaluation(const GomokuGame &board, Player player) { return 0; }

        virtual ~IGomokuAI() = default;
    };

} // namespace AI
