#include "ai/gomoku_ai.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

GomokuAI::GomokuAI(int depth, GomokuAIData data)
    : game(0, 0), depth(depth), evaluation_data(data), move_count(0), move_evaluated_count(0)
{
}

void sortMovesUtil(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth)
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

void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth)
{
    TIMER
    for (MoveHeuristic &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.row, move.col);
            move.score = heuristic_evaluation(game, ai_player);
            game.reverse_move(game_move);
        }
        catch (std::exception &e)
        {
            move.score = maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1;
        }
    }

    sortMovesUtil(moves, maximizingPlayer, depth);
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
            node.score = heuristic_evaluation(game, ai_player);
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<MoveHeuristic> moves = find_relevant_moves();
    if (depth > 1)
        sortMoves(moves, maximizingPlayer, depth);
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
    score += (game.get_patterns_count(player)[OPEN_THREE] + game.get_patterns_count(player)[FOUR] + game.get_patterns_count(player)[OPEN_FOUR] >= 2) ? evaluation_data.value_of_multiple_forced() : 0;
    score += (game.get_patterns_count(player)[OPEN_FOUR] >= 2 ? evaluation_data.value_of_multiple_o4() : 0);
    score += evaluation_data.value_of_captures(game.get_player_score(player));
    return score;
}

int GomokuAI::heuristic_evaluation(const GomokuGame &board, Player heuristic_player)
{
    TIMER
    return score_player(heuristic_player) - score_player(board.other_player(heuristic_player));
}

int GomokuAI::pseudo_heuristic_evaluation(std::pair<int, int> move)
{
    return 0;
}

MoveEvaluation GomokuAI::suggest_move(const GomokuGame &board)
{
    TIMER

    game = board;
    ai_player = board.get_current_player();
    human_player = board.other_player(ai_player);
    move_count = 0;
    move_evaluated_count = 0;
    evaluation_needed_count = 0;
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}

std::vector<MoveHeuristic> GomokuAI::find_relevant_moves() const
{
    TIMER
    std::vector<MoveHeuristic> relevantMoves;

    // Directions to check around each cell (8 directions).
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int row = 0; row < game.get_board_height(); ++row)
    {
        for (int col = 0; col < game.get_board_width(); ++col)
        {
            if (game.get_board_value(row, col) == E)
            { // Empty cell
                bool foundStone = false;
                for (const auto &dir : directions)
                {
                    for (int step = 1; step <= 2; ++step)
                    {
                        int newRow = row + step * dir.first;
                        int newCol = col + step * dir.second;

                        if (game.coordinates_are_valid(newRow, newCol) && game.get_board_value(newRow, newCol) != E)
                        {
                            relevantMoves.push_back(
                                MoveHeuristic{uint8_t(row), uint8_t(col), 0});
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

const GomokuAIData &GomokuAI::get_evaluation_data() const
{
    return evaluation_data;
}
