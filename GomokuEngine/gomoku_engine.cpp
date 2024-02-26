#include "gomoku_engine.h"

std::vector<char> GomokuGame::coordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                             'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

// Definitions of GomokuGame methods
GomokuGame::GomokuGame(uint _size) : board_size(_size), board(_size * _size), current_player(X), game_over(false)
{
    player_scores[X] = 0;
    player_scores[O] = 0;
}

unsigned char GomokuGame::get_board_value(int x, int y) const
{
    return board[x * board_size + y];
}

void GomokuGame::set_board_value(int x, int y, Player value)
{
    board[x * board_size + y] = value;
}

Player GomokuGame::other_player(Player player) const
{
    return (player == X) ? O : X;
}

int GomokuGame::compute_coordinate(char c) const
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'I')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
};

bool GomokuGame::is_game_over() const
{
    return game_over;
}

bool GomokuGame::coordinates_are_valid(int row, int col) const
{
    return row >= 0 && row < 19 && col >= 0 && col < 19;
}

std::pair<int, bool> GomokuGame::count_stones_and_gap(size_t row, size_t col, int row_dir, int col_dir, Player player, bool &space) const
{
    int stones = 0;
    bool gap = false;
    char otherPlayer = other_player(player);
    for (size_t i = 1; i < 5; i++)
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

bool GomokuGame::check_direction_for_open_three(size_t row, size_t col, int row_dir, int col_dir, Player player) const
{
    bool space = false;
    auto [stones1, gap1] = count_stones_and_gap(row, col, row_dir, col_dir, player, space);
    auto [stones2, gap2] = count_stones_and_gap(row, col, -row_dir, -col_dir, player, space);
    if (row_dir == 0 and col_dir == 1)
        if (stones1 + stones2 >= 2 and gap1 and gap2)
            return true;
    return false;
}

int GomokuGame::count_open_threes(size_t row, size_t col, Player player) const
{
    int open_threes = 0;
    open_threes += check_direction_for_open_three(row, col, 1, 0, player);
    open_threes += check_direction_for_open_three(row, col, 0, 1, player);
    open_threes += check_direction_for_open_three(row, col, 1, 1, player);
    open_threes += check_direction_for_open_three(row, col, 1, -1, player);
    return open_threes;
}

void GomokuGame::make_move(char row, char col)
{
    // TODO: Coordinates will be int later
    int row_num = compute_coordinate(row);
    int col_num = compute_coordinate(col);
    if (!coordinates_are_valid(row_num, col_num))
    {
        throw std::invalid_argument("Invalid coordinates");
    }
    if (get_board_value(row_num, col_num) != E)
    {
        throw std::invalid_argument("Cell is already occupied");
    }
    bool captured = capture(row_num, col_num, current_player);
    if (!captured)
    {
        int open_threes = count_open_threes(row_num, col_num, current_player);
        if (open_threes > 1)
        {
            throw std::invalid_argument("Invalid move: more than one open three");
        }
    }
    board[row_num * board_size + col_num] = current_player;
    if (check_win(row_num, col_num, current_player))
    {
        game_over = true;
        winner = current_player;
    }
    else
    {
        current_player = (current_player == X) ? O : X;
    }
}

bool GomokuGame::try_direction_for_capture(size_t row, size_t col, int row_dir, int col_dir, Player player)
{
    Player otherPlayer = other_player(player);
    std::vector<Player> expected_value = {otherPlayer, otherPlayer, player};

    for (size_t i = 1; i < 4; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        if (!coordinates_are_valid(x, y) or get_board_value(x, y) != expected_value[i - 1])
            return false;
    }
    set_board_value(row + row_dir, col + col_dir, E);
    set_board_value(row + 2 * row_dir, col + 2 * col_dir, E);
    player_scores[player] += 2;
    return true;
}

bool GomokuGame::try_cardinal_for_capture(size_t row, size_t col, int row_dir, int col_dir, Player player)
{
    bool ret = try_direction_for_capture(row, col, row_dir, col_dir, player);
    ret |= try_direction_for_capture(row, col, -row_dir, -col_dir, player);

    return ret;
}

bool GomokuGame::capture(size_t row, size_t col, Player player)
{
    bool ret = false;
    ret |= try_cardinal_for_capture(row, col, 1, 0, player);
    ret |= try_cardinal_for_capture(row, col, 0, 1, player);
    ret |= try_cardinal_for_capture(row, col, 1, 1, player);
    ret |= try_cardinal_for_capture(row, col, 1, -1, player);
    return ret;
}

int GomokuGame::get_board_size() const
{
    return board_size;
}

int GomokuGame::count_stones(size_t row, size_t col, int row_dir, int col_dir, Player player) const
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

bool GomokuGame::check_dir_for_5_in_a_row(size_t row, size_t col, int row_dir, int col_dir, Player player) const
{
    int stones = count_stones(row, col, row_dir, col_dir, player);
    stones += count_stones(row, col, -row_dir, -col_dir, player);
    return stones >= 4;
}

bool GomokuGame::check_5_in_a_row(size_t row, size_t col, Player player) const
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

bool GomokuGame::check_win(size_t row, size_t col, Player player)
{
    if (player_scores[player] >= 10)
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
    if (!game_over)
    {
        throw std::invalid_argument("Game is not over yet");
    }
    return winner;
}