#ifndef GOMOKU_ENGINE_H
#define GOMOKU_ENGINE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstddef>

#define X Player::BLACK
#define O Player::WHITE
#define E Player::EMPTY

enum Player: unsigned char
{
    EMPTY,
    BLACK,
    WHITE
};

class GomokuGame
{
private:
    std::vector<Player> board;
    uint board_size;
    Player current_player;
    std::map<Player, int> player_scores;
    Player winner;

    bool coordinates_are_valid(int row, int col) const;
    std::pair<int, bool> count_stones_and_gap(size_t row, size_t col, int row_dir, int col_dir, Player player, bool &space) const;
    bool check_direction_for_open_three(size_t row, size_t col, int row_dir, int col_dir, Player player) const;
    bool try_direction_for_capture(size_t row, size_t col, int row_dir, int col_dir, Player player);
    bool try_cardinal_for_capture(size_t row, size_t col, int row_dir, int col_dir, Player player);
    bool capture(size_t row, size_t col, Player player);
    int count_stones(size_t row, size_t col, int row_dir, int col_dir, Player player) const;
    bool check_dir_for_5_in_a_row(size_t row, size_t col, int row_dir, int col_dir, Player player) const;
    bool check_5_in_a_row(size_t row, size_t col, Player player) const;
    
    Player other_player(Player player) const;

public:
    GomokuGame(uint _size);
    bool is_game_over() const;
    void make_move(int row, int col);
    bool check_win(size_t row, size_t col, Player player);
    int count_open_threes(size_t row, size_t col, Player player) const;
    unsigned char get_winner() const;
    unsigned char get_board_value(int x, int y) const;
    int get_board_size() const;
    void set_board_value(int x, int y, Player value);
    unsigned char get_current_player() const;
};

#endif // GOMOKU_ENGINE_H