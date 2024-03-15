
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
enum CellPatternState {
    CELL_STATE_EMPTY,
    CELL_STATE_STONE,
    CELL_STATE_BLOCK,
};

/** One direction pattern cell.
 * Each cell contains all data required to know about what before them
 * without having to look for it.
 * Each cell is generated from the data of the previous cell and the value
 * of the game cell.
 * When a cell is updated from the game, we only have to modify the
 * matching cell in the structure matrix and all following cells until
 * we've stopped modifying it.
*/
struct CellPatternData {
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
    /** Is the previous structure closed
    */
    bool is_previous_structure_closed;

    /** Is gaped open three.
     * Handle niche case where we have sequence of open one and two
     */
    bool is_gap_open_three;
    /** Is the gaped open three closed.
    */
    bool is_gap_open_three_closed;

    bool operator!=(const CellPatternData& comp) const;

    static CellPatternData pre_bound_element();

    bool contains_structure() const;
};

std::ostream& operator<<(std::ostream& stream, const CellPatternData& cell_data);

typedef Matrix<Player>::Index GomokuCellIndex;

struct PatternCellIndex : public Matrix<CellPatternData>::Index {

    PatternCellIndex(int row, int col) :
    Matrix<CellPatternData>::Index(row, col)
    {}

    PatternCellIndex(GomokuCellIndex gomoku_index) :
        Matrix<CellPatternData>::Index(gomoku_index.row + 1, gomoku_index.col + 1)
    {}

    GomokuCellIndex to_game_index() const {
        return GomokuCellIndex(row - 1, col - 1);
    }
};

enum PatternDirection: uint8_t {
    LeftToRight = 0,
    UpToDown = 1,
    UpleftToDownright = 2,
    UprightToDownleft = 3,
    Count = 4
};

class GomokuPatternReconizer
{

public:

    GomokuPatternReconizer(Player player);

    void find_patterns_in_board(const GomokuGame& board);

    void update_patterns_with_move(const GomokuGame& board, const MoveResult& Count_move);

    void print_patterns();


private:

    /** Return the state of a cell for our gomoku player */
    CellPatternState cell_state_at(const GomokuGame& board, PatternCellIndex index) const;

    /** Calculate the next state from a cell when meeting each state. */
    CellPatternData cell_data_following(const CellPatternData& cell, CellPatternState state) const;

    /** Check if pattern cell matrices are of the desired size or adjust them */
    bool adjust_matrices_size(const GomokuGame& board);

    /** Initialize matrices bounds to block elements */
    void initialize_matrices_bounds();

    /** Update all cells of the matrices */
    void update_all_cells(const GomokuGame& board);

    /** Update cell in all direction matrices */
    void update_cell(const GomokuGame& board, PatternCellIndex index);

    /** Update cells in left to right matrices at the specified location
     * 
     * up_to_bound: Should we update everything until the end or stop when
     * we find identical results.
    */
    void update_cell_line(const GomokuGame& board, PatternCellIndex index, PatternDirection direction, bool up_to_bound = false);

    PatternCellIndex get_previous_index(PatternCellIndex index, PatternDirection direction) const;
    PatternCellIndex get_next_index(PatternCellIndex index, PatternDirection direction) const;

    void untag_celldata_structure(PatternCellIndex index, PatternDirection direction);
    void tag_celldata_structure(PatternCellIndex index, PatternDirection direction);

    void for_each_structures(std::function<void(PatternCellIndex, CellPatternData, PatternDirection)> lambda);

    Player _gomoku_player;
    std::vector<Matrix<CellPatternData>> _pattern_direction_cell_matrices;
    std::vector<std::map<int, std::set<int>>> _pattern_direction_structure_maps;

};
