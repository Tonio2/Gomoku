
#include "matrix.hpp"
#include "gomoku_engine.h"

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

    static CellPatternData pre_bound_element();

    void print();
};

class GomokuPatternReconizer
{

public:

    GomokuPatternReconizer(Player player);

    void find_patterns_in_board(const GomokuGame& board);

    void update_patterns_with_move(const GomokuGame& board, const MoveResult& last_move);

    void print_patterns();


// private:

    Player _gomoku_player;

    static const int PATTERN_DIRECTION_LEFT_TO_RIGHT = 0;
    static const int PATTERN_DIRECTION_UP_TO_DOWN = 1;
    static const int PATTERN_DIRECTION_UPLEFT_TO_DOWNRIGHT = 2;
    static const int PATTERN_DIRECTION_UPRIGHT_TO_DOWNLEFT = 3;

    static const int PATTERN_DIRECTIONS_COUNT = 4;

    std::vector<Matrix<CellPatternData>> _pattern_direction_cell_matrices;

    /** Calculate the next state from a cell when meeting each state. */
    CellPatternData cell_data_following(const CellPatternData& cell, CellPatternState state) const;

    /** Check if pattern cell matrices are of the desired size or adjust them */
    bool adjust_matrices_size(const GomokuGame& board);

    /** Initialize matrices bounds to block elements */
    void initialize_matrices_bounds();

    void update_all_cells(const GomokuGame& board);

    /** Update cell in all direction matrices */
    void update_cell(int width, int height, CellPatternState state);

};
