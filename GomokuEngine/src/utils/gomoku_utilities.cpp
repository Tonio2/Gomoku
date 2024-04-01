
#include "utils/gomoku_utilities.h"
#include <sstream>

int char_to_coordinate(char coordinate)
{
    if (coordinate >= 'A' && coordinate <= 'Z')
        return coordinate - 'A' + 10;

    if (coordinate >= '0' && coordinate <= '9')
        return coordinate - '0';

    return -1;
}

char coordinate_to_char(int coordinate)
{
    if (coordinate >= 0 && coordinate < 10)
        return '0' + coordinate;

    if (coordinate >= 10 && coordinate < 36)
        return 'A' + (coordinate - 10);

    return '?';
}

void apply_moves(GomokuGame& game, std::vector<std::string> moves)
{
    for (std::string move : moves)
    {
        int row = char_to_coordinate(move[0]);
        int col = char_to_coordinate(move[1]);

        game.make_move(row, col);
    }
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

void apply_moves(GomokuGame &game, std::string move_str)
{
    std::vector<std::string> moves = split(move_str, ',');
    return apply_moves(game, moves);
}

// void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth)
// {
//     // Create indentation based on the depth
//     std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

//     out << indent << "Move: " << boardCoordinates[eval.move.first] << boardCoordinates[eval.move.second] << " | " << eval.score << "\n";
//     if (depth < DEPTH)
//     {
//         out << indent << "Evals: " << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << "\n";
//     }

//     if (not eval.listMoves.empty())
//     {
//         out << indent << "ListMoves:\n";

//         for (const auto &move : eval.listMoves)
//         {
//             // Increase the indentation for each level
//             writeMoveEvaluation(out, move, depth + 1);
//         }
//     }
// }

// void logMoveEvaluation(const MoveEvaluation &eval)
// {
//     std::ofstream out("log.txt");
//     if (!out.is_open())
//     {
//         std::cerr << "Failed to open log.txt" << std::endl;
//         return;
//     }

//     writeMoveEvaluation(out, eval);
//     out.close();
// }

// void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth)
// {
//     if (depth == DEPTH)
//     {
//         return;
//     }

//     if (eval.neededEvalCount != eval.evaluatedMoves)
//     {
//         out << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << "\n";
//     }

//     if (not eval.listMoves.empty())
//     {
//         for (const auto &move : eval.listMoves)
//         {
//             writeSurplusEvaluation(out, move, depth + 1);
//         }
//     }
// }

// void logTooManyEvaluationsList(const MoveEvaluation &eval)
// {
//     std::ofstream out("surplusEval.txt");
//     if (!out.is_open())
//     {
//         std::cerr << "Failed to open log.txt" << std::endl;
//         return;
//     }

//     writeSurplusEvaluation(out, eval);
//     out.close();
// }

// std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer)
// {
//     int bestScore = std::numeric_limits<int>::min();
//     int worstScore = std::numeric_limits<int>::max();
//     std::pair<int, int> bestMove = {-1, -1};

//     for (const auto &move : eval.listMoves)
//     {
//         if (maximizingPlayer)
//         {
//             if (move.score > bestScore)
//             {
//                 bestScore = move.score;
//                 bestMove = move.move;
//             }
//         }
//         else
//         {
//             if (move.score < worstScore)
//             {
//                 worstScore = move.score;
//                 bestMove = move.move;
//             }
//         }
//     }

//     return bestMove;
// }
