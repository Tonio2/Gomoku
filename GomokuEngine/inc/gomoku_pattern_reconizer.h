
#pragma once

#include "gomoku_engine_types.h"
#include "matrix.hpp"
#include <map>
#include <set>
#include <functional>

class GomokuGame;

/** State of one cell.
 * Empty means no player is there.
 * Stone means we have one of our stone there
 * Block means the opponent has a stone there or we're out of bound.
 */
enum PatternCellState : uint8_t
{
    Empty = 0,
    Stoned = 1,
    Blocked = 2,
};

std::ostream &operator<<(std::ostream &stream, PatternCellState cell_state);

/** One direction pattern cell.
 * Each cell contains all data required to know about what before them
 * without having to look for it.
 * Each cell is generated from the data of the previous cell and the value
 * of the game cell.
 * When a cell is updated from the game, we only have to modify the
 * matching cell in the structure matrix and all following cells until
 * we've stopped modifying it.
 */
struct PatternCellData
{
    /** Length of the potential sequence we're doing */
    uint8_t sequence_length;
    /** Define if the sequence bounded by an obstable at the begining */
    bool is_sequence_closed;

    /** Size of the structure ending on this cell.
     * 0 means no structure
     */
    uint8_t structure_length;
    /** Define if the structure is closed from one side.
     * If the structure is closed on both sides, it's not a structure.
     */
    bool is_structure_closed;

    /** Size of the structure that we're directly following.
     * This case is relevant to check for open three with gaps.
     */
    uint8_t previous_structure_length;
    /** Is the previous structure closed */
    bool is_previous_structure_closed;

    /** Is gaped open three.
     * Handle niche case where we have sequence of open one and two
     */
    bool is_gap_open_three;
    /** Is the gaped open three closed. */
    bool is_gap_open_three_closed;

    bool operator!=(const PatternCellData &comp) const;

    static PatternCellData pre_bound_element();

    bool contains_structure() const;

    void get_structures_type_count(std::vector<int> &array, int factor = 1) const;
};

std::ostream &operator<<(std::ostream &stream, const PatternCellData &cell_data);

typedef Matrix<Player>::Index GomokuCellIndex;

struct PatternCellIndex : public Matrix<PatternCellData>::Index
{
    PatternCellIndex(int row, int col);
    PatternCellIndex(GomokuCellIndex gomoku_index);

    GomokuCellIndex to_game_index() const;
};

enum PatternDirection : uint8_t
{
    LeftToRight,
    UpToDown,
    UpleftToDownright,
    UprightToDownleft,
    Count_PatternDirection
};

std::ostream &operator<<(std::ostream &stream, PatternDirection direction);

class GomokuPatternReconizer
{

public:
    GomokuPatternReconizer(Player player);
    GomokuPatternReconizer(const GomokuPatternReconizer &copy);

    GomokuPatternReconizer &operator=(const GomokuPatternReconizer &copy);

    ~GomokuPatternReconizer();

    void find_patterns_in_board(const GomokuGame &board);

    void update_patterns_with_move(const GomokuGame &board, const MoveResult &Count_move);

    void print_patterns();

    const std::vector<int> &get_pattern_count();

    bool five_or_more_cant_be_captured(GomokuGame &game) const;
    bool can_capture(GomokuGame &game);

private:
    /** Return the state of a cell for our gomoku player */
    PatternCellState cell_state_at(const GomokuGame &board, PatternCellIndex index) const;

    /** Calculate the next state from a cell when meeting each state. */
    PatternCellData cell_data_following(const PatternCellData &cell, PatternCellState state) const;

    /** Memoized version of cell_data_following */
    const PatternCellData &cell_data_following_memoized(const PatternCellData &cell, PatternCellState state) const;

    /** Check if pattern cell matrices are of the desired size or adjust them */
    bool adjust_matrices_size(const GomokuGame &board);

    /** Initialize matrices bounds to block elements */
    void initialize_matrices_bounds();

    /** Update all cells of the matrices */
    void update_all_cells(const GomokuGame &board);

    /** Update cell in all direction matrices */
    void update_cell(const GomokuGame &board, PatternCellIndex index);

    /** Update cells in the direction matrix at the specified location
     *
     * up_to_bound: Should we update everything until the end or stop when
     * we find identical results.
     */
    void update_cell_direction(const GomokuGame &board, PatternCellIndex index, PatternDirection direction, bool up_to_bound = false);

    std::pair<int, int> get_dir_coordinates(PatternDirection direction) const;
    PatternCellIndex get_previous_index(PatternCellIndex index, PatternDirection direction) const;
    PatternCellIndex get_next_index(PatternCellIndex index, PatternDirection direction) const;

    void untag_celldata_structure(PatternCellIndex index, PatternDirection direction);
    void tag_celldata_structure(PatternCellIndex index, PatternDirection direction);

    void for_each_structures(std::function<void(PatternCellIndex, PatternCellData, PatternDirection)> lambda);

    Player _gomoku_player;
    std::vector<Matrix<PatternCellData>> _pattern_direction_cell_matrices;
    std::vector<std::map<int, std::set<int>>> _pattern_direction_structure_maps;
    std::vector<int> _cached_pattern_count;
};
