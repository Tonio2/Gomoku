
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

std::ostream& operator<<(std::ostream& stream, Player player);

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

struct Pattern
{
    StructureType type;
    std::string pattern;
};

struct Structure
{
    StructureType type;
    std::vector<std::pair<int, int>> cells;
};

struct CellChange
{
    // Matrix<Player>::Index index;
    unsigned short row;
    unsigned short col;
    Player old_value;
    Player new_value;
};

struct MoveResult
{
    std::vector<CellChange> cell_changes;

    short int white_score_change = 0;
    short int black_score_change = 0;
};
