#pragma once

#include "gomoku_engine.h"
#include <limits>
#include <utility>
#include <random>

class GomokuAIData
{
    static const int STC = StructureType::COUNT_STRUCTURE_TYPE;
    static const int VALUES_COUNT = STC + 5;
    float values[VALUES_COUNT];

    void initialize()
    {
        for (int i = 0; i < VALUES_COUNT; i++)
        {
            values[i] = 1000;
        }
        values[STC + 2] = 1;
        values[STC + 3] = 1;
        values[STC + 4] = 1;
    }

    friend class GomokuAIDataMutator;

public:
    GomokuAIData()
    {
        initialize();
    }

    GomokuAIData(const GomokuAIData &copy)
    {
        for (int i = 0; i < VALUES_COUNT; i++)
            values[i] = copy.values[i];
    }

    GomokuAIData &operator=(const GomokuAIData &copy)
    {
        if (this != &copy)
        {
            for (int i = 0; i < VALUES_COUNT; i++)
                values[i] = copy.values[i];
        }
        return *this;
    }

    float value_of_structure(int structure_type) const
    {
        return values[structure_type];
    }

    float value_of_multiple_forced() const
    {
        return values[STC + 0];
    }

    float value_of_multiple_o4() const
    {
        return values[STC + 1];
    }

    float value_of_captures(int capture_count) const
    {
        const float a = values[STC + 2];
        const float b = values[STC + 3];
        const float c = values[STC + 4];

        return a * (capture_count * capture_count) + b * (capture_count) + c;
    }
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
    int depth;
    Player ai_player;
    Player human_player;
    GomokuAIData evaluation_data;

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
