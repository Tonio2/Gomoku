#ifndef GOMOKU_ENGINE_H
#define GOMOKU_ENGINE_H

#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

class GomokuGame
{
private:
    std::vector<std::vector<char>> board;
    char current_player;
    bool game_over;

    static std::vector<char> coordinates;
    size_t compute_coordinate(char c) const;
    bool coordinates_are_valid(int row, int col) const;
    std::pair<int, bool> count_stones_and_gap(size_t row, size_t col, int row_dir, int col_dir, char player, bool &space) const;
    bool check_direction_for_open_three(size_t row, size_t col, int row_dir, int col_dir, char player) const;

public:
    GomokuGame();
    bool is_game_over() const;
    void make_move(char row, char col);
    bool check_win() const;
    std::string get_board() const;
    int count_open_threes(size_t row, size_t col, char player) const;
};

#endif // GOMOKU_ENGINE_H
