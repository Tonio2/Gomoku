
#pragma once

#include "gomoku_engine_types.h"
#include "gomoku_pattern_reconizer.h"

class GomokuGame
{
private:
    Matrix<Player> board;
    GomokuCellIndex _min_played;
    GomokuCellIndex _max_played;
    std::unordered_set<std::pair<int, int>> relevantMoves;
    int empty_cells;

    Player current_player;
    std::vector<int> players_scores;
    bool is_game_over_flag;
    Player winner;
    std::vector<GomokuPatternReconizer> players_reconizers;
    bool _capture_enabled;

    /** Capture */
    bool try_direction_for_capture(int row, int col, int dir_offset_index, Player player, MoveResult &move_result);
    bool capture(int row, int col, Player player, MoveResult &move_result);

    /** Board state */

    void modify_player_score(Player player, int score);
    bool is_cell_relevant(int row, int col) const;

    void remove_relevant_moves(int row, int col, MoveResult &move_result);
    void update_relevant_moves_after_capture(int x, int y, int dir_offset_index, MoveResult &move_result);

public:
    GomokuGame(uint width, uint height, bool capture_enabled = true);
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
    bool pattern_coordinate_is_valid(const PatternCellIndex &index) const;

    Player other_player(Player player) const;
    Player get_current_player() const;
    int get_player_score(Player player) const;
    CellChange set_board_value(int row, int col, Player value);

    std::pair<GomokuCellIndex, GomokuCellIndex> get_played_bounds(int margin = 0) const;
    bool has_player_bounds() const;
    const GomokuPatternReconizer &get_pattern_reconizer(Player player) const;
    const std::vector<int> &get_patterns_count(Player player);
    std::unordered_set<std::pair<int, int>> get_relevant_moves() const { return relevantMoves; };
    void print_patterns();
    std::vector<std::vector<int>> get_board() const;
};
