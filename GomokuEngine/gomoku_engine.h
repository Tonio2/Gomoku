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

struct CellChange {
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

class GomokuGame
{
private:
    std::vector<Player> board;
    uint board_size;
    Player current_player;
    int black_score;
    int white_score;
    Player winner;

    bool coordinates_are_valid(int row, int col) const;
    Player other_player(Player player) const;

    /** Stone counting */
    std::pair<int, bool> count_stones_and_gap(uint row, uint col, int row_dir, int col_dir, Player player, bool &space) const;
    int count_stones(uint row, uint col, int row_dir, int col_dir, Player player) const;

    bool check_dir_for_5_in_a_row(uint row, uint col, int row_dir, int col_dir, Player player) const;
    bool check_5_in_a_row(uint row, uint col, Player player) const;

    bool check_direction_for_open_three(uint row, uint col, int row_dir, int col_dir, Player player) const;

    /** Capture */
    bool try_direction_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult& move_result);
    bool try_cardinal_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult& move_result);
    bool capture(uint row, uint col, Player player, MoveResult& move_result);

    /** Board state */
    CellChange set_board_value(int row, int col, Player value);
    void modify_player_score(Player player, int score);

public:
    GomokuGame(uint _size);

    bool is_game_over() const;
    MoveResult make_move(int row, int col);
    void reverse_move(const MoveResult& move);
    void reapply_move(const MoveResult& move);
    bool check_win(uint row, uint col, Player player);
    int count_open_threes(uint row, uint col, Player player) const;
    unsigned char get_winner() const;
    unsigned char get_board_value(int row, int col) const;
    int get_board_size() const;
    unsigned char get_current_player() const;
    int get_player_score(Player player) const;
};

#endif // GOMOKU_ENGINE_H