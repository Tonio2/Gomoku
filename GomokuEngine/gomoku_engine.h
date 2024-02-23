#ifndef GOMOKU_ENGINE_H
#define GOMOKU_ENGINE_H

#include <string>
#include <vector>
#include <iostream>

class GomokuGame
{
private:
  std::vector<std::vector<char>> board;
  char current_player;
  bool game_over;

public:
  GomokuGame();
  bool is_game_over() const;
  void make_move(int row, int col);
  bool check_win() const;
  std::string get_board() const;
};

#endif // GOMOKU_ENGINE_H
