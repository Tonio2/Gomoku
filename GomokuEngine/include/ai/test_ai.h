
#pragma once

#include "engine/gomoku_engine.h"
#include "gomoku_ai_data.h"
#include "gomoku_ai.h"
#include "utils/gomoku_utilities.h"

class TestAI
{
private:
  int depth;
  int length;
  GomokuAIData evaluation_data;
  Player ai_player;

public:
  TestAI(const GomokuAiSettings &settings);
  std::pair<int, int> suggest_move(const GomokuGame &board);

  int score_player(GomokuGame &game, Player player);
  void find_relevant_moves(GomokuGame &game, std::vector<MoveHeuristic> &out_relevant_moves);
  bool is_cell_relevant(GomokuGame &game, int row, int col);

  void compute_score(Node &node);
  void explore_children(Node &node, int curDepth, bool maximizingPlayer, int alpha, int beta);
};
