
#pragma once

#include "ai/gomoku_ai_data.h"

class Arena
{
public:
    Arena();

    void play(int argc, char *argv[]);

    /** Play one game and return the winner */
    int play_game(const GomokuAIData &p1, const GomokuAIData &p2);

private:
    int _game_width;
    int _game_height;
};
