
#include "gomoku_structure_pattern.h"


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
_pattern_direction_cell_matrices(PATTERN_DIRECTIONS_COUNT)
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
        update_cell(board, change.row + 1, change.col + 1);
    }
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Nothing here" << std::endl;
}


CellPatternState GomokuPatternReconizer::cell_state_at(const GomokuGame& board, int row, int col) const {
    if (row >= board.get_board_size() + 1 || col >= board.get_board_size() + 1)
        return CellPatternState::CELL_STATE_BLOCK;

    Player player = Player(board.get_board_value(row - 1, col - 1));
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

    {
        const int height = _pattern_direction_cell_matrices[PATTERN_DIRECTION_LEFT_TO_RIGHT].get_height() - 1;
        for (int row = 1; row < height; ++row) {
            update_cell_left_to_right(board, row, 1, true);
        }
    }

    {
        const int width = _pattern_direction_cell_matrices[PATTERN_DIRECTION_UP_TO_DOWN].get_width() - 1;
        for (int col = 1; col < width; ++col) {
            update_cell_up_to_down(board, 1, col, true);
        }
    }

    {
        const int height = _pattern_direction_cell_matrices[PATTERN_DIRECTION_UPLEFT_TO_DOWNRIGHT].get_height() - 1;
        for (int row = 1; row < height; ++row) {
            update_cell_upleft_to_downright(board, row, 1, true);
        }
        const int width = _pattern_direction_cell_matrices[PATTERN_DIRECTION_UPLEFT_TO_DOWNRIGHT].get_width() - 1;
        for (int col = 2; col < width; ++col) {
            update_cell_upleft_to_downright(board, 1, col, true);
        }
    }

    {
        const int width = _pattern_direction_cell_matrices[PATTERN_DIRECTION_UPRIGHT_TO_DOWNLEFT].get_width() - 1;
        for (int col = 1; col < width; ++col) {
            update_cell_upright_to_downleft(board, 1, col, true);
        }
        const int height = _pattern_direction_cell_matrices[PATTERN_DIRECTION_UPRIGHT_TO_DOWNLEFT].get_height() - 1;
        for (int row = 2; row < height; ++row) {
            update_cell_upright_to_downleft(board, row, width - 1, true);
        }
    }

}

void GomokuPatternReconizer::update_cell(const GomokuGame& board, int row, int col) {
    update_cell_left_to_right(board, row, col);
    update_cell_up_to_down(board, row, col);
    update_cell_upleft_to_downright(board, row, col);
    update_cell_upright_to_downleft(board, row, col);
}

void GomokuPatternReconizer::update_cell_left_to_right(const GomokuGame& board, int row, int col, bool up_to_bound) {
    Matrix<CellPatternData>& cell_matrix(_pattern_direction_cell_matrices[PATTERN_DIRECTION_LEFT_TO_RIGHT]);

    if (col >= cell_matrix.get_width())
        return;

    const CellPatternState cell_state = cell_state_at(board, row, col);

    const CellPatternData old_data = cell_matrix(row, col);
    const CellPatternData new_data = cell_data_following(cell_matrix(row, col - 1), cell_state);
    cell_matrix(row, col) = new_data;
    if (old_data.structure_length) {
        std::cout << "-- Struct "
            << (old_data.is_structure_closed ? "Close-" : "Open-")
            << int(old_data.structure_length)
            << " [" << row << ';' << col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (old_data.is_gap_open_three) {
        std::cout << "-- Struct GapOp-3"
            << " [" << row << ';' << col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (new_data.structure_length) {
        std::cout << "++ Struct "
            << (new_data.is_structure_closed ? "Close-" : "Open-")
            << int(new_data.structure_length)
            << " [" << row << ';' << col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }
    if (new_data.is_gap_open_three) {
        std::cout << "++ Struct GapOp-3"
            << " [" << row << ';' << col << "]"
            << (_gomoku_player == Player::BLACK ? " (Black)" : " (white)" )
            << std::endl;
    }

    const bool should_continue = up_to_bound || old_data != new_data;

    if (should_continue)
        update_cell_left_to_right(board, row, col + 1);
}

void GomokuPatternReconizer::update_cell_up_to_down(const GomokuGame& board, int row, int col, bool up_to_bound) {

}

void GomokuPatternReconizer::update_cell_upleft_to_downright(const GomokuGame& board, int row, int col, bool up_to_bound) {

}

void GomokuPatternReconizer::update_cell_upright_to_downleft(const GomokuGame& board, int row, int col, bool up_to_bound) {

}
