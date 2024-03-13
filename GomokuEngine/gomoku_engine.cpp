#include "gomoku_engine.h"

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

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint _size) : board_size(_size),
                                     board(_size * _size),
                                     current_player(X),
                                     players_scores({0, 0, 0}),
                                     winner(E),
                                     structures(3, std::vector<Structure>(0))
{
}

static inline int board_index(int row, int col, uint board_size)
{
    return row * board_size + col;
}

unsigned char GomokuGame::get_board_value(int row, int col) const
{
    return board[board_index(row, col, board_size)];
}

CellChange GomokuGame::set_board_value(int row, int col, Player value)
{
    Timer timer(__FUNCTION__);
    CellChange cell_change;

    cell_change.row = row;
    cell_change.col = col;

    const int index = board_index(row, col, board_size);

    cell_change.old_value = board[index];
    board[index] = value;
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

std::vector<std::pair<int, int>> GomokuGame::check_pattern(uint row, uint col, std::string pattern, StructureType type, Player player, std::pair<int, int> dir) const
{
    std::vector<std::pair<int, int>> cells;
    Player otherPlayer = other_player(player);
    for (int i = 0; i < pattern.size(); i++)
    {
        if (i == 1)
        {
            cells.push_back({row, col});
            continue;
        }
        int y = row + (i - 1) * dir.first;
        int x = col + (i - 1) * dir.second;
        char val;
        unsigned char cell = get_board_value(y, x);
        if (!coordinates_are_valid(y, x) or cell == otherPlayer)
        {
            val = 'B';
        }
        else if (cell == player)
        {
            val = 'C';
        }
        else
        {
            val = 'E';
        }

        if (val != pattern[i])
            throw std::invalid_argument("Invalid pattern");

        cells.push_back({y, x});
    }
    return cells;
}

void GomokuGame::update_structures(Player player)
{
    structures[player].clear();

    // Check for structures in rows
    for (uint row = 0; row < board_size; row++)
    {
        for (uint col = 0; col < board_size; col++)
        {
            if (get_board_value(row, col) == player)
            {
                for (uint i = 0; i < patterns.size(); i++)
                {
                    std::string pattern = patterns[i].pattern;
                    StructureType type = patterns[i].type;
                    try
                    {
                        std::vector<std::pair<int, int>> cells = check_pattern(row, col, pattern, type, player, {0, 1});
                        structures[player].push_back({type, cells});
                    }
                    catch (const std::invalid_argument &e)
                    {
                    }
                }
            }
        }
    }

    // Check for structures in columns
    for (uint col = 0; col < board_size; col++)
    {
        for (uint row = 0; row < board_size; row++)
        {
            if (get_board_value(row, col) == player)
            {
                for (uint i = 0; i < patterns.size(); i++)
                {
                    std::string pattern = patterns[i].pattern;
                    StructureType type = patterns[i].type;
                    try
                    {
                        std::vector<std::pair<int, int>> cells = check_pattern(row, col, pattern, type, player, {1, 0});
                        structures[player].push_back({type, cells});
                    }
                    catch (const std::invalid_argument &e)
                    {
                    }
                }
            }
        }
    }

    // Check for structures in diagonals (1, -1)
    int row = 0;
    int col = 0;
    while (row != 19 or col != 17)
    {
        if (not coordinates_are_valid(row, col))
        {
            if (row <= 18)
            {
                col = row;
                row = 0;
            }
            else
            {
                row = col + 2;
                col = 18;
            }
        }
        if (get_board_value(row, col) == player)
        {
            for (uint i = 0; i < patterns.size(); i++)
            {
                std::string pattern = patterns[i].pattern;
                StructureType type = patterns[i].type;
                try
                {
                    std::vector<std::pair<int, int>> cells = check_pattern(row, col, pattern, type, player, {1, -1});
                    structures[player].push_back({type, cells});
                }
                catch (const std::invalid_argument &e)
                {
                }
            }
        }
        row++;
        col--;
    }

    // Check for structures in diagonals (1, 1)
    row = 0;
    col = 18;
    while (row != 19 or col != 1)
    {
        if (not coordinates_are_valid(row, col))
        {
            if (row <= 18)
            {
                col = 18 - row;
                row = 0;
            }
            else
            {
                row = 20 - col;
                col = 0;
            }
        }
        if (get_board_value(row, col) == player)
        {
            for (uint i = 0; i < patterns.size(); i++)
            {
                std::string pattern = patterns[i].pattern;
                StructureType type = patterns[i].type;
                try
                {
                    std::vector<std::pair<int, int>> cells = check_pattern(row, col, pattern, type, player, {1, 1});
                    structures[player].push_back({type, cells});
                }
                catch (const std::invalid_argument &e)
                {
                }
            }
        }
        row++;
        col++;
    }
}

bool GomokuGame::is_game_over() const
{
    return winner != E;
}

bool GomokuGame::coordinates_are_valid(int row, int col) const
{
    return row >= 0 && row < board_size && col >= 0 && col < board_size;
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
        unsigned char cell = get_board_value(x, y);
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
    if (row_dir == 0 and col_dir == 1)
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
    update_structures(current_player);
    update_structures(other_player(current_player));
    move_result.cell_changes.push_back(cell_change);

    if (check_win(row, col, current_player))
        winner = current_player;

    current_player = other_player(current_player);

    move_result.black_score_change = get_player_score(X) - old_black_score;
    move_result.white_score_change = get_player_score(O) - old_white_score;

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

    update_structures(current_player);
    update_structures(other_player(current_player));

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

    for (int row = 0; row < board_size; ++row)
    {
        for (int col = 0; col < board_size; ++col)
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
    return structures;
}

void GomokuGame::set_structures(std::vector<std::vector<Structure>> _structures)
{
    structures = _structures;
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
    return board_size;
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

unsigned char GomokuGame::get_winner() const
{
    return winner;
}

unsigned char GomokuGame::get_current_player() const
{
    return current_player;
}

int GomokuGame::get_player_score(Player player) const
{
    return players_scores[player];
}

void GomokuGame::display_struct() const
{
    for (uint i = 1; i < structures.size(); i++)
    {
        for (uint j = 0; j < structures[i].size(); j++)
        {
            std::cout << player_names[i] << ": Structure " << j << " " << structure_names[structures[i][j].type] << " ";
            for (uint k = 0; k < structures[i][j].cells.size(); k++)
            {
                std::cout << "(" << structures[i][j].cells[k].first << ", " << structures[i][j].cells[k].second << ") ";
            }
            std::cout << std::endl;
        }
    }
}
