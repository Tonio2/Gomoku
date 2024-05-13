#include "ai/gomoku_ai.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

GomokuAI::GomokuAI(const GomokuAiSettings &settings)
    : game(0, 0), depth(settings.depth), length(settings.length), evaluation_data(settings.data), move_count(0), move_evaluated_count(0)
{
}

void sortMovesUtil(std::vector<MoveHeuristic> &moves, bool maximizingPlayer)
{
    TIMER

    std::function<bool(const MoveHeuristic &, const MoveHeuristic &)> compare;

    if (maximizingPlayer)
    {
        compare = [](const MoveHeuristic &a, const MoveHeuristic &b)
        {
            return a.score > b.score;
        };
    }
    else
    {
        compare = [](const MoveHeuristic &a, const MoveHeuristic &b)
        {
            return a.score < b.score;
        };
    }
    std::sort(moves.begin(), moves.end(), compare);
}

void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer)
{
    TIMER
    for (MoveHeuristic &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.row, move.col);
            move.score = _heuristic_evaluation();
            game.reverse_move(game_move);
        }
        catch (std::exception &e)
        {
            move.score = maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1;
        }
    }

    sortMovesUtil(moves, maximizingPlayer);
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    TIMER

    MoveEvaluation node = {{row, col}, 0, 0, 0, {}};

    if (depth == 0 || game.is_game_over())
    {
        if (game.is_game_over())
        {
            if (game.get_winner() == ai_player)
                node.score = std::numeric_limits<int>::max() - 1;
            else if (game.get_winner() == human_player)
                node.score = std::numeric_limits<int>::min() + 1;
            else
                node.score = 0;
        }
        else
            node.score = _heuristic_evaluation();
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<MoveHeuristic> moves;
    find_relevant_moves(moves);

    if (depth > 1)
        sortMoves(moves, maximizingPlayer);
    int moveIdx = 1;
    node.totalEvalCount = moves.size();
    node.evaluatedMoves = moves.size();

    // For each move, make the move, call minimax recursively and reverse the move.
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (const MoveHeuristic &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.row, move.col);
            MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
            game.reverse_move(game_move);

            if (maximizingPlayer)
            {
                if (evalNode.score > extremeEval)
                {
                    extremeEval = evalNode.score;
                    node.score = extremeEval;
                    alpha = std::max(alpha, evalNode.score);
                    node.neededEvalCount = moveIdx;
                }
            }
            else
            {
                if (evalNode.score < extremeEval)
                {
                    extremeEval = evalNode.score;
                    node.score = extremeEval;
                    beta = std::min(beta, evalNode.score);
                    node.neededEvalCount = moveIdx;
                }
            }

            node.listMoves.push_back(evalNode);

            if (beta <= alpha)
            {
                node.evaluatedMoves = moveIdx;
                break;
            }
            moveIdx++;
        }
        catch (std::exception &e)
        {
            move_count--;
        }
    }
    move_count += moves.size();
    move_evaluated_count += node.evaluatedMoves;
    evaluation_needed_count += node.neededEvalCount;
    return node;
}

int GomokuAI::score_player(Player player)
{
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

int GomokuAI::_heuristic_evaluation()
{
    TIMER
    return score_player(ai_player) - score_player(human_player);
}

int GomokuAI::get_heuristic_evaluation(const GomokuGame &board, Player player)
{
    TIMER

    game = board;
    ai_player = player;
    human_player = board.other_player(player);
    return _heuristic_evaluation();
}

MoveEvaluation GomokuAI::suggest_move(const GomokuGame &board)
{
    Timer timer(__FUNCTION__);

    game = board;
    ai_player = board.get_current_player();
    human_player = board.other_player(ai_player);
    move_count = 0;
    move_evaluated_count = 0;
    evaluation_needed_count = 0;
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}

void GomokuAI::find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves) const
{
    TIMER

    auto [min, max] = game.get_played_bounds(length);

    for (int row = min.row; row <= max.row; ++row)
    {
        for (int col = min.col; col <= max.col; ++col)
        {
            if (game.get_board_value(row, col) != E)
                continue;

            if (is_cell_relevant(row, col))
            {
                out_relevant_moves.push_back(
                    MoveHeuristic{uint8_t(row), uint8_t(col), 0});
            }
        }
    }
}

static const std::vector<std::pair<int, int>> _directions_offsets = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

bool GomokuAI::is_cell_relevant(int row, int col) const
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

std::vector<MoveHeuristic> GomokuAI::get_relevant_moves(const GomokuGame &board)
{
    game = board;
    ai_player = board.get_current_player();
    human_player = board.other_player(ai_player);

    std::vector<MoveHeuristic> moves;
    find_relevant_moves(moves);
    return moves;
}

const GomokuAIData &GomokuAI::get_evaluation_data() const
{
    return evaluation_data;
}
