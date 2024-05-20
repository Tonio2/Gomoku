
#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include "timer/Timer.h"
#include "matrix/Matrix.hpp"
#include <functional> // For std::hash

#define X Player::BLACK
#define O Player::WHITE
#define E Player::EMPTY

enum Player : uint8_t
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
};

namespace std
{
    template <>
    struct hash<std::pair<int, int>>
    {
        std::size_t operator()(const std::pair<int, int> &p) const
        {
            // Compute individual hash values for two data members and combine them using XOR and bit shifting
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };
}

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
    std::vector<std::pair<int, int>> new_relevant_moves;
    std::vector<std::pair<int, int>> removed_relevant_moves;

    int8_t white_score_change = 0;
    int8_t black_score_change = 0;

    GomokuCellIndex previous_min_move;
    GomokuCellIndex previous_max_move;
};
