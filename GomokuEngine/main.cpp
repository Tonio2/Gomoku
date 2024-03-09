#include <iostream>
#include <fstream>
#include <utility>

#include "AI1.h"

std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

void displayBoard(GomokuGame &game)
{
    std::cout << " ";
    for (char coordinate : boardCoordinates)
    {
        std::cout << " " << coordinate;
    }
    std::cout << std::endl;
    for (int i = 0; i < game.get_board_size(); i++)
    {
        std::cout << boardCoordinates[i] << " ";
        for (int j = 0; j < game.get_board_size(); j++)
        {
            if (game.get_board_value(i, j) == X)
            {
                std::cout << "X ";
            }
            else if (game.get_board_value(i, j) == O)
            {
                std::cout << "O ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0)
{
    // Create indentation based on the depth
    std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

    out << indent << "Move: (" << eval.move.first << ", " << eval.move.second << ")\n";
    out << indent << "Score: " << eval.score << "\n";
    if (not eval.listMoves.empty())
    {
        out << indent << "ListMoves:\n";

        for (const auto &move : eval.listMoves)
        {
            // Increase the indentation for each level
            writeMoveEvaluation(out, move, depth + 1);
        }
    } else {
        out << indent << "-----------\n";
    }
}

void logMoveEvaluation(const MoveEvaluation &eval)
{
    std::ofstream out("log.txt");
    if (!out.is_open())
    {
        std::cerr << "Failed to open log.txt" << std::endl;
        return;
    }

    writeMoveEvaluation(out, eval);
    out.close();
}

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer)
{
    int bestScore = std::numeric_limits<int>::min();
    int worstScore = std::numeric_limits<int>::max();
    std::pair<int, int> bestMove = {-1, -1};

    for (const auto &move : eval.listMoves)
    {
        if (maximizingPlayer)
        {
            if (move.score > bestScore)
            {
                bestScore = move.score;
                bestMove = move.move;
            }
        }
        else
        {
            if (move.score < worstScore)
            {
                worstScore = move.score;
                bestMove = move.move;
            }
        }
    }

    return bestMove;
}

int main()
{
    std::cout << "Hello, World!" << std::endl;
    GomokuGame game(19);
    game.make_move(4, 4);
    game.make_move(2, 2);
    game.make_move(5, 4);
    // game.make_move(0, 0);
    // game.make_move(0, 1);
    game.display_struct();
    GomokuAI AI(game, O, 3);
    MoveEvaluation moveEvalutation = AI.suggest_move();
    logMoveEvaluation(moveEvalutation);
    displayBoard(game);
    std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    std::cout << "Best move: (" << bestMove.first << ", " << bestMove.second << ")" << std::endl;
    return 0;
}