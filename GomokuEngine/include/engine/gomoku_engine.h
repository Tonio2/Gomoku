
#pragma once

#include "gomoku_engine_types.h"
#include "gomoku_pattern_reconizer.h"
#include <unordered_map>
#include <unordered_set>

#define RELEVANCY_LENGTH 2

using CellSet = std::unordered_map<int, std::unordered_set<int>>;

class GomokuGame
{
private:
    Matrix<Player> board;
    GomokuCellIndex _min_played;
    GomokuCellIndex _max_played;
    Matrix<int8_t> _relevancy_matrix;
    CellSet _relevant_cells;

    int empty_cells;

    Player current_player;
    std::vector<int> players_scores;
    bool is_game_over_flag;
    Player winner;
    std::vector<GomokuPatternReconizer> players_reconizers;
    bool _capture_enabled;

    /** Relevancy */
    void update_relevancy(int8_t row, int8_t col, bool is_new_empty_cell);

    /** Capture */
    bool try_direction_for_capture(int row, int col, int row_dir, int col_dir, Player player, MoveResult &move_result, bool updateRelevancyMatrix = true);
    bool capture(int row, int col, Player player, MoveResult &move_result, bool updateRelevancyMatrix = true);

    /** Board state */

    void modify_player_score(Player player, int score);

public:
    GomokuGame(uint width, uint height, bool capture_enabled = true);
    GomokuGame(const GomokuGame &copy);

    GomokuGame &operator=(const GomokuGame &copy);

    virtual ~GomokuGame();

    bool is_game_over() const;
    MoveResult make_move(int row, int col, bool updateRelevancyMatrix = true);
    void reverse_move(const MoveResult &move, bool updateRelevancyMatrix = true);
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
    CellChange set_board_value(int row, int col, Player value, bool updateRelevancyMatrix = true);

    std::pair<GomokuCellIndex, GomokuCellIndex> get_played_bounds(int margin = 0) const;
    bool has_player_bounds() const;
    const CellSet &get_relevant_cells() const;
    int8_t get_cell_relevancy(int row, int col) const;

    const GomokuPatternReconizer &get_pattern_reconizer(Player player) const;
    const std::vector<int> &get_patterns_count(Player player);
    void print_patterns();
    std::vector<std::vector<int>> get_board() const;
};

bool cell_set_contains(const CellSet &cell_set, int row, int col);
