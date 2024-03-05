#include "AI1.h"

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), ai_player(ai_player), depth(depth)
{
    human_player = (ai_player == X) ? O : X;
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
    MoveEvaluation node;
    node.move = {-1, -1};  // Initialize with an invalid move.
    if (depth == 0 || game.is_game_over()) {
        node.score = heuristic_evaluation();
        return node;
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (int i = 0; i < game.get_board_size(); i++) {
            for (int j = 0; j < game.get_board_size(); j++) {
                if (game.get_board_value(i, j) == E) {
                    game.make_move(i, j);
                    MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, false);
                    game.set_board_value(i, j, E);

                    if (evalNode.score > maxEval) {
                        maxEval = evalNode.score;
                        node.move = {i, j};
                        node.score = maxEval;
                        alpha = std::max(alpha, evalNode.score);
                    }

                    node.listMoves.push_back(evalNode);

                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (int i = 0; i < game.get_board_size(); i++) {
            for (int j = 0; j < game.get_board_size(); j++) {
                if (game.get_board_value(i, j) == E) {
                    game.make_move(i, j);
                    MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, true);
                    game.set_board_value(i, j, E);

                    if (evalNode.score < minEval) {
                        minEval = evalNode.score;
                        node.move = {i, j};
                        node.score = minEval;
                        beta = std::min(beta, evalNode.score);
                    }

                    node.listMoves.push_back(evalNode);

                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
    }
    return node;
}


// std::pair<int, std::pair<int, int>> GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer)
// {
//     std::pair<int, int> bestMove = {-1, -1};
//     if (depth == 0 || game.is_game_over())
//     {
//         return {heuristic_evaluation(), bestMove};
//     }

//     if (maximizingPlayer)
//     {
//         int maxEval = std::numeric_limits<int>::min();
//         for (int i = 0; i < game.get_board_size(); i++)
//         {
//             for (int j = 0; j < game.get_board_size(); j++)
//             {
//                 if (game.get_board_value(i, j) == E)
//                 {
//                     game.make_move(i, j); // Remplacer par is_legal to prevent capture
//                     auto result = minimax(depth - 1, alpha, beta, false);
//                     int eval = result.first;
//                     game.set_board_value(i, j, E);

//                     if (eval > maxEval)
//                     {
//                         maxEval = eval;
//                         bestMove = {i, j};
//                         alpha = std::max(alpha, eval);
//                     }

//                     if (beta <= alpha)
//                     {
//                         std::cout << "Pruning at depth " << depth << std::endl;
//                         break;
//                     }
//                 }
//             }
//         }
//         return {maxEval, bestMove};
//     }
//     else
//     {
//         int minEval = std::numeric_limits<int>::max();
//         for (int i = 0; i < game.get_board_size(); i++)
//         {
//             for (int j = 0; j < game.get_board_size(); j++)
//             {
//                 if (game.get_board_value(i, j) == E)
//                 {
//                     game.make_move(i, j);
//                     auto result = minimax(depth - 1, alpha, beta, true);
//                     int eval = result.first;
//                     game.set_board_value(i, j, E);

//                     if (eval < minEval)
//                     {
//                         minEval = eval;
//                         beta = std::min(beta, eval);
//                     }

//                     if (beta <= alpha)
//                     {
//                         std::cout << "Pruning at depth " << depth << std::endl;
//                         break;
//                     }
//                 }
//             }
//         }
//         return {minEval, bestMove};
//     }
// }

int GomokuAI::heuristic_evaluation()
{
    std::random_device rd;                          // Obtain a random number from hardware
    std::mt19937 eng(rd());                         // Seed the generator
    std::uniform_int_distribution<> distr(0, 1000); // Define the range

    return distr(eng); // Generate and return a random number
}

MoveEvaluation GomokuAI::suggest_move()
{
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);
    return result;
}
