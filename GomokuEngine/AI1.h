#ifndef GOMOKU_AI_H
#define GOMOKU_AI_H

#include "gomoku_engine.h"
#include <limits>
#include <utility>
#include <random>


class GomokuAI {
private:
    GomokuGame game;
    int depth;
    Player ai_player;
    Player human_player;

    std::pair<int, std::pair<int, int>> minimax(int depth, int alpha, int beta, bool maximizingPlayer);
    int heuristic_evaluation();

public:
    GomokuAI(GomokuGame game, Player ai_player, int depth);
    std::pair<int, int> suggest_move();
};

#endif // GOMOKU_AI_H