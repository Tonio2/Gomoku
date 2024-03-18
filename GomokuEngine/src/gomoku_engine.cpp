#include "gomoku_engine.h"

#include "gomoku_pattern_reconizer.h"

std::map<std::string, Timer::FunctionAccumulation> Timer::accumulatedFunctions;
std::set<std::string> Timer::activeFunctions;

std::vector<Pattern> patterns = {
    {StructureType::OPEN_FOUR, "ECCCCE"},
    {StructureType::FOUR, "ECCCCB"},
    {StructureType::FOUR, "BCCCCE"},
    {StructureType::OPEN_THREE, "ECCCE"},
    {StructureType::OPEN_THREE, "ECECCE"},
    {StructureType::OPEN_THREE, "ECCECE"},
    {StructureType::THREE, "BCCCE"},
    {StructureType::THREE, "ECCCB"},
    {StructureType::THREE, "BCECCE"},
    {StructureType::THREE, "ECECCB"},
    {StructureType::THREE, "BCCECE"},
    {StructureType::THREE, "ECCECB"},
    {StructureType::OPEN_TWO, "ECCE"}, // Should i add open two with a gap in between?
    {StructureType::TWO, "BCCE"},
    {StructureType::TWO, "ECCB"},
    {StructureType::OPEN_ONE, "ECE"},
    {StructureType::ONE, "BCE"},
    {StructureType::ONE, "ECB"}};

std::vector<std::string> structure_names = {"OPEN_FOUR", "FOUR", "OPEN_THREE", "THREE", "OPEN_TWO", "TWO", "OPEN_ONE", "ONE"};
std::vector<std::string> player_names = {"E", "X", "O"};

std::ostream &operator<<(std::ostream &stream, Player player)
{
    stream << player_names[player];
    return stream;
}

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint _size) : board(_size, _size),
                                     current_player(X),
                                     players_scores({0, 0, 0}),
                                     winner(E),
                                     players_structures(3, std::vector<Structure>(0)),
                                     players_reconizers({
                                         GomokuPatternReconizer(E),
                                         GomokuPatternReconizer(X),
                                         GomokuPatternReconizer(O),
                                     })
{
    players_reconizers[X].find_patterns_in_board(*this);
    players_reconizers[O].find_patterns_in_board(*this);
}

GomokuGame::~GomokuGame() {}

Player GomokuGame::get_board_value(int row, int col) const
{
    return board(row, col);
}

CellChange GomokuGame::set_board_value(int row, int col, Player value)
{
    CellChange cell_change;

    cell_change.row = row;
    cell_change.col = col;

    cell_change.old_value = board(row, col);
    board(row, col) = value;
    cell_change.new_value = value;

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

void GomokuGame::display_board() const
{
    std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
    std::cout << " ";
    for (char coordinate : boardCoordinates)
    {
        std::cout << " " << coordinate;
    }
    std::cout << std::endl;
    for (int i = 0; i < get_board_size(); i++)
    {
        std::cout << boardCoordinates[i] << " ";
        for (int j = 0; j < get_board_size(); j++)
        {
            if (get_board_value(i, j) == X)
            {
                std::cout << "X ";
            }
            else if (get_board_value(i, j) == O)
            {
                std::cout << "O ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<int>> GomokuGame::get_patterns_count()
{
    std::vector<std::vector<int>> patterns_count(3, std::vector<int>(8, 0));
    patterns_count[1] = players_reconizers[X].get_pattern_count();
    patterns_count[2] = players_reconizers[O].get_pattern_count();
    return patterns_count;
}

void GomokuGame::print_patterns()
{
    players_reconizers[X].print_patterns();
    players_reconizers[O].print_patterns();
}

bool GomokuGame::is_game_over() const
{
    return winner != E;
}

bool GomokuGame::coordinates_are_valid(int row, int col) const
{
    return board.is_in_bound(row, col);
}

std::pair<int, bool> GomokuGame::count_stones_and_gap(uint row, uint col, int row_dir, int col_dir, Player player, bool &space) const
{
    int stones = 0;
    bool gap = false;
    char otherPlayer = other_player(player);
    for (uint i = 1; i < 5; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        Player cell = get_board_value(x, y);
        if (!coordinates_are_valid(x, y) or cell == otherPlayer)
        {
            if (space && !stones)
                space = false;
            break;
        }
        else if (cell == E)
        {
            bool space_val = space;
            space = true;
            gap = true;
            if (space_val)
            {
                if (!stones)
                    space = false;
                break;
            }
        }
        else
        {
            if (stones == 2)
                break;
            stones++;
            gap = false;
        }
    }
    return std::make_pair(stones, gap);
}

bool GomokuGame::check_direction_for_open_three(uint row, uint col, int row_dir, int col_dir, Player player) const
{
    bool space = false;
    auto [stones1, gap1] = count_stones_and_gap(row, col, row_dir, col_dir, player, space);
    auto [stones2, gap2] = count_stones_and_gap(row, col, -row_dir, -col_dir, player, space);
    if (stones1 + stones2 >= 2 and gap1 and gap2)
        return true;
    return false;
}

int GomokuGame::count_open_threes(uint row, uint col, Player player) const
{
    int open_threes = 0;
    open_threes += check_direction_for_open_three(row, col, 1, 0, player);
    open_threes += check_direction_for_open_three(row, col, 0, 1, player);
    open_threes += check_direction_for_open_three(row, col, 1, 1, player);
    open_threes += check_direction_for_open_three(row, col, 1, -1, player);
    return open_threes;
}

MoveResult GomokuGame::make_move(int row, int col)
{
    MoveResult move_result;
    const int old_black_score = get_player_score(X);
    const int old_white_score = get_player_score(O);

    if (!coordinates_are_valid(row, col))
    {
        throw std::invalid_argument("Invalid coordinates");
    }
    if (get_board_value(row, col) != E)
    {
        throw std::invalid_argument("Cell is already occupied");
    }

    bool captured = capture(row, col, current_player, move_result);

    if (!captured)
    {
        int open_threes = count_open_threes(row, col, current_player);
        if (open_threes > 1)
        {
            throw std::invalid_argument("Invalid move: more than one open three");
        }
    }

    const CellChange cell_change = set_board_value(row, col, current_player);
    move_result.cell_changes.push_back(cell_change);

    if (check_win(row, col, current_player))
        winner = current_player;

    current_player = other_player(current_player);

    move_result.black_score_change = get_player_score(X) - old_black_score;
    move_result.white_score_change = get_player_score(O) - old_white_score;

    players_reconizers[X].update_patterns_with_move(*this, move_result);
    players_reconizers[O].update_patterns_with_move(*this, move_result);

    // std::cout << std::endl
    //           << std::endl;
    // players_reconizers[X].print_patterns();
    // players_reconizers[O].print_patterns();

    return move_result;
}

void GomokuGame::reverse_move(const MoveResult &move)
{
    modify_player_score(X, -move.black_score_change);
    modify_player_score(O, -move.white_score_change);

    current_player = other_player(current_player);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.old_value);
    }

    players_reconizers[X].update_patterns_with_move(*this, move);
    players_reconizers[O].update_patterns_with_move(*this, move);

    winner = E;
}

void GomokuGame::reapply_move(const MoveResult &move)
{
    modify_player_score(X, move.black_score_change);
    modify_player_score(O, move.white_score_change);

    for (const CellChange &cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.new_value);
    }

    CellChange cell = move.cell_changes.back();

    if (check_win(cell.row, cell.col, current_player))
        winner = current_player;

    current_player = other_player(current_player);
}

std::vector<std::pair<std::pair<int, int>, int>> GomokuGame::findRelevantMoves() const
{
    std::vector<std::pair<std::pair<int, int>, int>> relevantMoves;

    // Directions to check around each cell (8 directions).
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int row = 0; row < board.get_height(); ++row)
    {
        for (int col = 0; col < board.get_width(); ++col)
        {
            if (get_board_value(row, col) == E)
            { // Empty cell
                bool foundStone = false;
                for (const auto &dir : directions)
                {
                    for (int step = 1; step <= 2; ++step)
                    {
                        int newRow = row + step * dir.first;
                        int newCol = col + step * dir.second;

                        if (coordinates_are_valid(newRow, newCol) && get_board_value(newRow, newCol) != E)
                        {
                            relevantMoves.push_back({{row, col}, 0});
                            foundStone = true;
                            break; // No need to check further if one stone is found near this cell.
                        }
                    }
                    if (foundStone)
                    {
                        break; // No need to check further if one stone is found near this cell.
                    }
                }
            }
        }
    }

    return relevantMoves;
}

std::vector<std::vector<Structure>> GomokuGame::get_structures() const
{
    return players_structures;
}

void GomokuGame::set_structures(std::vector<std::vector<Structure>> _structures)
{
    players_structures = _structures;
}

bool GomokuGame::try_direction_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result)
{
    Player otherPlayer = other_player(player);
    std::vector<Player> expected_value = {otherPlayer, otherPlayer, player};

    for (uint i = 1; i < 4; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        if (!coordinates_are_valid(x, y) or get_board_value(x, y) != expected_value[i - 1])
            return false;
    }
    move_result.cell_changes.push_back(
        set_board_value(row + row_dir, col + col_dir, E));

    move_result.cell_changes.push_back(
        set_board_value(row + 2 * row_dir, col + 2 * col_dir, E));

    modify_player_score(player, 2);
    return true;
}

bool GomokuGame::try_cardinal_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult &move_result)
{
    bool ret = try_direction_for_capture(row, col, row_dir, col_dir, player, move_result);
    ret |= try_direction_for_capture(row, col, -row_dir, -col_dir, player, move_result);

    return ret;
}

bool GomokuGame::capture(uint row, uint col, Player player, MoveResult &move_result)
{
    bool ret = false;
    ret |= try_cardinal_for_capture(row, col, 1, 0, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 0, 1, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 1, 1, player, move_result);
    ret |= try_cardinal_for_capture(row, col, 1, -1, player, move_result);
    return ret;
}

int GomokuGame::get_board_size() const
{
    return board.get_width();
}

int GomokuGame::count_stones(uint row, uint col, int row_dir, int col_dir, Player player) const
{
    int stones = 0;
    for (int i = 1; i < 5; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        if (!coordinates_are_valid(x, y) or get_board_value(x, y) != player)
        {
            break;
        }
        stones++;
    }
    return stones;
}

bool GomokuGame::check_dir_for_5_in_a_row(uint row, uint col, int row_dir, int col_dir, Player player) const
{
    int stones = count_stones(row, col, row_dir, col_dir, player);
    stones += count_stones(row, col, -row_dir, -col_dir, player);
    return stones >= 4;
}

bool GomokuGame::check_5_in_a_row(uint row, uint col, Player player) const
{
    if (check_dir_for_5_in_a_row(row, col, 1, 0, player))
        return true;
    if (check_dir_for_5_in_a_row(row, col, 0, 1, player))
        return true;
    if (check_dir_for_5_in_a_row(row, col, 1, 1, player))
        return true;
    if (check_dir_for_5_in_a_row(row, col, 1, -1, player))
        return true;
    return false;
}

bool GomokuGame::check_win(uint row, uint col, Player player)
{
    if (get_player_score(player) >= 10)
    {
        return true;
    }
    else
    {
        return check_5_in_a_row(row, col, player);
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

void GomokuGame::display_struct() const
{
    for (uint i = 1; i < players_structures.size(); i++)
    {
        for (uint j = 0; j < players_structures[i].size(); j++)
        {
            std::cout << player_names[i] << ": Structure " << j << " " << structure_names[players_structures[i][j].type] << " ";
            for (uint k = 0; k < players_structures[i][j].cells.size(); k++)
            {
                std::cout << "(" << players_structures[i][j].cells[k].first << ", " << players_structures[i][j].cells[k].second << ") ";
            }
            std::cout << std::endl;
        }
    }
}
