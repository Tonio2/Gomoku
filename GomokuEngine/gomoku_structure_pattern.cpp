
#include "gomoku_structure_pattern.h"
#include <cassert>


/** CellPatternData */

bool CellPatternData::operator!=(const CellPatternData& comp) const {
    return sequence_length != comp.sequence_length
        || is_sequence_closed != comp.is_sequence_closed
        || structure_length != comp.structure_length
        || is_structure_closed != comp.is_structure_closed
        || previous_structure_length != comp.previous_structure_length
        || is_previous_structure_closed != comp.is_previous_structure_closed
        || is_gap_open_three != comp.is_gap_open_three;
}

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
_pattern_direction_cell_matrices(static_cast<int>(PatternDirection::last))
{}

void GomokuPatternReconizer::find_patterns_in_board(const GomokuGame& board)
{
    Timer timer("find_patterns_in_board");

    adjust_matrices_size(board);
    initialize_matrices_bounds();
    update_all_cells(board);
}

void GomokuPatternReconizer::update_patterns_with_move(const GomokuGame& board, const MoveResult& last_move)
{
    Timer timer("update_patterns_with_move");

    for (CellChange change : last_move.cell_changes) {
        update_cell(board, PatternCellIndex(GomokuCellIndex(change.row, change.col)));
    }
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Nothing here" << std::endl;
}


CellPatternState GomokuPatternReconizer::cell_state_at(const GomokuGame& board, PatternCellIndex index) const {
    if (index.row >= _pattern_direction_cell_matrices[0].get_height()
        || index.col >= _pattern_direction_cell_matrices[0].get_width())
        return CellPatternState::CELL_STATE_BLOCK;

    const GomokuCellIndex gomoku_index = index.to_game_index();

    Player player = Player(board.get_board_value(gomoku_index.row, gomoku_index.col));

    if (player == _gomoku_player)
        return CellPatternState::CELL_STATE_STONE;
    if (player == Player::EMPTY)
        return CellPatternState::CELL_STATE_EMPTY;
    return CellPatternState::CELL_STATE_BLOCK;
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

    for (int i = 0; i < PatternDirection::last; ++i) {
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

    for (int i = 0; i < PatternDirection::last; ++i) {
        
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

    {
        const PatternDirection direction = PatternDirection::LeftToRight;
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        PatternCellIndex index(1, 1);
        for (index.row = 1; index.row < height; ++index.row) {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpToDown;
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        PatternCellIndex index(1, 1);
        for (index.col = 1; index.col < width; ++index.col) {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpleftToDownright;
        PatternCellIndex index(1, 1);
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        for (index.row = 1; index.row < height; ++index.row) {
            update_cell_line(board, index, direction, true);
        }
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        index.row = 1;
        for (index.col = 2; index.col < width; ++index.col) {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UprightToDownleft;
        PatternCellIndex index(1, 1);
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        for (index.col = 1; index.col < width; ++index.col) {
            update_cell_line(board, index, direction, true);
        }
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        index.col = width - 1;
        for (index.row = 2; index.row < height; ++index.row) {
            update_cell_line(board, index, direction, true);
        }
    }

}

void GomokuPatternReconizer::update_cell(const GomokuGame& board, PatternCellIndex index) {
    for (int i = 0; i < PatternDirection::last; ++i) {
        update_cell_line(board, index, PatternDirection(i));
    }
}

void GomokuPatternReconizer::update_cell_line(const GomokuGame& board, PatternCellIndex index, PatternDirection direction, bool up_to_bound) {
    Matrix<CellPatternData>& cell_matrix(_pattern_direction_cell_matrices[direction]);

    assert(get_previous_index(index, direction).is_valid(cell_matrix));

    if (index.col >= cell_matrix.get_width() || index.row >= cell_matrix.get_height())
        return;

    const CellPatternState cell_state = cell_state_at(board, index);

    const CellPatternData old_data = cell_matrix(index.row, index.col);

    const PatternCellIndex prev = get_previous_index(index, direction);
    const CellPatternData new_data = cell_data_following(cell_matrix(prev.row, prev.col), cell_state);

    cell_matrix(index.row, index.col) = new_data;

    if (old_data.structure_length) {
        std::cout << "-- Struct "
            << (old_data.is_structure_closed ? "Close-" : "Open-")
            << int(old_data.structure_length)
            << " [" << index.row << ';' << index.col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (old_data.is_gap_open_three) {
        std::cout << "-- Struct GapOp-3"
            << " [" << index.row << ';' << index.col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (new_data.structure_length) {
        std::cout << "++ Struct "
            << (new_data.is_structure_closed ? "Close-" : "Open-")
            << int(new_data.structure_length)
            << " [" << index.row << ';' << index.col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (new_data.is_gap_open_three) {
        std::cout << "++ Struct GapOp-3"
            << " [" << index.row << ';' << index.col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }

    const bool should_continue = up_to_bound || old_data != new_data;

    if (should_continue)
    {
        const PatternCellIndex next = get_next_index(index, direction);
        if (next.is_valid(cell_matrix)) {
            update_cell_line(board, next, direction, up_to_bound);
        }
    }
}

PatternCellIndex GomokuPatternReconizer::get_previous_index(PatternCellIndex index, PatternDirection direction) const {
    switch (direction) {
    case PatternDirection::LeftToRight:
        return PatternCellIndex(index.row, index.col - 1);
    case PatternDirection::UpToDown:
        return PatternCellIndex(index.row - 1, index.col);
    case PatternDirection::UpleftToDownright:
        return PatternCellIndex(index.row - 1, index.col - 1);
    case PatternDirection::UprightToDownleft:
        return PatternCellIndex(index.row - 1, index.col + 1);
    default:
        return index;
    }
}

PatternCellIndex GomokuPatternReconizer::get_next_index(PatternCellIndex index, PatternDirection direction) const {
    switch (direction) {
    case PatternDirection::LeftToRight:
        return PatternCellIndex(index.row, index.col + 1);
    case PatternDirection::UpToDown:
        return PatternCellIndex(index.row + 1, index.col);
    case PatternDirection::UpleftToDownright:
        return PatternCellIndex(index.row + 1, index.col + 1);
    case PatternDirection::UprightToDownleft:
        return PatternCellIndex(index.row + 1, index.col - 1);
    default:
        return index;
    }
}
