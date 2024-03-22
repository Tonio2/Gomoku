
#include "gomoku_pattern_reconizer.h"
#include "gomoku_engine.h"
#include <cassert>

/** PatternCellState */

std::ostream &operator<<(std::ostream &stream, PatternCellState cell_state)
{
    switch (cell_state)
    {
    case PatternCellState::Empty:
        stream << "_";
        break;
    case PatternCellState::Stoned:
        stream << "O";
        break;
    case PatternCellState::Blocked:
        stream << "X";
        break;
    }
    return stream;
}

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

void PatternCellData::get_structures_type_count(std::vector<int> &array, int factor) const
{
    if (structure_length > 0 && structure_length < 5)
    {
        int index = structure_length * 2;
        index += is_structure_closed ? 1 : 0;
        array[index] += factor;
    }

    if (structure_length >= 5)
        array[StructureType::FIVE_OR_MORE] += factor;

    if (is_gap_open_three)
    {
        if (is_gap_open_three_closed)
        {
            array[StructureType::THREE] += factor;

            if (is_structure_closed)
            {
                // Closed open three because of previous structure
                if (structure_length == 1)
                {
                    // closed 1 + open 2
                    array[StructureType::ONE] -= factor;
                    array[StructureType::OPEN_TWO] -= factor;
                }
                else
                {
                    // closed 2 + open 1
                    array[StructureType::TWO] -= factor;
                    array[StructureType::OPEN_ONE] -= factor;
                }
            }
            else
            {
                if (structure_length == 2)
                {
                    // closed 1 + open 2
                    array[StructureType::ONE] -= factor;
                    array[StructureType::OPEN_TWO] -= factor;
                }
                else
                {
                    // closed 2 + open 1
                    array[StructureType::TWO] -= factor;
                    array[StructureType::OPEN_ONE] -= factor;
                }
            }
        }
        else
        {
            array[StructureType::OPEN_THREE] += factor;

            array[StructureType::OPEN_ONE] -= factor;
            array[StructureType::OPEN_TWO] -= factor;
        }
    }
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
    else
        stream << "    ";

    stream << "]";

    return stream;
}

/** PatternCellIndex */

PatternCellIndex::PatternCellIndex(int row, int col) : Matrix<PatternCellData>::Index(row, col)
{
}

PatternCellIndex::PatternCellIndex(GomokuCellIndex gomoku_index) : Matrix<PatternCellData>::Index(gomoku_index.row + 1, gomoku_index.col + 1)
{
}

GomokuCellIndex PatternCellIndex::to_game_index() const
{
    return GomokuCellIndex(row - 1, col - 1);
}

/** PatternDirection */

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

/** GomokuPatternReconizer */

GomokuPatternReconizer::GomokuPatternReconizer(Player player)
    : _gomoku_player(player),
      _pattern_direction_cell_matrices(static_cast<int>(PatternDirection::Count_PatternDirection)),
      _pattern_direction_structure_maps(static_cast<int>(PatternDirection::Count_PatternDirection)),
      _cached_pattern_count(static_cast<int>(StructureType::COUNT_STRUCTURE_TYPE), 0)
{
}

GomokuPatternReconizer::GomokuPatternReconizer(const GomokuPatternReconizer &copy)
    : _gomoku_player(copy._gomoku_player),
      _pattern_direction_cell_matrices(copy._pattern_direction_cell_matrices),
      _pattern_direction_structure_maps(copy._pattern_direction_structure_maps),
      _cached_pattern_count(copy._cached_pattern_count)
{
}

GomokuPatternReconizer &GomokuPatternReconizer::operator=(const GomokuPatternReconizer &copy)
{
    if (this != &copy)
    {
        _gomoku_player = copy._gomoku_player;
        _pattern_direction_cell_matrices = copy._pattern_direction_cell_matrices;
        _pattern_direction_structure_maps = copy._pattern_direction_structure_maps;
        _cached_pattern_count = copy._cached_pattern_count;
    }
    return *this;
}

GomokuPatternReconizer::~GomokuPatternReconizer()
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
    TIMER
    for (CellChange change : Count_move.cell_changes)
    {
        update_cell(board, PatternCellIndex(GomokuCellIndex(change.row, change.col)));
    }
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Pattern(" << _gomoku_player << "):" << std::endl;
    for_each_structures(
        [](PatternCellIndex index, PatternCellData data, PatternDirection direction)
        {
        GomokuCellIndex game_index = index.to_game_index();
        std::cout << "  " << direction << " " << data << " [" << int(game_index.row) << ';' << int(game_index.col) << "]" << std::endl; });

    std::cout << "  cached:" << std::endl;
    for (size_t i = 0; i < _cached_pattern_count.size(); i++)
    {
        std::cout << "    " << StructureType(i) << ':' << _cached_pattern_count[i] << std::endl;
    }
}

const std::vector<int> &GomokuPatternReconizer::get_pattern_count()
{
    return _cached_pattern_count;
}

bool GomokuPatternReconizer::five_or_more_cant_be_captured(GomokuGame &game) const
{
    for (int dir = 0; dir < PatternDirection::Count_PatternDirection; ++dir)
    {
        for (int row = 1; row < game.get_board_height() + 1; ++row)
        {
            for (int col = 1; col < game.get_board_width() + 1; ++col)
            {
                PatternCellIndex index(row, col);
                PatternCellData data = _pattern_direction_cell_matrices[dir][index];

                if (data.structure_length >= 5)
                {
                    for (int i = 1; i <= data.structure_length + 1; ++i)
                    {
                        PatternCellIndex index2 = get_index_offset(index, PatternDirection(dir), -i);
                        GomokuCellIndex gameIndex2 = index2.to_game_index();
                        for (int dir2 = 0; dir2 < PatternDirection::Count_PatternDirection; ++dir2)
                        {
                            if (dir2 == dir)
                                continue;
                            std::pair<int, int> dir_coordinates = get_direction_offset(PatternDirection(dir2));
                            int dir2x = dir_coordinates.first;
                            int dir2y = dir_coordinates.second;

                            if (game.coordinates_are_valid(gameIndex2.row + 2 * dir2x, gameIndex2.col + 2 * dir2y))
                            {
                                Player cellValue1 = game.get_board_value(gameIndex2.row + 2 * dir2x, gameIndex2.col + 2 * dir2y);
                                if (cellValue1 == E && game.try_direction_for_capture_without_capturing(gameIndex2.row + 2 * dir2x, gameIndex2.col + 2 * dir2y, -dir2x, -dir2y, game.other_player(_gomoku_player)))
                                    return false;
                            }

                            if (game.coordinates_are_valid(gameIndex2.row - 2 * dir2x, gameIndex2.col - 2 * dir2y))
                            {
                                Player cellValue2 = game.get_board_value(gameIndex2.row - 2 * dir2x, gameIndex2.col - 2 * dir2y);
                                if (cellValue2 == E && game.try_direction_for_capture_without_capturing(gameIndex2.row - 2 * dir2x, gameIndex2.col - 2 * dir2y, dir2x, dir2y, game.other_player(_gomoku_player)))
                                    return false;
                            }

                            if (game.coordinates_are_valid(gameIndex2.row + 1 * dir2x, gameIndex2.col + 1 * dir2y))
                            {
                                Player cellValue1 = game.get_board_value(gameIndex2.row + 1 * dir2x, gameIndex2.col + 1 * dir2y);
                                if (cellValue1 == E && game.try_direction_for_capture_without_capturing(gameIndex2.row + 1 * dir2x, gameIndex2.col + 1 * dir2y, -dir2x, -dir2y, game.other_player(_gomoku_player)))
                                    return false;
                            }

                            if (game.coordinates_are_valid(gameIndex2.row - 1 * dir2x, gameIndex2.col - 1 * dir2y))
                            {
                                Player cellValue2 = game.get_board_value(gameIndex2.row - 1 * dir2x, gameIndex2.col - 1 * dir2y);
                                if (cellValue2 == E && game.try_direction_for_capture_without_capturing(gameIndex2.row - 1 * dir2x, gameIndex2.col - 1 * dir2y, dir2x, dir2y, game.other_player(_gomoku_player)))
                                    return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool GomokuPatternReconizer::can_be_captured(GomokuGame &game)
{
    for (int dir = 0; dir < PatternDirection::Count_PatternDirection; ++dir)
    {
        for (int row = 1; row < game.get_board_height() + 1; ++row)
        {
            for (int col = 1; col < game.get_board_width() + 1; ++col)
            {
                PatternCellIndex index(row, col);
                PatternCellData data = _pattern_direction_cell_matrices[dir][index];

                if (data.structure_length == 2 && data.is_structure_closed)
                {
                    std::pair<int, int> dir_coordinates = get_direction_offset(PatternDirection(dir));
                    int dirx = dir_coordinates.first;
                    int diry = dir_coordinates.second;
                    GomokuCellIndex gameIndex = index.to_game_index();
                    Player cellValue = game.get_board_value(gameIndex.row, gameIndex.col);
                    if (cellValue == E)
                    {
                        if (game.try_direction_for_capture_without_capturing(gameIndex.row, gameIndex.col, -dirx, -diry, game.other_player(_gomoku_player)))
                            return true;
                    }
                    else
                    {
                        PatternCellIndex otherSideIndex = get_index_offset(index, PatternDirection(dir), -3);
                        GomokuCellIndex otherSideGameIndex = otherSideIndex.to_game_index();
                        if (game.coordinates_are_valid(otherSideGameIndex.row, otherSideGameIndex.col))
                        {
                            if (game.try_direction_for_capture_without_capturing(otherSideGameIndex.row, otherSideGameIndex.col, dirx, diry, game.other_player(_gomoku_player)))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

PatternCellState GomokuPatternReconizer::cell_state_at(const GomokuGame &board, PatternCellIndex index) const
{

    const GomokuCellIndex game_index = index.to_game_index();

    if (!board.coordinates_are_valid(game_index.row, game_index.col))
        return PatternCellState::Blocked;

    Player player = Player(board.get_board_value(game_index.row, game_index.col));

    if (player == _gomoku_player)
        return PatternCellState::Stoned;
    if (player == Player::EMPTY)
        return PatternCellState::Empty;
    return PatternCellState::Blocked;
}

PatternCellData GomokuPatternReconizer::cell_data_following(const PatternCellData &cell, PatternCellState state) const
{
    PatternCellData result;

    switch (state)
    {
    case Empty:
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
    case Stoned:
        result.sequence_length = cell.sequence_length + 1;
        result.is_sequence_closed = cell.is_sequence_closed;
        result.structure_length = 0;
        result.is_structure_closed = 0;
        result.previous_structure_length = cell.previous_structure_length;
        result.is_previous_structure_closed = cell.is_previous_structure_closed;
        result.is_gap_open_three = false;
        result.is_gap_open_three_closed = false;
        break;
    case Blocked:
        result.sequence_length = 0;
        result.is_sequence_closed = true;
        const bool structure_present = (!cell.is_sequence_closed && cell.sequence_length > 0) || cell.sequence_length >= 5;
        result.structure_length = structure_present ? cell.sequence_length : 0;
        result.is_structure_closed = structure_present;
        result.previous_structure_length = 0;
        result.is_previous_structure_closed = 0;
        result.is_gap_open_three = !cell.is_previous_structure_closed && ((cell.previous_structure_length == 2 && result.structure_length == 1) || (cell.previous_structure_length == 1 && result.structure_length == 2));
        result.is_gap_open_three_closed = result.is_gap_open_three;
        break;
    };

    if (result.is_gap_open_three)
    {
        result.previous_structure_length = 0;
        result.is_previous_structure_closed = false;
    }

    return result;
}

const PatternCellData &GomokuPatternReconizer::cell_data_following_memoized(const PatternCellData &cell, PatternCellState state) const
{
    const int max_seq_len = 10;
    const int max_prev_len = 3;
    static PatternCellData cell_values[3][max_seq_len][2][max_prev_len][2];
    static bool initialized = false;

    if (!initialized)
    {
        PatternCellData prev;
        prev.structure_length = 0;
        prev.is_structure_closed = false;
        prev.is_gap_open_three = false;
        prev.is_gap_open_three_closed = false;
        for (int s = 0; s < 3; ++s)
        {
            for (int seq_len = 0; seq_len < max_seq_len; ++seq_len)
            {
                for (int seq_closed = 0; seq_closed < 2; ++seq_closed)
                {
                    for (int prev_len = 0; prev_len < max_prev_len; ++prev_len)
                    {
                        for (int prev_closed = 0; prev_closed < 2; ++prev_closed)
                        {
                            prev.sequence_length = seq_len;
                            prev.is_sequence_closed = bool(seq_closed);
                            prev.previous_structure_length = prev_len;
                            prev.is_previous_structure_closed = bool(prev_closed);

                            PatternCellState ss = static_cast<PatternCellState>(s);
                            PatternCellData current = cell_data_following(prev, ss);

                            cell_values[ss]
                                       [seq_len]
                                       [seq_closed]
                                       [prev_len]
                                       [prev_closed] = current;
                        }
                    }
                }
            }
        }
        initialized = true;
    }

    return cell_values
        [state]
        [cell.sequence_length]
        // [cell.sequence_length >= max_seq_len ? max_seq_len : cell.sequence_length]
        [cell.is_sequence_closed]
        // [cell.previous_structure_length >= max_prev_len ? max_prev_len : cell.previous_structure_length]
        [cell.previous_structure_length]
        [cell.is_previous_structure_closed];
}

bool GomokuPatternReconizer::adjust_matrices_size(const GomokuGame &board)
{
    bool modified_matrices = false;

    for (int i = 0; i < PatternDirection::Count_PatternDirection; ++i)
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

    for (int i = 0; i < PatternDirection::Count_PatternDirection; ++i)
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
            update_cell_direction(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpToDown;
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        PatternCellIndex index(1, 1);
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_direction(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpleftToDownright;
        PatternCellIndex index(1, 1);
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        for (index.row = 1; index.row < height; ++index.row)
        {
            update_cell_direction(board, index, direction, true);
        }
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        index.row = 1;
        for (index.col = 2; index.col < width; ++index.col)
        {
            update_cell_direction(board, index, direction, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UprightToDownleft;
        PatternCellIndex index(1, 1);
        const int width = _pattern_direction_cell_matrices[direction].get_width() - 1;
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_direction(board, index, direction, true);
        }
        const int height = _pattern_direction_cell_matrices[direction].get_height() - 1;
        index.col = width - 1;
        for (index.row = 2; index.row < height; ++index.row)
        {
            update_cell_direction(board, index, direction, true);
        }
    }
}

void GomokuPatternReconizer::update_cell(const GomokuGame &board, PatternCellIndex index)
{
    for (int i = 0; i < PatternDirection::Count_PatternDirection; ++i)
    {
        update_cell_direction(board, index, PatternDirection(i));
    }
}

void GomokuPatternReconizer::update_cell_direction(const GomokuGame &board, PatternCellIndex index, PatternDirection direction, bool up_to_bound)
{
    Matrix<PatternCellData> &cell_matrix(_pattern_direction_cell_matrices[direction]);

    assert(index.is_valid(cell_matrix));
    assert(get_index_offset(index, direction, -1).is_valid(cell_matrix));

    const PatternCellData old_data = cell_matrix[index];

    const PatternCellData new_data = cell_data_following_memoized(
        cell_matrix[get_index_offset(index, direction, -1)],
        cell_state_at(board, index));

    old_data.get_structures_type_count(_cached_pattern_count, -1);
    new_data.get_structures_type_count(_cached_pattern_count, 1);

    // if (old_data.contains_structure() && !new_data.contains_structure())
    //     untag_celldata_structure(index, direction);

    cell_matrix[index] = new_data;

    // if (!old_data.contains_structure() && new_data.contains_structure())
    //     tag_celldata_structure(index, direction);

    const bool should_continue = up_to_bound || old_data != new_data;

    if (should_continue)
    {
        const PatternCellIndex next = get_index_offset(index, direction, 1);
        if (next.is_valid(cell_matrix))
        {
            update_cell_direction(board, next, direction, up_to_bound);
        }
    }
}

std::pair<int, int> GomokuPatternReconizer::get_direction_offset(PatternDirection direction)
{
    switch (direction)
    {
    case PatternDirection::LeftToRight:
        return std::make_pair(0, 1);
    case PatternDirection::UpToDown:
        return std::make_pair(1, 0);
    case PatternDirection::UpleftToDownright:
        return std::make_pair(1, 1);
    case PatternDirection::UprightToDownleft:
        return std::make_pair(1, -1);
    default:
        return std::make_pair(0, 0);
    }
}

PatternCellIndex GomokuPatternReconizer::get_index_offset(PatternCellIndex index, PatternDirection direction, int distance)
{
    switch (direction)
    {
    case PatternDirection::LeftToRight:
        return PatternCellIndex(index.row, index.col + distance);
    case PatternDirection::UpToDown:
        return PatternCellIndex(index.row + distance, index.col);
    case PatternDirection::UpleftToDownright:
        return PatternCellIndex(index.row + distance, index.col + distance);
    case PatternDirection::UprightToDownleft:
        return PatternCellIndex(index.row + distance, index.col - distance);
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
    for (int direction = 0; direction < PatternDirection::Count_PatternDirection; ++direction)
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
