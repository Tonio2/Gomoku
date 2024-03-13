#include "AI1.h"
#include <algorithm>

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), ai_player(ai_player), depth(depth)
{
    human_player = (ai_player == X) ? O : X;
}

void GomokuAI::sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer)
{
    for (std::pair<std::pair<int, int>, int> &move : moves)
    {
        std::vector<std::vector<Structure>> structuresAll = game.get_structures();
        MoveResult game_move = game.make_move(move.first.first, move.first.second);
        move.second = heuristic_evaluation();
        game.reverse_move(game_move);
        game.set_structures(structuresAll);
    }
    if (maximizingPlayer)
        std::sort(moves.begin(), moves.end(), [](const std::pair<std::pair<int, int>, int> &a, const std::pair<std::pair<int, int>, int> &b)
                  { return a.second > b.second; });
    else
        std::sort(moves.begin(), moves.end(), [](const std::pair<std::pair<int, int>, int> &a, const std::pair<std::pair<int, int>, int> &b)
                  { return a.second < b.second; });
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    // If the depth is 0 or the game is over, return the heuristic evaluation of the current board.
    MoveEvaluation node;
    node.move = {row, col}; // Initialize with an invalid move.
    if (depth == 0 || game.is_game_over())
    {
        node.score = game.is_game_over() ? (maximizingPlayer ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min()) : heuristic_evaluation();
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<std::pair<std::pair<int, int>, int>> moves = game.findRelevantMoves();
    if (depth > 1)
    {
        sortMoves(moves, maximizingPlayer);
    }

    // For each move, make the move, call minimax recursively and reverse the move.
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (auto moveWithScore : moves)
    {
        std::pair<int, int> move = moveWithScore.first;
        std::vector<std::vector<Structure>> structuresAll = game.get_structures();
        MoveResult game_move = game.make_move(move.first, move.second);
        MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, !maximizingPlayer, move.first, move.second);
        game.reverse_move(game_move);
        game.set_structures(structuresAll);

        if (maximizingPlayer)
        {
            if (evalNode.score > extremeEval)
            {
                extremeEval = evalNode.score;
                node.score = extremeEval;
                alpha = std::max(alpha, evalNode.score);
            }
        }
        else
        {
            if (evalNode.score < extremeEval)
            {
                extremeEval = evalNode.score;
                node.score = extremeEval;
                beta = std::min(beta, evalNode.score);
            }
        }

        node.listMoves.push_back(evalNode);

        if (beta <= alpha)
        {
            break;
        }
    }
    return node;
}

int GomokuAI::heuristic_evaluation()
{
    int player = ai_player;
    std::vector<std::vector<Structure>> structuresAll = game.get_structures();
    int multiplier = 1;
    int score = 0;
    std::vector<int> counts(8, 0);

    for (int i = 0; i < 2; i++)
    {
        for (Structure &structure : structuresAll[player])
        {
            counts[structure.type] += 1;
        }

        score += counts[OPEN_FOUR] * 10000 * multiplier;
        score += (counts[OPEN_THREE] >= 2 or counts[FOUR] >= 2 or (counts[OPEN_THREE] >= 1 and counts[FOUR] >= 1)) * 9000 * multiplier;
        score += counts[OPEN_THREE] * 1000 * multiplier;
        score += counts[THREE] * 500 * multiplier;
        score += counts[OPEN_TWO] * 100 * multiplier;
        score += counts[TWO] * 50 * multiplier;
        score += counts[OPEN_ONE] * 10 * multiplier;
        score += counts[ONE] * 5 * multiplier;
        player = human_player;
        multiplier = -1;
        counts = {0, 0, 0, 0, 0, 0, 0, 0};
    }
    return score;
}

MoveEvaluation GomokuAI::suggest_move()
{
    Timer timer("suggest_move");
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}
