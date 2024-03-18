
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstddef>
#include "Timer.h"
#include "matrix.hpp"

#define X Player::BLACK
#define O Player::WHITE
#define E Player::EMPTY

enum Player : unsigned char
{
    EMPTY,
    BLACK,
    WHITE
};

std::ostream &operator<<(std::ostream &stream, Player player);

enum StructureType
{
    OPEN_FOUR,
    FOUR,
    OPEN_THREE,
    THREE,
    OPEN_TWO,
    TWO,
    OPEN_ONE,
    ONE
};

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
};
