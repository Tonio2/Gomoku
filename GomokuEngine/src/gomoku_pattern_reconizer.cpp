
#include "gomoku_pattern_reconizer.h"
#include "gomoku_engine.h"
#include <cassert>

/** PatternCellData */

bool PatternCellData::operator!=(const PatternCellData &comp) const
{
    return sequence_length != comp.sequence_length || is_sequence_closed != comp.is_sequence_closed || structure_length != comp.structure_length || is_structure_closed != comp.is_structure_closed || previous_structure_length != comp.previous_structure_length || is_previous_structure_closed != comp.is_previous_structure_closed || is_gap_open_three != comp.is_gap_open_three;
}

PatternCellData PatternCellData::pre_bound_element()
{
    return PatternCellData{0, true, 0, false, 0, false, false};
}

bool PatternCellData::contains_structure() const
{
    return structure_length > 0 || is_gap_open_three;
}

std::ostream &operator<<(std::ostream &stream, const PatternCellData &c)
{
    stream << "[" << static_cast<int>(c.sequence_length)
           << (c.is_sequence_closed ? " X " : " O ")
           << static_cast<int>(c.structure_length)
           << (c.is_structure_closed ? "x (" : "o (")
           << static_cast<int>(c.previous_structure_length)
           << (c.is_previous_structure_closed ? "x" : "o")
           << ")";
    if (c.is_gap_open_three)
        stream << "-G3" << (c.is_gap_open_three_closed ? "x" : "o");

    stream << "]";

    return stream;
}

/** GomokuPatternReconizer */

GomokuPatternReconizer::GomokuPatternReconizer(Player player)
    : _gomoku_player(player),
      _pattern_direction_cell_matrices(static_cast<int>(PatternDirection::Count)),
      _pattern_direction_structure_maps(static_cast<int>(PatternDirection::Count))
{
}

void GomokuPatternReconizer::find_patterns_in_board(const GomokuGame &board)
{
    adjust_matrices_size(board);
    initialize_matrices_bounds();
    update_all_cells(board);
}

void GomokuPatternReconizer::update_patterns_with_move(const GomokuGame &board, const MoveResult &Count_move)
{
    for (CellChange change : Count_move.cell_changes)
    {
        update_cell(board, PatternCellIndex(GomokuCellIndex(change.row, change.col)));
    }
}

std::ostream &operator<<(std::ostream &stream, PatternDirection direction)
{
    switch (direction)
    {
    case PatternDirection::LeftToRight:
        stream << "→";
        break;
    case PatternDirection::UpToDown:
        stream << "↓";
        break;
    case PatternDirection::UpleftToDownright:
        stream << "↘";
        break;
    case PatternDirection::UprightToDownleft:
        stream << "↙";
        break;
    default:
        break;
    }
    return stream;
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Pattern(" << _gomoku_player << "):" << std::endl;
    for_each_structures([](PatternCellIndex index, PatternCellData data, PatternDirection direction)
                        {
        GomokuCellIndex game_index = index.to_game_index();
        std::cout << direction << " " << data << " [" << int(game_index.row) << ';' << int(game_index.col) << "]" << std::endl; });
}

std::vector<int> GomokuPatternReconizer::get_pattern_count()
{
    std::vector<int> pattern_count(8, 0);
    for (int direction = 0; direction < PatternDirection::Count; ++direction)
    {
        const auto &structure_map = _pattern_direction_structure_maps[direction];
        for (auto structure_set_row : structure_map)
        {
            int row = structure_set_row.first;
            for (int col : structure_set_row.second)
            {

                PatternCellIndex index(row, col);
                PatternCellData data = _pattern_direction_cell_matrices[direction](row, col);
                GomokuCellIndex game_index = index.to_game_index();

                if (data.structure_length == 4 && !data.is_structure_closed)
                {
                    pattern_count[0]++;
                }
                else if (data.structure_length == 4 && data.is_structure_closed)
                {
                    pattern_count[1]++;
                }
                else if (data.structure_length == 3 && !data.is_structure_closed)
                {
                    pattern_count[2]++;
                }
                else if (data.structure_length == 3 && data.is_structure_closed)
                {
                    pattern_count[3]++;
                }
                else if (data.is_gap_open_three && !data.is_gap_open_three_closed)
                {
                    pattern_count[2]++;
                }
                else if (data.is_gap_open_three && data.is_gap_open_three_closed)
                {
                    pattern_count[3]++;
                }
                else if (data.structure_length == 2 && !data.is_structure_closed)
                {
                    pattern_count[4]++;
                }
                else if (data.structure_length == 2 && data.is_structure_closed)
                {
                    pattern_count[5]++;
                }
                else if (data.structure_length == 1 && !data.is_structure_closed)
                {
                    pattern_count[6]++;
                }
                else if (data.structure_length == 1 && data.is_structure_closed)
                {
                    pattern_count[7]++;
                }
            }
        }
    }
    return pattern_count;
}

PatternCellState GomokuPatternReconizer::cell_state_at(const GomokuGame &board, PatternCellIndex index) const
{

    const GomokuCellIndex game_index = index.to_game_index();

    if (!board.coordinates_are_valid(game_index.row, game_index.col))
        return PatternCellState::CELL_STATE_BLOCK;

    Player player = Player(board.get_board_value(game_index.row, game_index.col));

    if (player == _gomoku_player)
        return PatternCellState::CELL_STATE_STONE;
    if (player == Player::EMPTY)
        return PatternCellState::CELL_STATE_EMPTY;
    return PatternCellState::CELL_STATE_BLOCK;
}

PatternCellData GomokuPatternReconizer::cell_data_following(const PatternCellData &cell, PatternCellState state) const
{
    PatternCellData result;

    switch (state)
    {
    case CELL_STATE_EMPTY:
        result.sequence_length = 0;
        result.is_sequence_closed = false;
        result.structure_length = cell.sequence_length;
        result.is_structure_closed = cell.is_sequence_closed;
        result.previous_structure_length = result.structure_length;
        result.is_previous_structure_closed = result.is_structure_closed;
        result.is_gap_open_three =
            (cell.previous_structure_length == 2 && result.structure_length == 1) || (cell.previous_structure_length == 1 && result.structure_length == 2);
        result.is_gap_open_three_closed =
            result.is_gap_open_three ? cell.is_previous_structure_closed : false;
        break;
    case CELL_STATE_STONE:
        result.sequence_length = cell.sequence_length + 1;
        result.is_sequence_closed = cell.is_sequence_closed;
        result.structure_length = 0;
        result.is_structure_closed = 0;
        result.previous_structure_length = cell.previous_structure_length;
        result.is_previous_structure_closed = cell.is_previous_structure_closed;
        result.is_gap_open_three = false;
        result.is_gap_open_three_closed = false;
        break;
    case CELL_STATE_BLOCK:
        result.sequence_length = 0;
        result.is_sequence_closed = true;
        const bool structure_present = !cell.is_sequence_closed && cell.sequence_length > 0;
        result.structure_length = structure_present ? cell.sequence_length : 0;
        result.is_structure_closed = structure_present;
        result.previous_structure_length = 0;
        result.is_previous_structure_closed = 0;
        result.is_gap_open_three = !cell.is_previous_structure_closed && ((cell.previous_structure_length == 2 && result.structure_length == 1) || (cell.previous_structure_length == 1 && result.structure_length == 2));
        result.is_gap_open_three_closed = result.is_gap_open_three;
        break;
    };
    return result;
}

bool GomokuPatternReconizer::adjust_matrices_size(const GomokuGame &board)
{
    bool modified_matrices = false;

    for (int i = 0; i < PatternDirection::Count; ++i)
    {
        int width = board.get_board_width() + 2;
        int height = board.get_board_height() + 2;

        const bool size_differ = _pattern_direction_cell_matrices[i].get_width() != width || _pattern_direction_cell_matrices[i].get_height() != height;

        if (size_differ)
        {
            _pattern_direction_cell_matrices[i] = Matrix<PatternCellData>(width, height);
            modified_matrices = true;
        }
    }

    return modified_matrices;
}

void GomokuPatternReconizer::initialize_matrices_bounds()
{

    PatternCellData out_cell = PatternCellData::pre_bound_element();

    for (int i = 0; i < PatternDirection::Count; ++i)
    {

        Matrix<PatternCellData> &mat(_pattern_direction_cell_matrices[i]);

        for (int row = 0; row < mat.get_height(); ++row)
        {
            mat(row, 0) = out_cell;
            mat(row, mat.get_width() - 1) = out_cell;
        }
        for (int col = 0; col < mat.get_width(); ++col)
        {
            mat(0, col) = out_cell;
            mat(mat.get_height() - 1, col) = out_cell;
        }
    }
}

void GomokuPatternReconizer::update_all_cells(const GomokuGame &board)
{

    {
        const PatternDirection direction = PatternDirection::LeftToRight;
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        PatternCellIndex index(1, 1);
        for (index.row = 1; index.row < height; ++index.row)
        {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpToDown;
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        PatternCellIndex index(1, 1);
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpleftToDownright;
        PatternCellIndex index(1, 1);
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        for (index.row = 1; index.row < height; ++index.row)
        {
            update_cell_line(board, index, direction, true);
        }
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        index.row = 1;
        for (index.col = 2; index.col < width; ++index.col)
        {
            update_cell_line(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UprightToDownleft;
        PatternCellIndex index(1, 1);
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_line(board, index, direction, true);
        }
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        index.col = width - 1;
        for (index.row = 2; index.row < height; ++index.row)
        {
            update_cell_line(board, index, direction, true);
        }
    }
}

void GomokuPatternReconizer::update_cell(const GomokuGame &board, PatternCellIndex index)
{
    for (int i = 0; i < PatternDirection::Count; ++i)
    {
        update_cell_line(board, index, PatternDirection(i));
    }
}

void GomokuPatternReconizer::update_cell_line(const GomokuGame &board, PatternCellIndex index, PatternDirection direction, bool up_to_bound)
{
    Matrix<PatternCellData> &cell_matrix(_pattern_direction_cell_matrices[direction]);

    assert(get_previous_index(index, direction).is_valid(cell_matrix));

    if (index.col >= cell_matrix.get_width() || index.row >= cell_matrix.get_height())
        return;

    const PatternCellState cell_state = cell_state_at(board, index);

    const PatternCellData old_data = cell_matrix(index.row, index.col);

    const PatternCellIndex prev = get_previous_index(index, direction);
    const PatternCellData new_data = cell_data_following(cell_matrix(prev.row, prev.col), cell_state);

    cell_matrix(index.row, index.col) = new_data;

    if (old_data.contains_structure() && !new_data.contains_structure())
        untag_celldata_structure(index, direction);

    if (!old_data.contains_structure() && new_data.contains_structure())
        tag_celldata_structure(index, direction);

    const bool should_continue = up_to_bound || old_data != new_data;

    if (should_continue)
    {
        const PatternCellIndex next = get_next_index(index, direction);
        if (next.is_valid(cell_matrix))
        {
            update_cell_line(board, next, direction, up_to_bound);
        }
    }
}

PatternCellIndex GomokuPatternReconizer::get_previous_index(PatternCellIndex index, PatternDirection direction) const
{
    switch (direction)
    {
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

PatternCellIndex GomokuPatternReconizer::get_next_index(PatternCellIndex index, PatternDirection direction) const
{
    switch (direction)
    {
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

void GomokuPatternReconizer::untag_celldata_structure(PatternCellIndex index, PatternDirection direction)
{
    _pattern_direction_structure_maps[direction][index.row].erase(index.col);
}

void GomokuPatternReconizer::tag_celldata_structure(PatternCellIndex index, PatternDirection direction)
{
    _pattern_direction_structure_maps[direction][index.row].insert(index.col);
}

void GomokuPatternReconizer::for_each_structures(std::function<void(PatternCellIndex, PatternCellData, PatternDirection)> lambda)
{
    for (int direction = 0; direction < PatternDirection::Count; ++direction)
    {
        const auto &structure_map = _pattern_direction_structure_maps[direction];
        for (auto structure_set_row : structure_map)
        {
            int row = structure_set_row.first;
            for (int col : structure_set_row.second)
            {

                PatternCellIndex index(row, col);
                PatternCellData data = _pattern_direction_cell_matrices[direction](row, col);

                lambda(index, data, PatternDirection(direction));
            }
        }
    }
}
