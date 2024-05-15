
#pragma once

#include "ai/gomoku_ai_interface.h"

class Arena
{
public:
    Arena();

    void play(int argc, char *argv[]);

    /** Play one game and return the winner */
    int play_game(AI::IGomokuAI &p1, AI::IGomokuAI &p2);

private:
    int _game_width;
    int _game_height;
};
