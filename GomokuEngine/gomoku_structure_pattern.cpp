
#include "gomoku_structure_pattern.h"


/** CellPatternData */

CellPatternData CellPatternData::pre_bound_element() {
    return CellPatternData { 0, true, 0, false, 0, false, false };
}

void CellPatternData::print() {
    std::cout << "[" << static_cast<int>(sequence_length)
        << (is_sequence_closed ? " X " : " O ")
        << static_cast<int>(structure_length)
        << (is_structure_closed ? "x (" : "o (")
        << static_cast<int>(previous_structure_length)
        << (is_previous_structure_closed ? "x" : "o")
        << ")"
        << (is_gap_open_three ? " - GOT" : "")
        << "]";
}

/** GomokuPatternReconizer */

GomokuPatternReconizer::GomokuPatternReconizer(Player player)
:
_gomoku_player(player),
_pattern_direction_cell_matrices(PATTERN_DIRECTIONS_COUNT)
{}

void GomokuPatternReconizer::find_patterns_in_board(const GomokuGame& board)
{
    adjust_matrices_size(board);
    initialize_matrices_bounds();
    update_all_cells(board);
}

void GomokuPatternReconizer::update_patterns_with_move(const GomokuGame& board, const MoveResult& last_move)
{
    find_patterns_in_board(board);
    // TODO: update only one cell
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Nothing here" << std::endl;
}


CellPatternData GomokuPatternReconizer::cell_data_following(const CellPatternData& cell, CellPatternState state) const
{
    CellPatternData result;

    switch (state) {
    case CELL_STATE_EMPTY:
        result.sequence_length = 0;
        result.is_sequence_closed = false;
        result.structure_length = cell.sequence_length;
        result.is_structure_closed = cell.is_sequence_closed;
        result.previous_structure_length = result.structure_length;
        result.is_previous_structure_closed = result.is_structure_closed;
        result.is_gap_open_three =
            (cell.previous_structure_length == 2 && result.structure_length == 1)
            || (cell.previous_structure_length == 1 && result.structure_length == 2);
        break;
    case CELL_STATE_STONE:
        result.sequence_length = cell.sequence_length + 1;
        result.is_sequence_closed = cell.is_sequence_closed;
        result.structure_length = 0;
        result.is_structure_closed = 0;
        result.previous_structure_length = cell.previous_structure_length;
        result.is_previous_structure_closed = cell.is_previous_structure_closed;
        result.is_gap_open_three = false;
        break;
    case CELL_STATE_BLOCK:
        result.sequence_length = 0;
        result.is_sequence_closed = true;
        const bool structure_present = !cell.is_sequence_closed && cell.sequence_length > 0;
        result.structure_length = structure_present ? cell.sequence_length : 0;
        result.is_structure_closed = structure_present;
        result.previous_structure_length = 0;
        result.is_previous_structure_closed = 0;
        result.is_gap_open_three =
            (cell.previous_structure_length == 2 && result.structure_length == 1)
            || (cell.previous_structure_length == 1 && result.structure_length == 2);
        break;
    };
    return result;
}

bool GomokuPatternReconizer::adjust_matrices_size(const GomokuGame& board) {
    bool modified_matrices = false;

    for (int i = 0; i < PATTERN_DIRECTIONS_COUNT; ++i) {
        int width = board.get_board_size() + 2;
        int height = board.get_board_size() + 2;

        const bool size_differ = _pattern_direction_cell_matrices[i].get_width() != width
            || _pattern_direction_cell_matrices[i].get_height() != height;

        if (size_differ) {
            _pattern_direction_cell_matrices[i] = Matrix<CellPatternData>(width, height);
            modified_matrices = true;
        }
    }

    return modified_matrices;
}

void GomokuPatternReconizer::initialize_matrices_bounds() {

    CellPatternData out_cell = CellPatternData::pre_bound_element();

    for (int i = 0; i < PATTERN_DIRECTIONS_COUNT; ++i) {
        
        Matrix<CellPatternData>& mat(_pattern_direction_cell_matrices[i]);

        for (int row = 0; row < mat.get_height(); ++row) {
            mat(row, 0) = out_cell;
            mat(row, mat.get_width() - 1) = out_cell;
        }
        for (int col = 0; col < mat.get_width(); ++col) {
            mat(0, col) = out_cell;
            mat(mat.get_height() - 1, col) = out_cell;
        }
    }
}

void GomokuPatternReconizer::update_all_cells(const GomokuGame& board) {
    // TODO: update cells for all directions
}

void GomokuPatternReconizer::update_cell(int width, int height, CellPatternState state) {
    // TODO: update one cell for each directions while we're modifying the state
}
