#pragma once

#include "engine/gomoku_engine.h"

namespace AI
{

using Move = Matrix<Player>::Index;

class IGomokuAI
{
public:
    virtual Move suggest_move(const GomokuGame &board) = 0;
    virtual std::vector<Move> suggest_move_sequence(const GomokuGame &board) = 0;

    virtual ~IGomokuAI() = default;
};

} // namespace AI
