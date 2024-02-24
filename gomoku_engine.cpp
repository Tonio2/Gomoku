#include "gomoku_engine.h"

std::vector<char> GomokuGame::coordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                             'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

// Definitions of GomokuGame methods
GomokuGame::GomokuGame() : board(19, std::vector<char>(19, ' ')), current_player('X'), game_over(false)
{
}

size_t GomokuGame::compute_coordinate(char c) const
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

std::pair<int, bool> GomokuGame::count_stones_and_gap(size_t row, size_t col, int row_dir, int col_dir, char player, bool &space) const
{
    int stones = 0;
    bool gap = false;
    char other_player = (player == 'X') ? 'O' : 'X';
    for (size_t i = 1; i < 5; i++)
    {
        int x = row + i * row_dir;
        int y = col + i * col_dir;
        if (!coordinates_are_valid(x, y) or board[x][y] == other_player)
        {
            if (space && !stones)
                space = false;
            break;
        }
        else if (board[x][y] == ' ')
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

bool GomokuGame::check_direction_for_open_three(size_t row, size_t col, int row_dir, int col_dir, char player) const
{
    bool space = false;
    auto [stones1, gap1] = count_stones_and_gap(row, col, row_dir, col_dir, player, space);
    auto [stones2, gap2] = count_stones_and_gap(row, col, -row_dir, -col_dir, player, space);
    if (row_dir == 0 and col_dir == 1)
        std::cout << "stones1: " << stones1 << " gap1: " << gap1 << " stones2: " << stones2 << " gap2: " << gap2 << std::endl;
    if (stones1 + stones2 >= 2 and gap1 and gap2)
        return true;
    return false;
}

int GomokuGame::count_open_threes(size_t row, size_t col, char player) const
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
    size_t row_num = compute_coordinate(row);
    size_t col_num = compute_coordinate(col);
    if (!coordinates_are_valid(row_num, col_num))
    {
        throw std::invalid_argument("Invalid coordinates");
    }
    board[row_num][col_num] = current_player;
    std::cout << count_open_threes(row_num, col_num, current_player) << " open threes for this move" << std::endl;
    current_player = (current_player == 'X') ? 'O' : 'X';
}

bool GomokuGame::check_win() const
{
    return false;
}

std::string GomokuGame::get_board() const
{

    int row_num = 0;
    std::string board_str;
    board_str += "  0 1 2 3 4 5 6 7 8 9 A B C D E F G H I\n";
    // std::cout << board_str;
    for (const auto &row : board)
    {
        board_str += " +-------------------------------------+\n";

        board_str += GomokuGame::coordinates[row_num++];
        // std::cout << board_str;
        for (const auto &cell : row)
        {
            // std::cout << cell;
            board_str += "|";
            board_str += cell;
        }
        board_str += "|\n";
    }
    board_str += " +-------------------------------------+\n";
    return board_str;
}
