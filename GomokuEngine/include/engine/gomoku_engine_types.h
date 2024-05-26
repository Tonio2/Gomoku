
#pragma once

#include "matrix/Matrix.hpp"
#include "timer/Timer.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

enum Player : uint8_t
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
};

#define X BLACK
#define O WHITE
#define E EMPTY

std::ostream &operator<<(std::ostream &stream, Player player);

typedef Matrix<Player>::Index GomokuCellIndex;

bool operator==(const GomokuCellIndex &lhs, const GomokuCellIndex &rhs);

enum StructureType : uint8_t
{
    NONE = 0,
    FIVE_OR_MORE = 1,
    OPEN_ONE = 2,
    ONE = 3,
    OPEN_TWO = 4,
    TWO = 5,
    OPEN_THREE = 6,
    THREE = 7,
    OPEN_FOUR = 8,
    FOUR = 9,
    COUNT_STRUCTURE_TYPE,
};

enum Pattern : uint8_t
{
    FORK = 10,
    CAPTURE = 11,
};

bool compare_structure_type(StructureType lhs, StructureType rhs);
std::ostream &operator<<(std::ostream &stream, StructureType structure_type);

struct CellChange
{
    int8_t row;
    int8_t col;
    Player old_value;
    Player new_value;
};

struct MoveResult
{
    std::vector<CellChange> cell_changes;

    int8_t white_score_change = 0;
    int8_t black_score_change = 0;

    GomokuCellIndex previous_min_move;
    GomokuCellIndex previous_max_move;
};
