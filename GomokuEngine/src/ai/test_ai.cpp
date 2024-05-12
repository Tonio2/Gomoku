#include "ai/test_ai.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

TestAI::TestAI(const GomokuAiSettings &settings)
    : depth(settings.depth), length(settings.length), evaluation_data(settings.data)
{
}

Node::Node(GomokuGame &game, const uint8_t row, const uint8_t col)
    : game(game), move(row, col), score(0)
{
  // std::cout << "Node for move " << (int)row << " " << (int)col << std::endl;
}

int TestAI::score_player(GomokuGame &game, Player player)
{
  TIMER

  int score = 0;

  const std::vector<int> &patterns_count = game.get_patterns_count(player);

  for (int i = 0; i < StructureType::COUNT_STRUCTURE_TYPE; i++)
  {
    score += patterns_count[i] * evaluation_data.value_of_structure(i);
  }
  score += (patterns_count[OPEN_THREE] + patterns_count[FOUR] + patterns_count[OPEN_FOUR] >= 2) ? evaluation_data.value_of_multiple_forced() : 0;
  score += (patterns_count[OPEN_FOUR] >= 2 ? evaluation_data.value_of_multiple_o4() : 0);
  score += evaluation_data.value_of_captures(game.get_player_score(player));
  return score;
}

void TestAI::compute_score(Node &node)
{
  TIMER

  node.score = score_player(node.game, ai_player) - score_player(node.game, node.game.other_player(ai_player));
}

void TestAI::find_relevant_moves(GomokuGame &game, std::vector<MoveHeuristic> &out_relevant_moves)
{
  TIMER

  auto [min, max] = game.get_played_bounds(length);

  for (int row = min.row; row <= max.row; ++row)
  {
    for (int col = min.col; col <= max.col; ++col)
    {
      if (game.get_board_value(row, col) != E)
        continue;

      if (is_cell_relevant(game, row, col))
      {
        out_relevant_moves.push_back(
            MoveHeuristic{uint8_t(row), uint8_t(col), 0});
      }
    }
  }
}

static const std::vector<std::pair<int, int>> _directions_offsets = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

bool TestAI::is_cell_relevant(GomokuGame &game, int row, int col)
{
  for (const auto &dir : _directions_offsets)
  {
    for (int step = 1; step <= length; ++step)
    {
      int newRow = row + step * dir.first;
      int newCol = col + step * dir.second;

      if (game.coordinates_are_valid(newRow, newCol) && game.get_board_value(newRow, newCol) != E)
      {
        return true;
      }
    }
  }

  return false;
}

void sortMovesUtil(std::vector<Node *> &moves, bool maximizingPlayer)
{
  TIMER

  std::function<bool(const Node *, const Node *)> compare;

  if (maximizingPlayer)
  {
    compare = [](const Node *a, const Node *b)
    {
      return a->score > b->score;
    };
  }
  else
  {
    compare = [](const Node *a, const Node *b)
    {
      return a->score < b->score;
    };
  }
  std::sort(moves.begin(), moves.end(), compare);
}

void TestAI::explore_children(Node &node, int curDepth, bool maximizingPlayer, int alpha, int beta)
{
  TIMER

  if (curDepth == 0 || node.game.is_game_over())
  {
    if (node.game.is_game_over())
    {
      if (node.game.get_winner() == ai_player)
        node.score = std::numeric_limits<int>::max() - 1;
      else if (node.game.get_winner() == node.game.other_player(ai_player))
        node.score = std::numeric_limits<int>::min() + 1;
      else
        node.score = 0;
    }
    else
    {
      compute_score(node);
    }
    return;
  }

  std::vector<MoveHeuristic> moves;
  find_relevant_moves(node.game, moves);
  int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

  if (curDepth > 1)
  {
    for (const auto &move : moves)
    {
      try
      {
        Node *child = new Node(node.game, move.row, move.col);
        child->game.make_move(move.row, move.col);
        compute_score(*child);
        node.children.push_back(child);
      }
      catch (std::exception &e)
      {
      }
    }
    sortMovesUtil(node.children, maximizingPlayer);

    for (Node *child : node.children)
    {
      explore_children(*child, curDepth - 1, !maximizingPlayer, alpha, beta);
      if (maximizingPlayer)
      {
        if (child->score > extremeEval)
        {
          extremeEval = child->score;
          alpha = std::max(alpha, child->score);
          node.score = extremeEval;
        }
      }
      else
      {
        if (child->score < extremeEval)
        {
          extremeEval = child->score;
          beta = std::min(beta, child->score);
          node.score = extremeEval;
        }
      }

      if (alpha >= beta)
      {
        break;
      }
    }
  }
  else
  {
    for (const auto &move : moves)
    {
      try
      {
        Node *child = new Node(node.game, move.row, move.col);
        child->game.make_move(move.row, move.col);
        node.children.push_back(child);
        explore_children(*child, curDepth - 1, !maximizingPlayer, alpha, beta);
        if (maximizingPlayer)
        {
          if (child->score > extremeEval)
          {
            extremeEval = child->score;
            alpha = std::max(alpha, child->score);
            node.score = extremeEval;
          }
        }
        else
        {
          if (child->score < extremeEval)
          {
            extremeEval = child->score;
            beta = std::min(beta, child->score);
            node.score = extremeEval;
          }
        }

        if (alpha >= beta)
        {
          break;
        }
      }
      catch (std::exception &e)
      {
      }
    }
  }
}

std::pair<int, int> TestAI::suggest_move(const GomokuGame &board)
{
  TIMER
  SOLOTIMER

  GomokuGame game(board);
  ai_player = game.get_current_player();
  Node root(game);
  explore_children(root, depth, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

  logNode(root);

  int maxScore = std::numeric_limits<int>::min();
  int row = -1;
  int col = -1;
  for (Node *child : root.children)
  {
    if (child->score > maxScore)
    {
      maxScore = child->score;
      row = child->move.row;
      col = child->move.col;
    }
  }

  ft_free(root);

  return std::make_pair(row, col);
}