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
        node.score = heuristic_evaluation();
        return node;
    }

    if (maximizingPlayer)
    {
        int maxEval = std::numeric_limits<int>::min();
        std::vector<std::pair<int, int>> moves = game.findRelevantMoves();
        for (auto move : moves)
        {
            game.make_move(move.first, move.second);
            MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, false, move.first, move.second);
            game.set_board_value(move.first, move.second, E);

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
    else
    {
        int minEval = std::numeric_limits<int>::max();
        std::vector<std::pair<int, int>> moves = game.findRelevantMoves();
        for (auto move : moves)
        {
            game.make_move(move.first, move.second);
            MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, true, move.first, move.second);
            game.set_board_value(move.first, move.second, E);

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
    return node;
}

int GomokuAI::heuristic_evaluation()
{
    std::random_device rd;                          // Obtain a random number from hardware
    std::mt19937 eng(rd());                         // Seed the generator
    std::uniform_int_distribution<> distr(0, 1000); // Define the range

    return distr(eng); // Generate and return a random number
}

MoveEvaluation GomokuAI::suggest_move()
{
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}
