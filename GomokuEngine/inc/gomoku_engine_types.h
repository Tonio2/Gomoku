
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include "Timer.h"
#include "matrix.hpp"

#define X Player::BLACK
#define O Player::WHITE
#define E Player::EMPTY

#ifndef NOTIMER
#define TIMER Timer timer(__FUNCTION__);
#else
#define TIMER
#endif

enum Player : unsigned char
{
    EMPTY,
    BLACK,
    WHITE,
};

std::ostream &operator<<(std::ostream &stream, Player player);

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

struct MoveHistory
{
    int row;
    int col;
    MoveResult move_result;
    std::pair<Entity, Action> action;
};

enum Mode
{
    HUMAN_VS_HUMAN,
    HUMAN_VS_AI,
};

enum RuleStyle
{
    STANDARD,
    PRO,
    SWAP,
};

enum Action
{
    PLAY,
    SWAP_CHOICE,
};

enum Entity
{
    PLAYER1,
    PLAYER2,
};

enum Team
{
    BLACKS,
    WHITES
};

struct ActionResult
{
    bool success;
    std::string message;
    std::pair<Entity, Action> next_action;
};

struct ActionParameters
{
    int row;
    int col;
    Player team;
};