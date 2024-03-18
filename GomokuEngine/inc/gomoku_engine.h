
#pragma once

#include "gomoku_engine_types.h"
#include "gomoku_structure_pattern.h"

class GomokuGame
{
private:
    Matrix<Player> board;

    Player current_player;
    std::vector<int> players_scores;
    Player winner;
    std::vector<GomokuPatternReconizer> players_reconizers;

    bool coordinates_are_valid(int row, int col) const;
    Player other_player(Player player) const;

    /** Stone counting */
    std::pair<int, bool> count_stones_and_gap(uint row, uint col, int row_dir, int col_dir, Player player, bool &space) const;
    int count_stones(uint row, uint col, int row_dir, int col_dir, Player player) const;

    bool check_dir_for_5_in_a_row(uint row, uint col, int row_dir, int col_dir, Player player) const;
    bool check_5_in_a_row(uint row, uint col, Player player) const;

    bool check_direction_for_open_three(uint row, uint col, int row_dir, int col_dir, Player player) const;

    /** Capture */
    bool try_direction_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result);
    bool try_cardinal_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result);
    bool capture(uint row, uint col, Player player, MoveResult &move_result);

    /** Board state */

    void modify_player_score(Player player, int score);

public:
    GomokuGame(uint _size);

    virtual ~GomokuGame();

    bool is_game_over() const;
    MoveResult make_move(int row, int col);
    void reverse_move(const MoveResult &move);
    void reapply_move(const MoveResult &move);
    bool check_win(uint row, uint col, Player player);
    int count_open_threes(uint row, uint col, Player player) const;
    Player get_winner() const;
    Player get_board_value(int row, int col) const;

    // TODO: Replace with get_width && get_height
    int get_board_size() const;

    Player get_current_player() const;
    int get_player_score(Player player) const;
    std::vector<std::pair<std::pair<int, int>, int>> findRelevantMoves() const;
    CellChange set_board_value(int row, int col, Player value);
    void display_board() const;
    std::vector<std::vector<int>> get_patterns_count();
    void print_patterns();
};
