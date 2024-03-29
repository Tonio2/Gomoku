
#pragma once

#include "gomoku_engine_types.h"
#include "gomoku_pattern_reconizer.h"

class GomokuGame
{
private:
    Matrix<Player> board;
    int empty_cells;

    Player current_player;
    std::vector<int> players_scores;
    bool is_game_over_flag;
    Player winner;
    std::vector<GomokuPatternReconizer> players_reconizers;

    /** Capture */
    bool try_direction_for_capture(int row, int col, int row_dir, int col_dir, Player player, MoveResult &move_result);
    bool capture(int row, int col, Player player, MoveResult &move_result);

    /** Board state */

    void modify_player_score(Player player, int score);

public:
    GomokuGame(uint width, uint height);
    GomokuGame(const GomokuGame &copy);

    GomokuGame &operator=(const GomokuGame &copy);

    virtual ~GomokuGame();

    bool is_game_over() const;
    MoveResult make_move(int row, int col);
    void reverse_move(const MoveResult &move);
    void reapply_move(const MoveResult &move);
    void check_win(Player player);
    Player get_winner() const;
    Player get_board_value(int row, int col) const;

    int get_board_width() const;
    int get_board_height() const;
    bool coordinates_are_valid(int row, int col) const;

    Player other_player(Player player) const;
    Player get_current_player() const;
    int get_player_score(Player player) const;
    CellChange set_board_value(int row, int col, Player value);
    void display_board() const;
    const std::vector<int> &get_patterns_count(Player player);
    void print_patterns();
    std::vector<std::vector<int>> get_board() const;
};
