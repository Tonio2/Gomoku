
#include "engine/gomoku_engine.h"
#include "engine/gomoku_pattern_reconizer.h"

std::vector<std::string> structure_names = {
    "NONE",
    "FIVE_OR_MORE",
    "OPEN_ONE",
    "ONE",
    "OPEN_TWO",
    "TWO",
    "OPEN_THREE",
    "THREE",
    "OPEN_FOUR",
    "FOUR",
    "COUNT_STRUCTURE_TYPE"};

std::vector<std::string> player_names = {".", "X", "O"};

std::ostream &operator<<(std::ostream &stream, Player player)
{
    stream << player_names[player];
    return stream;
}

std::ostream &operator<<(std::ostream &stream, StructureType structure_type)
{
    stream << structure_names[structure_type];
    return stream;
}

static int score_from_structure(StructureType type)
{
    switch (type)
    {
    case NONE:
        return 0;
    case ONE:
        return 1;
    case OPEN_ONE:
        return 2;
    case TWO:
        return 3;
    case OPEN_TWO:
        return 4;
    case THREE:
        return 5;
    case OPEN_THREE:
        return 6;
    case FOUR:
        return 7;
    case OPEN_FOUR:
        return 8;
    case FIVE_OR_MORE:
        return 9;
    default:
        return 0;
    }
}

bool compare_structure_type(StructureType lhs, StructureType rhs)
{
    return score_from_structure(lhs) < score_from_structure(rhs);
}

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint width, uint height, bool capture_enabled)
    : board(width, height),
      _min_played(width, height),
      _max_played(0, 0),
      _relevancy_matrix(width + 4, height + 4),
      _relevant_cells({}),
      empty_cells(width * height),
      current_player(X),
      players_scores({0, 0, 0}),
      is_game_over_flag(false),
      winner(E),
      players_reconizers({
          GomokuPatternReconizer(E),
          GomokuPatternReconizer(X),
          GomokuPatternReconizer(O),
      }),
      _capture_enabled(capture_enabled)
{
    players_reconizers[X].find_patterns_in_board(*this);
    players_reconizers[O].find_patterns_in_board(*this);
}

GomokuGame::GomokuGame(const GomokuGame &copy)
    : board(copy.board),
      _min_played(copy._min_played),
      _max_played(copy._max_played),
      _relevancy_matrix(copy._relevancy_matrix),
      _relevant_cells(copy._relevant_cells),
      empty_cells(copy.empty_cells),
      current_player(copy.current_player),
      players_scores(copy.players_scores),
      is_game_over_flag(copy.is_game_over_flag),
      winner(copy.winner),
      players_reconizers(copy.players_reconizers),
      _capture_enabled(copy._capture_enabled)
{
}

GomokuGame &GomokuGame::operator=(const GomokuGame &copy)
{
    if (this != &copy)
    {
        board = copy.board;
        _min_played = copy._min_played;
        _max_played = copy._max_played;
        _relevancy_matrix = copy._relevancy_matrix;
        _relevant_cells = copy._relevant_cells;
        empty_cells = copy.empty_cells;
        current_player = copy.current_player;
        players_scores = copy.players_scores;
        is_game_over_flag = copy.is_game_over_flag;
        winner = copy.winner;
        players_reconizers = copy.players_reconizers;
        _capture_enabled = copy._capture_enabled;
    }
    return *this;
}

GomokuGame::~GomokuGame() {}

CellChange GomokuGame::set_board_value(int row, int col, Player value, bool updateRelevancyMatrix)
{
    assert(coordinates_are_valid(row, col));
    assert(board(row, col) != value);

    CellChange cell_change;

    cell_change.row = row;
    cell_change.col = col;

    cell_change.old_value = board(row, col);
    board(row, col) = value;
    cell_change.new_value = value;

    // Assume that if the value is E, then the cell was occupied before because there is no situation in which you would empty an empty cell
    // and if the value is not E, then the cell was not occupied because you cannot turn a stone into another stone in Gomoku.
    empty_cells += (value == E) ? 1 : -1;
    if (updateRelevancyMatrix) 
        update_relevancy(row, col, value == E);

    return cell_change;
}

Player GomokuGame::other_player(Player player) const
{
    return (player == X) ? O : X;
}

void GomokuGame::modify_player_score(Player player, int score)
{
    players_scores[player] += score;
}

std::pair<GomokuCellIndex, GomokuCellIndex> GomokuGame::get_played_bounds(int margin) const
{
    if (margin == 0)
        return {_min_played, _max_played};

    GomokuCellIndex min_played = GomokuCellIndex(
        std::max(0, _min_played.row - margin),
        std::max(0, _min_played.col - margin));

    GomokuCellIndex max_played = GomokuCellIndex(
        std::min(get_board_height() - 1, _max_played.row + margin),
        std::min(get_board_width() - 1, _max_played.col + margin));

    return {min_played, max_played};
}

bool GomokuGame::has_player_bounds() const
{
    return empty_cells < get_board_width() * get_board_height();
}

const CellSet &GomokuGame::get_relevant_cells() const
{
    return _relevant_cells;
}


const GomokuPatternReconizer &GomokuGame::get_pattern_reconizer(Player player) const
{
    return players_reconizers[player];
}

const std::vector<int> &GomokuGame::get_patterns_count(Player player)
{
    TIMER
    return players_reconizers[player].get_pattern_count();
}

void GomokuGame::print_patterns()
{
    players_reconizers[X].print_patterns();
    players_reconizers[O].print_patterns();
}

std::vector<std::vector<int>> GomokuGame::get_board() const
{
    std::vector<std::vector<int>> board_values(get_board_height(), std::vector<int>(get_board_width()));

    for (int row = 0; row < get_board_height(); ++row)
    {
        for (int col = 0; col < get_board_width(); ++col)
        {
            board_values[row][col] = get_board_value(row, col);
        }
    }

    return board_values;
}

bool GomokuGame::is_game_over() const
{
    return is_game_over_flag;
}

bool GomokuGame::coordinates_are_valid(int row, int col) const
{
    return board.is_in_bound(row, col);
}

bool GomokuGame::pattern_coordinate_is_valid(const PatternCellIndex &index) const
{
    return coordinates_are_valid(index.row - 1, index.col - 1);
}

MoveResult GomokuGame::make_move(int row, int col, bool updateRelevancyMatrix)
{
    TIMER
    MoveResult move_result;
    move_result.previous_min_move = _min_played;
    move_result.previous_max_move = _max_played;

    const int old_black_score = get_player_score(X);
    const int old_white_score = get_player_score(O);

    const int old_open_three_count = players_reconizers[current_player].get_pattern_count()[StructureType::OPEN_THREE];

    if (!coordinates_are_valid(row, col))
    {
        throw std::invalid_argument("Invalid coordinates " + std::to_string(row) + ", " + std::to_string(col));
    }
    if (get_board_value(row, col) != E)
    {
        throw std::invalid_argument("Cell is already occupied");
    }

    const CellChange cell_change = set_board_value(row, col, current_player, updateRelevancyMatrix);
    move_result.cell_changes.push_back(cell_change);

    bool captured = false;

    if (_capture_enabled)
        captured = capture(row, col, current_player, move_result, updateRelevancyMatrix);

    move_result.black_score_change = get_player_score(X) - old_black_score;
    move_result.white_score_change = get_player_score(O) - old_white_score;

    players_reconizers[X].update_patterns_with_move(*this, move_result);
    players_reconizers[O].update_patterns_with_move(*this, move_result);

    if (!captured)
    {
        const int new_open_three_count = players_reconizers[current_player].get_pattern_count()[StructureType::OPEN_THREE];
        if (new_open_three_count - old_open_three_count > 1)
        {
            Player p = current_player;
            reverse_move(move_result, updateRelevancyMatrix);
            current_player = p;
            throw std::invalid_argument("Invalid move: more than one open three");
        }
    }

    check_win(current_player);

    current_player = other_player(current_player);

    if (row < _min_played.row)
        _min_played.row = row;
    if (row > _max_played.row)
        _max_played.row = row;
    if (col < _min_played.col)
        _min_played.col = col;
    if (col > _max_played.col)
        _max_played.col = col;

    return move_result;
}

void GomokuGame::reverse_move(const MoveResult &move, bool updateRelevancyMatrix)
{
    TIMER
    modify_player_score(X, -move.black_score_change);
    modify_player_score(O, -move.white_score_change);

    current_player = other_player(current_player);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.old_value, updateRelevancyMatrix);
    }

    players_reconizers[X].update_patterns_with_move(*this, move);
    players_reconizers[O].update_patterns_with_move(*this, move);

    winner = E;
    is_game_over_flag = false;

    _min_played = move.previous_min_move;
    _max_played = move.previous_max_move;
}

void GomokuGame::reapply_move(const MoveResult &move)
{
    modify_player_score(X, move.black_score_change);
    modify_player_score(O, move.white_score_change);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.new_value);
    }

    players_reconizers[X].update_patterns_with_move(*this, move);
    players_reconizers[O].update_patterns_with_move(*this, move);

    check_win(current_player);

    current_player = other_player(current_player);

    const int8_t &row = move.cell_changes[0].row;
    const int8_t &col = move.cell_changes[0].col;

    if (row < _min_played.row)
        _min_played.row = row;
    if (row > _max_played.row)
        _max_played.row = row;
    if (col < _min_played.col)
        _min_played.col = col;
    if (col > _max_played.col)
        _max_played.col = col;
}

static const std::vector<std::pair<int, int>> _directions_offsets = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

void GomokuGame::update_relevancy(int8_t row, int8_t col, bool is_new_empty_cell)
{
    int8_t modify = is_new_empty_cell ? -1 : 1;
    int baseRow = row + 2;
    int baseCol = col + 2;
    for (const auto &dir : _directions_offsets)
    {
        int dirFirst = dir.first;
        int dirSecond = dir.second;
        for (int step = 1; step <= 2; ++step)
        {
            int newRow = baseRow + step * dirFirst;
            int newCol = baseCol + step * dirSecond;

            _relevancy_matrix(newRow, newCol) += modify;
        }
    }
}

bool GomokuGame::try_direction_for_capture(int row, int col, int row_dir, int col_dir, Player player, MoveResult &move_result, bool updateRelevancyMatrix)
{
    const Player otherPlayer = other_player(player);

    const int x1 = row + row_dir;
    const int y1 = col + col_dir;
    const int x2 = row + 2 * row_dir;
    const int y2 = col + 2 * col_dir;
    const int x3 = row + 3 * row_dir;
    const int y3 = col + 3 * col_dir;

    if (get_board_value(x1, y1) != otherPlayer || get_board_value(x2, y2) != otherPlayer || get_board_value(x3, y3) != player)
        return false;

    move_result.cell_changes.push_back(
        set_board_value(row + row_dir, col + col_dir, E, updateRelevancyMatrix));

    move_result.cell_changes.push_back(
        set_board_value(row + 2 * row_dir, col + 2 * col_dir, E, updateRelevancyMatrix));

    modify_player_score(player, 2);
    return true;
}

bool GomokuGame::capture(int row, int col, Player player, MoveResult &move_result, bool updateRelevancyMatrix)
{
    bool ret = false;
    if (row < board.get_height() - 3)
    {
        ret |= try_direction_for_capture(row, col, 1, 0, player, move_result, updateRelevancyMatrix);
        if (col < board.get_width() - 3)
            ret |= try_direction_for_capture(row, col, 1, 1, player, move_result, updateRelevancyMatrix);
    }
    if (row > 2)
    {
        ret |= try_direction_for_capture(row, col, -1, 0, player, move_result, updateRelevancyMatrix);
        if (col > 2)
            ret |= try_direction_for_capture(row, col, -1, -1, player, move_result, updateRelevancyMatrix);
    }
    if (col < board.get_width() - 3)
    {
        ret |= try_direction_for_capture(row, col, 0, 1, player, move_result, updateRelevancyMatrix);
        if (row > 2)
            ret |= try_direction_for_capture(row, col, -1, 1, player, move_result, updateRelevancyMatrix);
    }
    if (col > 2)
    {
        ret |= try_direction_for_capture(row, col, 0, -1, player, move_result, updateRelevancyMatrix);
        if (row < board.get_height() - 3)
            ret |= try_direction_for_capture(row, col, 1, -1, player, move_result, updateRelevancyMatrix);
    }
    return ret;
}

int GomokuGame::get_board_width() const
{
    return board.get_width();
}

int GomokuGame::get_board_height() const
{
    return board.get_height();
}

void GomokuGame::check_win(Player player)
{
    if (get_player_score(player) >= 10)
    {
        is_game_over_flag = true;
        winner = current_player;
        return;
    }

    if (players_reconizers[player].get_pattern_count()
            [StructureType::FIVE_OR_MORE] > 0)
    {
        if (not _capture_enabled || players_reconizers[player].five_or_more_cant_be_captured(*this))
        {
            is_game_over_flag = true;
            winner = current_player;
            return;
        }
    }

    if (get_player_score(other_player(player)) == 8)
    {
        if (players_reconizers[player].can_be_captured(*this))
        {
            is_game_over_flag = true;
            winner = other_player(player);
            return;
        }
    }

    if (empty_cells == 0)
    {
        is_game_over_flag = true;
    }
}

Player GomokuGame::get_winner() const
{
    return winner;
}

Player GomokuGame::get_current_player() const
{
    return current_player;
}

int GomokuGame::get_player_score(Player player) const
{
    return players_scores[player];
}

bool cell_set_contains(const CellSet &cell_set, int row, int col)
{
    auto row_it = cell_set.find(row);
    if (row_it == cell_set.end())
        return false;
    return row_it->second.find(col) != row_it->second.end();
}
