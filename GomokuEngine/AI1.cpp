#include "AI1.h"

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), ai_player(ai_player), depth(depth)
{
    human_player = (ai_player == X) ? O : X;
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    MoveEvaluation node;
    node.move = {row, col}; // Initialize with an invalid move.
    if (depth == 0 || game.is_game_over())
    {
        if (game.is_game_over())
        {
            if (maximizingPlayer)
            {
                node.score = std::numeric_limits<int>::max();
            }
            else
            {
                node.score = std::numeric_limits<int>::min();
            }
        }
        node.score = heuristic_evaluation(game.get_current_player() == 1 ? 2 : 1);
        return node;
    }

    if (maximizingPlayer)
    {
        int maxEval = std::numeric_limits<int>::min();
        std::vector<std::pair<int, int>> moves = game.findRelevantMoves();
        for (auto move : moves)
        {
            if (game.get_board_value(move.first, move.second) == E)
            {
                MoveResult game_move = game.make_move(move.first, move.second);
                MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, false, move.first, move.second);
                game.reverse_move(game_move);

                if (evalNode.score > maxEval)
                {
                    maxEval = evalNode.score;
                    node.score = maxEval;
                    alpha = std::max(alpha, evalNode.score);
                }

                node.listMoves.push_back(evalNode);

                if (beta <= alpha)
                {
                    break;
                }
            }
        }
    }
    else
    {
        int minEval = std::numeric_limits<int>::max();
        std::vector<std::pair<int, int>> moves = game.findRelevantMoves();
        for (auto move : moves)
        {
            if (game.get_board_value(move.first, move.second) == E)
            {
                MoveResult game_move = game.make_move(move.first, move.second);
                MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, true, move.first, move.second);
                game.reverse_move(game_move);

                if (evalNode.score < minEval)
                {
                    minEval = evalNode.score;
                    node.score = minEval;
                    beta = std::min(beta, evalNode.score);
                }

                node.listMoves.push_back(evalNode);

                if (beta <= alpha)
                {
                    break;
                }
            }
        }
    }
    return node;
}

int GomokuAI::heuristic_evaluation(unsigned char player)
{
    std::vector<Structure> structures = game.get_structures()[player];
    int score = 0;
    std::vector<int> counts(8, 0);

    for (Structure &structure : structures)
    {
        counts[structure.type] += 1;
    }

    if (counts[OPEN_FOUR] >= 1)
    {
        score += 10000;
    }
    else if (counts[OPEN_THREE] >= 2 or counts[FOUR] >= 2 or (counts[OPEN_THREE] >= 1 and counts[FOUR] >= 1))
    {
        score += 9000;
    } else {
        score += counts[OPEN_THREE] * 1000;
        score += counts[THREE] * 500;
        score += counts[OPEN_TWO] * 100;
        score += counts[TWO] * 50;
        score += counts[OPEN_ONE] * 10;
        score += counts[ONE] * 5;
    }

    return score;
}

MoveEvaluation GomokuAI::suggest_move()
{
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}
