
#include "engine/gomoku_engine.h"
#include "engine/gomoku_pattern_reconizer.h"
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

PatternCellData::PatternCellData(uint16_t data) : _data(data)
{
    assert((_data & 0b1100000000000000) == 0);
}

PatternCellData::PatternCellData(uint8_t sequence_length,
                                 bool is_sequence_closed,
                                 uint8_t structure_length,
                                 bool is_structure_closed,
                                 uint8_t previous_structure_length,
                                 bool is_previous_structure_closed,
                                 bool is_gap_open_three,
                                 bool is_gap_open_three_closed)
    : _data(0)
{
    _data |= std::min(sequence_length, uint8_t(15));
    _data |= is_sequence_closed ? 0b10000 : 0;
    _data |= std::min(structure_length, uint8_t(7)) << 5;
    _data |= is_structure_closed ? 0b100000000 : 0;
    _data |= std::min(previous_structure_length, uint8_t(3)) << 9;
    _data |= is_previous_structure_closed ? 0b100000000000 : 0;
    _data |= is_gap_open_three ? 0b1000000000000 : 0;
    _data |= is_gap_open_three_closed ? 0b10000000000000 : 0;

    assert((_data & 0b1100000000000000) == 0);
}

/*
 * IsClosGop(1) | IsGop(1) | PrevStructClos(1) | PrevStructLen(2) | StructClos(1) | StructLen(3) | SeqClos(1) | SeqLen(4)
 * 0              0          0                   00                 0               000            0            0000
 *  0  0  0 00 0 000 0 0000
 * 13 12 11  9 8   5 4    0
 */

uint8_t PatternCellData::sequence_length() const
{
    return _data & 0b1111;
}

bool PatternCellData::is_sequence_closed() const
{
    return (_data & 0b10000) != 0;
}

uint8_t PatternCellData::structure_length() const
{
    return (_data & 0b11100000) >> 5;
}

bool PatternCellData::is_structure_closed() const
{
    return (_data & 0b100000000) != 0;
}

uint8_t PatternCellData::previous_structure_length() const
{
    return (_data & 0b11000000000) >> 9;
}

bool PatternCellData::is_previous_structure_closed() const
{
    return (_data & 0b100000000000) != 0;
}

bool PatternCellData::is_gap_open_three() const
{
    return (_data & 0b1000000000000) != 0;
}

bool PatternCellData::is_gap_open_three_closed() const
{
    return (_data & 0b10000000000000) != 0;
}

bool PatternCellData::operator!=(const PatternCellData &comp) const
{
    return _data != comp._data;
}

PatternCellData PatternCellData::pre_bound_element()
{
    return PatternCellData(0, true, 0, false, 0, false, false, false);
}

bool PatternCellData::contains_structure() const
{
    return structure_length() > 0 || is_gap_open_three();
}

StructureType PatternCellData::get_relevant_structure() const
{
    if (is_gap_open_three())
    {
        return is_gap_open_three_closed() ? StructureType::THREE : StructureType::OPEN_THREE;
    }

    if (structure_length() >= 5)
        return StructureType::FIVE_OR_MORE;

    if (structure_length() > 0)
    {
        int type_index = structure_length() * 2;
        type_index += is_structure_closed() ? 1 : 0;
        return StructureType(type_index);
    }

    return StructureType::NONE;
}

void PatternCellData::get_structures_type_count(std::vector<int> &array, int factor) const
{
    if (structure_length() > 0 && structure_length() < 5)
    {
        int index = structure_length() * 2;
        index += is_structure_closed() ? 1 : 0;
        array[index] += factor;
    }

    if (structure_length() >= 5)
        array[StructureType::FIVE_OR_MORE] += factor;

    if (is_gap_open_three())
    {
        if (is_gap_open_three_closed())
        {
            array[StructureType::THREE] += factor;

            if (is_structure_closed())
            {
                // Closed open three because of previous structure
                if (structure_length() == 1)
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
                if (structure_length() == 2)
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

uint16_t PatternCellData::data() const
{
    return _data;
}

std::ostream &operator<<(std::ostream &stream, const PatternCellData &c)
{
    stream << "[" << static_cast<int>(c.sequence_length())
           << (c.is_sequence_closed() ? "x " : "o ")
           << static_cast<int>(c.structure_length())
           << (c.is_structure_closed() ? "x (" : "o (")
           << static_cast<int>(c.previous_structure_length())
           << (c.is_previous_structure_closed() ? "x" : "o")
           << ")";
    if (c.is_gap_open_three())
        stream << "-G3" << (c.is_gap_open_three_closed() ? "x" : "o");
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
      _cell_matrices(static_cast<int>(PatternDirection::Count_PatternDirection)),
      _structure_maps(static_cast<int>(PatternDirection::Count_PatternDirection)),
      _cached_pattern_count(static_cast<int>(StructureType::COUNT_STRUCTURE_TYPE), 0),
      _tagging_mode(false)
{
}

GomokuPatternReconizer::GomokuPatternReconizer(const GomokuPatternReconizer &copy)
    : _gomoku_player(copy._gomoku_player),
      _cell_matrices(copy._cell_matrices),
      _structure_maps(copy._structure_maps),
      _cached_pattern_count(copy._cached_pattern_count),
      _tagging_mode(copy._tagging_mode)
{
}

GomokuPatternReconizer &GomokuPatternReconizer::operator=(const GomokuPatternReconizer &copy)
{
    if (this != &copy)
    {
        _gomoku_player = copy._gomoku_player;
        _cell_matrices = copy._cell_matrices;
        _structure_maps = copy._structure_maps;
        _cached_pattern_count = copy._cached_pattern_count;
        _tagging_mode = copy._tagging_mode;
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
    for (const CellChange &change : Count_move.cell_changes)
    {
        update_cell(board, PatternCellIndex(GomokuCellIndex(change.row, change.col)));
    }
}

void GomokuPatternReconizer::print_patterns()
{
    std::cout << "Pattern(" << _gomoku_player << "):" << std::endl;

    std::cout << " Tagged:" << std::endl;
    for_each_tagged_structures(
        [](PatternCellIndex index, const PatternCellData &data, PatternDirection direction, bool &should_continue)
        {
            std::cout << "  " << direction << " " << data << " [" << int(index.row) << ';' << int(index.col) << "]" << std::endl;
        });

    std::cout << " Cached: {";
    for (size_t i = 0; i < _cached_pattern_count.size(); i++)
    {
        std::cout << StructureType(i) << ':' << _cached_pattern_count[i] << (i == _cached_pattern_count.size() - 1 ? "" : ",");
    }
    std::cout << "}" << std::endl;
}

const std::vector<int> &GomokuPatternReconizer::get_pattern_count() const
{
    return _cached_pattern_count;
}

bool GomokuPatternReconizer::five_or_more_cant_be_captured(const GomokuGame &board)
{
    // TODO: Check intersection of five or more patterns with blocked two
    int five_count = get_pattern_count()[StructureType::FIVE_OR_MORE];
    int five_capturables = 0;

    for_each_tagged_structures(
        [this, &five_capturables, &board](PatternCellIndex index, const PatternCellData &data, PatternDirection direction, bool &should_continue)
        {
            const uint8_t length = data.structure_length();
            if (length >= 5)
            {
                for (uint8_t i = 1; i <= length; ++i)
                {
                    PatternCellIndex struct_index = get_index_offset(index, direction, -i);

                    for (int dir = 0; dir < PatternDirection::Count_PatternDirection; ++dir)
                    {
                        if (dir == direction)
                            continue;

                        auto structure = get_structure_at(struct_index, PatternDirection(dir));

                        const PatternCellData &cell_data = _cell_matrices[dir][PatternCellIndex(structure.second)];
                        if (is_structure_capturable(board, structure.second, cell_data, PatternDirection(dir)))
                        {
                            ++five_capturables;
                            return;
                        }
                    }
                }
            }
        });

    return five_count > five_capturables;
}

bool GomokuPatternReconizer::can_be_captured(const GomokuGame &board)
{
    if (get_pattern_count()[StructureType::TWO] <= 0)
        return false;

    bool capturable = false;

    for_each_tagged_structures(
        [this, &capturable, &board](PatternCellIndex index, const PatternCellData &data, PatternDirection direction, bool &should_continue)
        {
            if (is_structure_capturable(board, index, data, direction))
            {
                capturable = true;
                should_continue = false;
                return;
            }
        });

    return capturable;
}

std::pair<StructureType, PatternCellIndex> GomokuPatternReconizer::get_structure_at(PatternCellIndex index, PatternDirection direction, int min_distance) const
{
    const Matrix<PatternCellData> &cell_matrix(_cell_matrices[direction]);

    std::function<std::pair<StructureType, PatternCellIndex>(PatternCellIndex, int, bool, bool)> find_structure;

    find_structure = [&cell_matrix, &find_structure, direction](PatternCellIndex i, int distance, bool try_next, bool met_gap) -> std::pair<StructureType, PatternCellIndex>
    {
        if (!i.is_valid(cell_matrix))
            return std::make_pair(StructureType::NONE, i);

        const PatternCellData &cell_data(cell_matrix[i]);
        const PatternCellIndex next = get_index_offset(i, direction, 1);

        /** In case of stone, search the next cell */
        if (cell_data.sequence_length() > 0)
            return find_structure(next, distance - 1, false, met_gap);

        /** In case of hole or block, look for the length of the current structure */
        if (cell_data.structure_length() > 0)
        {
            /** If we're starting a new closed sequence, then it's a block. No need to go further */
            if (cell_data.is_sequence_closed())
                return std::make_pair(cell_data.get_relevant_structure(), i);

            /** case of possible three */
            if (!met_gap && (cell_data.structure_length() == 1 || cell_data.structure_length() == 2) && (!cell_data.is_gap_open_three()))
            {
                const std::pair<StructureType, PatternCellIndex> next_structure = find_structure(next, distance - 1, false, true);

                /** If the next structure is a three or open three then it's possibly a continuation of this structure */
                if (next_structure.first == StructureType::OPEN_THREE || next_structure.first == StructureType::THREE)
                {
                    /** If the next structure is itself a different length than three, then it's the second part of this structure */
                    if (cell_matrix[PatternCellIndex(next_structure.second)].structure_length() != 3)
                        return next_structure;
                }
            }

            return std::make_pair(cell_data.get_relevant_structure(), i);
        }

        if (try_next || distance > 0)
            return find_structure(next, distance - 1, false, met_gap);

        return std::make_pair(StructureType::NONE, i);
    };

    return find_structure(PatternCellIndex(index), min_distance, true, false);
}

StructureType GomokuPatternReconizer::highest_structure_around(PatternCellIndex index, int distance) const
{
    StructureType highest_structure = StructureType::NONE;
    for (int i = 0; i < PatternDirection::Count_PatternDirection; ++i)
    {
        PatternCellIndex offset = get_index_offset(index, PatternDirection(i), -distance);
        int8_t overflow = 0;
        if (!offset.is_valid(_cell_matrices[i]))
        {
            if (offset.col < 0)
            {
                overflow = -offset.col;
                offset = get_index_offset(offset, PatternDirection(i), overflow);
            }
            else if (offset.col >= _cell_matrices[i].get_width())
            {
                overflow = offset.col - _cell_matrices[i].get_width() + 1;
                offset = get_index_offset(offset, PatternDirection(i), -overflow);
            }
            else if (offset.row < 0)
            {
                overflow = -offset.row;
                offset = get_index_offset(offset, PatternDirection(i), overflow);
            }
            else if (offset.row >= _cell_matrices[i].get_height())
            {
                overflow = offset.row - _cell_matrices[i].get_height() + 1;
                offset = get_index_offset(offset, PatternDirection(i), -overflow);
            }
        }
        auto struct_at = get_structure_at(offset, PatternDirection(i), distance * 2 + 1 - overflow);
        if (compare_structure_type(highest_structure, struct_at.first))
            highest_structure = struct_at.first;
    }
    return highest_structure;
}

const Matrix<PatternCellData> &GomokuPatternReconizer::get_pattern_cell_matrix(PatternDirection direction) const
{
    return _cell_matrices[direction];
}

PatternCellState GomokuPatternReconizer::cell_state_at(const GomokuGame &board, PatternCellIndex index) const
{
    assert(board.pattern_coordinate_is_valid(index));

    const Player player = board.get_board_value(index.row - 1, index.col - 1);

    if (player == _gomoku_player)
        return PatternCellState::Stoned;
    if (player == Player::EMPTY)
        return PatternCellState::Empty;
    return PatternCellState::Blocked;
}

PatternCellData GomokuPatternReconizer::cell_data_following(const PatternCellData &cell, PatternCellState state) const
{
    uint8_t sequence_length;
    bool is_sequence_closed;
    uint8_t structure_length;
    bool is_structure_closed;
    uint8_t previous_structure_length;
    bool is_previous_structure_closed;
    bool is_gap_open_three;
    bool is_gap_open_three_closed;

    switch (state)
    {
    case Empty:
        sequence_length = 0;
        is_sequence_closed = false;
        structure_length = cell.sequence_length();
        is_structure_closed = cell.is_sequence_closed();
        previous_structure_length = structure_length;
        is_previous_structure_closed = is_structure_closed;
        is_gap_open_three =
            (cell.previous_structure_length() == 2 && structure_length == 1) || (cell.previous_structure_length() == 1 && structure_length == 2);
        is_gap_open_three_closed =
            is_gap_open_three ? cell.is_previous_structure_closed() : false;
        break;
    case Stoned:
        sequence_length = cell.sequence_length() + 1;
        is_sequence_closed = cell.is_sequence_closed();
        structure_length = 0;
        is_structure_closed = 0;
        previous_structure_length = cell.previous_structure_length();
        is_previous_structure_closed = cell.is_previous_structure_closed();
        is_gap_open_three = false;
        is_gap_open_three_closed = false;
        break;
    case Blocked:
        sequence_length = 0;
        is_sequence_closed = true;
        const bool structure_present = (!cell.is_sequence_closed() && cell.sequence_length() > 0) || cell.sequence_length() >= 5;
        structure_length = structure_present ? cell.sequence_length() : 0;
        is_structure_closed = structure_present;
        previous_structure_length = 0;
        is_previous_structure_closed = 0;
        is_gap_open_three = !cell.is_previous_structure_closed() && ((cell.previous_structure_length() == 2 && structure_length == 1) || (cell.previous_structure_length() == 1 && structure_length == 2));
        is_gap_open_three_closed = is_gap_open_three;
        break;
    };

    if (is_gap_open_three)
    {
        previous_structure_length = 0;
        is_previous_structure_closed = false;
    }

    return PatternCellData(sequence_length,
                           is_sequence_closed,
                           structure_length,
                           is_structure_closed,
                           previous_structure_length,
                           is_previous_structure_closed,
                           is_gap_open_three,
                           is_gap_open_three_closed);
}

static uint16_t cell_and_state_to_index(PatternCellData cell, PatternCellState state)
{
    return (static_cast<uint16_t>(cell.data()) << 2) | static_cast<uint16_t>(state);
}

const PatternCellData &GomokuPatternReconizer::cell_data_following_memoized(const PatternCellData &cell, PatternCellState state) const
{
    const int combinaisons_count = 1 << 16;
    static PatternCellData cell_values[combinaisons_count];
    static bool initialized = false;

    if (!initialized)
    {
        for (uint16_t d = 0; d < 0b100000000000000; ++d)
        {
            PatternCellData cell_data(d);
            for (int state = 0; state <= 2; ++state)
            {
                PatternCellState ss = static_cast<PatternCellState>(state);
                PatternCellData current = cell_data_following(cell_data, ss);
                cell_values[cell_and_state_to_index(cell_data, ss)] = current;
            }
        }
        initialized = true;
    }

    return cell_values[cell_and_state_to_index(cell, state)];
}

bool GomokuPatternReconizer::adjust_matrices_size(const GomokuGame &board)
{
    bool modified_matrices = false;

    for (int i = 0; i < PatternDirection::Count_PatternDirection; ++i)
    {
        int width = board.get_board_width() + 2;
        int height = board.get_board_height() + 2;

        const bool size_differ = _cell_matrices[i].get_width() != width || _cell_matrices[i].get_height() != height;

        if (size_differ)
        {
            _cell_matrices[i] = Matrix<PatternCellData>(width, height);
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

        Matrix<PatternCellData> &mat(_cell_matrices[i]);

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
        const int height = _cell_matrices[PatternDirection::LeftToRight].get_height() - 1;
        PatternCellIndex index(1, 1);
        for (index.row = 1; index.row < height; ++index.row)
        {
            update_cell_left_to_right(board, index, true);
        }
    }

    {
        const int width = _cell_matrices[PatternDirection::UpToDown].get_width() - 1;
        PatternCellIndex index(1, 1);
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_up_to_down(board, index, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UpleftToDownright;
        PatternCellIndex index(1, 1);
        const int height = _cell_matrices[direction].get_height() - 1;
        for (index.row = 1; index.row < height; ++index.row)
        {
            update_cell_upleft_to_downright(board, index, true);
        }
        const int width = _cell_matrices[direction].get_width() - 1;
        index.row = 1;
        for (index.col = 2; index.col < width; ++index.col)
        {
            update_cell_upleft_to_downright(board, index, true);
        }
    }

    {
        const PatternDirection direction = PatternDirection::UprightToDownleft;
        PatternCellIndex index(1, 1);
        const int width = _cell_matrices[direction].get_width() - 1;
        for (index.col = 1; index.col < width; ++index.col)
        {
            update_cell_upright_to_downleft(board, index, true);
        }
        const int height = _cell_matrices[direction].get_height() - 1;
        index.col = width - 1;
        for (index.row = 2; index.row < height; ++index.row)
        {
            update_cell_upright_to_downleft(board, index, true);
        }
    }
}

void GomokuPatternReconizer::update_cell(const GomokuGame &board, PatternCellIndex index)
{
    update_cell_left_to_right(board, index);
    update_cell_up_to_down(board, index);
    update_cell_upleft_to_downright(board, index);
    update_cell_upright_to_downleft(board, index);
}

bool GomokuPatternReconizer::update_cell_direction(
    const GomokuGame &board,
    PatternCellIndex index,
    PatternDirection direction,
    PatternCellState new_state)
{
    Matrix<PatternCellData> &cell_matrix(_cell_matrices[direction]);

    assert(index.is_valid(cell_matrix));
    assert(get_index_offset(index, direction, -1).is_valid(cell_matrix));

    const PatternCellData old_data = cell_matrix[index];

    const PatternCellData new_data = cell_data_following_memoized(
        cell_matrix[get_index_offset(index, direction, -1)],
        new_state);

    old_data.get_structures_type_count(_cached_pattern_count, -1);
    new_data.get_structures_type_count(_cached_pattern_count, 1);

    if (_tagging_mode)
    {
        const bool old_data_relevant = is_relevant_to_tag(old_data);
        const bool new_data_relevant = is_relevant_to_tag(new_data);

        if (old_data_relevant && !new_data_relevant)
            untag_celldata_structure(index, direction);

        if (!old_data_relevant && new_data_relevant)
            tag_celldata_structure(index, direction);
    }

    cell_matrix[index] = new_data;

    return old_data != new_data;
}

void GomokuPatternReconizer::update_cell_left_to_right(const GomokuGame &board, PatternCellIndex index, bool up_to_bound)
{
    const PatternDirection direction = PatternDirection::LeftToRight;

    PatternCellState new_state =
        /* index.col - 1 < board.get_board_width() */
        (index.col <= board.get_board_width())
            ? cell_state_at(board, index)
            : PatternCellState::Blocked;

    const bool modified = update_cell_direction(board, index, direction, new_state);

    if (up_to_bound || modified)
    {
        if (++index.col < _cell_matrices[direction].get_width())
        {
            update_cell_left_to_right(board, index, up_to_bound);
        }
    }
}

void GomokuPatternReconizer::update_cell_up_to_down(const GomokuGame &board, PatternCellIndex index, bool up_to_bound)
{
    const PatternDirection direction = PatternDirection::UpToDown;

    PatternCellState new_state =
        /* index.row - 1 < board.get_board_height() */
        (index.row <= board.get_board_height())
            ? cell_state_at(board, index)
            : PatternCellState::Blocked;

    const bool modified = update_cell_direction(board, index, direction, new_state);

    if (up_to_bound || modified)
    {
        if (++index.row < _cell_matrices[direction].get_height())
        {
            update_cell_up_to_down(board, index, up_to_bound);
        }
    }
}

void GomokuPatternReconizer::update_cell_upleft_to_downright(const GomokuGame &board, PatternCellIndex index, bool up_to_bound)
{
    const PatternDirection direction = PatternDirection::UpleftToDownright;

    PatternCellState new_state =
        (index.col <= board.get_board_width() && index.row <= board.get_board_height())
            ? cell_state_at(board, index)
            : PatternCellState::Blocked;

    const bool modified = update_cell_direction(board, index, direction, new_state);

    if (up_to_bound || modified)
    {
        const Matrix<PatternCellData> &cell_matrix(_cell_matrices[direction]);
        if (++index.row < cell_matrix.get_height() && ++index.col < cell_matrix.get_width())
        {
            update_cell_upleft_to_downright(board, index, up_to_bound);
        }
    }
}

void GomokuPatternReconizer::update_cell_upright_to_downleft(const GomokuGame &board, PatternCellIndex index, bool up_to_bound)
{
    const PatternDirection direction = PatternDirection::UprightToDownleft;

    PatternCellState new_state =
        (index.col > 0 && index.row <= board.get_board_height())
            ? cell_state_at(board, index)
            : PatternCellState::Blocked;

    const bool modified = update_cell_direction(board, index, direction, new_state);

    if (up_to_bound || modified)
    {
        if (++index.row < _cell_matrices[direction].get_height() && --index.col >= 0)
        {
            update_cell_upright_to_downleft(board, index, up_to_bound);
        }
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

void GomokuPatternReconizer::activate_tagging_mode()
{
    if (_tagging_mode)
        return;

    _tagging_mode = true;

    for (int dir = 0; dir < PatternDirection::Count_PatternDirection; ++dir)
    {
        PatternCellIndex index(0, 0);
        for (index.row = 0; index.row < _cell_matrices[dir].get_height(); ++index.row)
        {
            for (index.col = 0; index.col < _cell_matrices[dir].get_width(); ++index.col)
            {
                const PatternCellData &cell_data = _cell_matrices[dir][index];

                if (is_relevant_to_tag(cell_data))
                {
                    tag_celldata_structure(index, PatternDirection(dir));
                }
            }
        }
    }
}

bool GomokuPatternReconizer::is_relevant_to_tag(const PatternCellData &data) const
{
    uint8_t length = data.structure_length();

    if (length >= 5)
        return true;

    if (length == 2 && data.is_structure_closed())
        return true;

    return false;
}

void GomokuPatternReconizer::untag_celldata_structure(PatternCellIndex index, PatternDirection direction)
{
    _structure_maps[direction][index.row].erase(index.col);
}

void GomokuPatternReconizer::tag_celldata_structure(PatternCellIndex index, PatternDirection direction)
{
    _structure_maps[direction][index.row].insert(index.col);
}

void GomokuPatternReconizer::for_each_tagged_structures(std::function<void(PatternCellIndex, const PatternCellData &, PatternDirection, bool &should_continue)> lambda)
{
    activate_tagging_mode();
    bool should_continue = true;
    for (int direction = 0; direction < PatternDirection::Count_PatternDirection; ++direction)
    {
        const auto &structure_map = _structure_maps[direction];
        for (auto structure_set_row : structure_map)
        {
            int row = structure_set_row.first;
            for (int col : structure_set_row.second)
            {

                PatternCellIndex index(row, col);
                PatternCellData data = _cell_matrices[direction](row, col);

                lambda(index, data, PatternDirection(direction), should_continue);

                if (!should_continue)
                    return;
            }
        }
    }
}

bool GomokuPatternReconizer::is_structure_capturable(const GomokuGame &board, PatternCellIndex index, const PatternCellData &data, PatternDirection direction) const
{
    if (data.structure_length() == 2 && data.is_structure_closed())
    {
        const bool index_is_valid = board.pattern_coordinate_is_valid(index);
        if (index_is_valid)
        {
            const PatternCellIndex opposite_side_index = get_index_offset(index, direction, -3);
            const bool opposite_side_is_valid = board.pattern_coordinate_is_valid(opposite_side_index);

            return opposite_side_is_valid;
        }
    }
    return false;
}
