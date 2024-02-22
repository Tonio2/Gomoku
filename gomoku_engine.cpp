#include "gomoku_engine.h"

// Definitions of GomokuGame methods
GomokuGame::GomokuGame() : board(19, std::vector<char>(19, ' ')), current_player('X'), game_over(false) {}

bool GomokuGame::is_game_over() const
{
  return game_over;
}

void GomokuGame::make_move(int row, int col)
{
  board[row][col] = current_player;
  current_player = (current_player == 'X') ? 'O' : 'X';
}

bool GomokuGame::check_win() const
{
  return false;
}

std::string GomokuGame::get_board() const
{
  std::vector<char> coordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
  int row_num = 0;
  std::string board_str;
  board_str += "  0 1 2 3 4 5 6 7 8 9 A B C D E F G H I\n";
  // std::cout << board_str;
  for (const auto &row : board)
  {
    board_str += " +-------------------------------------+\n";

    board_str += coordinates[row_num++];
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
