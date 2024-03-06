#include "gomoku_engine.h"

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint _size) : board_size(_size), board(_size * _size), current_player(X), winner(E)
{
    black_score = 0;
    white_score = 0;
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
    switch (player)
    {
        case X:
            black_score += score;
            break;
        case O:
            white_score += score;
            break;
        default:
            break;
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
        if (!coordinates_are_valid(x, y) or get_board_value(x, y) == otherPlayer)
        {
            if (space && !stones)
                space = false;
            break;
        }
        else if (get_board_value(x, y) == E)
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
    move_result.cell_changes.push_back(cell_change);

    if (check_win(row, col, current_player))
        winner = current_player;
    else
        current_player = other_player(current_player);

    move_result.black_score_change = get_player_score(X) - old_black_score;
    move_result.white_score_change = get_player_score(O) - old_white_score;

    return move_result;
}

void GomokuGame::reverse_move(const MoveResult& move)
{
    modify_player_score(X, -move.black_score_change);
    modify_player_score(O, -move.white_score_change);

    current_player = other_player(current_player);

    for (const CellChange& cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.old_value);
    }
}

void GomokuGame::reapply_move(const MoveResult& move)
{
    modify_player_score(X, move.black_score_change);
    modify_player_score(O, move.white_score_change);

    current_player = other_player(current_player);

    for (const CellChange& cell_change : move.cell_changes)
    {
        set_board_value(cell_change.row, cell_change.col, cell_change.new_value);
    }
}

bool GomokuGame::try_direction_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult& move_result)
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
        set_board_value(row + row_dir, col + col_dir, E)
    );

    move_result.cell_changes.push_back(
        set_board_value(row + 2 * row_dir, col + 2 * col_dir, E)
    );

    modify_player_score(player, 2);
    return true;
}

bool GomokuGame::try_cardinal_for_capture(uint row, uint col, int row_dir, int col_dir, Player player, MoveResult& move_result)
{
    bool ret = try_direction_for_capture(row, col, row_dir, col_dir, player, move_result);
    ret |= try_direction_for_capture(row, col, -row_dir, -col_dir, player, move_result);

    return ret;
}

bool GomokuGame::capture(uint row, uint col, Player player, MoveResult& move_result)
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
    switch (player)
    {
        case Player::BLACK:
            return black_score;
        case Player::WHITE:
            return white_score;
        default:
            break;
    }
    return 0;
}
