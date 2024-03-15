#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "AI1.h"

#define DEPTH 3

std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0)
{
    // Create indentation based on the depth
    std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

    out << indent << "Move: (" << eval.move.first << ", " << eval.move.second << ") | " << eval.score << "\n";
    if (depth < DEPTH)
    {
        out << indent << "Evals: " << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << "\n";
    }

    if (not eval.listMoves.empty())
    {
        out << indent << "ListMoves:\n";

        for (const auto &move : eval.listMoves)
        {
            // Increase the indentation for each level
            writeMoveEvaluation(out, move, depth + 1);
        }
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

void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0)
{
    if (depth == DEPTH)
    {
        return;
    }

    if (eval.neededEvalCount != eval.evaluatedMoves)
    {
        out << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << "\n";
    }

    if (not eval.listMoves.empty())
    {
        for (const auto &move : eval.listMoves)
        {
            writeSurplusEvaluation(out, move, depth + 1);
        }
    }
}

void logTooManyEvaluationsList(const MoveEvaluation &eval)
{
    std::ofstream out("surplusEval.txt");
    if (!out.is_open())
    {
        std::cerr << "Failed to open log.txt" << std::endl;
        return;
    }

    writeSurplusEvaluation(out, eval);
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

int getCoordinate(char coordinate)
{
    for (int i = 0; i < boardCoordinates.size(); i++)
    {
        if (boardCoordinates[i] == coordinate)
        {
            return i;
        }
    }
    return -1;
}

void test_problems()
{
    // Read problems.txt
    std::ifstream in("problems.txt");
    if (!in.is_open())
    {
        std::cerr << "Failed to open problems.txt" << std::endl;
        return;
    }

    // Loop over each line
    std::string line;
    std::string description;
    while (std::getline(in, line))
    {
        if (line[0] == '#')
        {
            description = line;
            continue;
        }
        // Create a game with the given board size
        GomokuGame game(19);
        std::vector<std::string> problem = split(line, ':');
        std::vector<std::string> moves = split(problem[0], ',');
        // Loop over each character in the line
        for (std::string move : moves)
        {
            // Get the row and column from the line
            int row = getCoordinate(move[0]);
            int col = getCoordinate(move[1]);
            // Make the move
            game.make_move(row, col);
        }
        GomokuAI AI(game, moves.size() % 2 == 0 ? X : O, DEPTH);
        // Suggest a move
        MoveEvaluation moveEvalutation = AI.suggest_move();
        // Get the best move
        std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);

        std::vector<std::string> expected_moves_string;
        std::vector<std::string> expected_moves;
        std::vector<std::string> forbidden_moves;

        if (problem[1].find("|") != std::string::npos) {
            expected_moves_string = split(problem[1], '|');
            expected_moves = split(expected_moves_string[0], ',');
            forbidden_moves = split(expected_moves_string[1], ',');
        } else {
            expected_moves = split(problem[1], ',');
        }

        
        std::string best_move_string = std::to_string(bestMove.first) + std::to_string(bestMove.second);
        if (std::find(forbidden_moves.begin(), forbidden_moves.end(), best_move_string) != forbidden_moves.end())
        {
            // Print the description then print "...NOK" to std::cout in red color
            std::cout << "\033[1;31m" << description << "...NOK"
                      << "\033[0m" << std::endl;
        }
        else if (std::find(expected_moves.begin(), expected_moves.end(), best_move_string) != expected_moves.end())
        {
            // Print the description then print "...OK" to std::cout in green color
            std::cout << "\033[1;32m" << description << "...OK"
                      << "\033[0m" << std::endl;
        }
        else
        {
            std::cout << "\033[1;31m" << description << "...NOK"
                      << "\033[0m" << std::endl;
        }
    }
}

void test_problem(int problem_idx)
{
    std::ifstream in("problems.txt");
    if (!in.is_open())
    {
        std::cerr << "Failed to open problems.txt" << std::endl;
        return;
    }

    // Loop over each line
    std::string line;
    while (std::getline(in, line))
    {
        if (line[0] != '#')
        {
            if (problem_idx == 1)
            {
                break;
            }
            problem_idx--;
        }
    }
    std::vector<std::string> problem = split(line, ':');
    std::vector<std::string> moves = split(problem[0], ',');

    GomokuGame game(19);
    // Loop over each character in the line
    for (std::string move : moves)
    {
        // Get the row and column from the line
        int row = getCoordinate(move[0]);
        int col = getCoordinate(move[1]);
        // Make the move
        game.make_move(row, col);
    }
    // Create an AI with the game and the depth
    std::cout << "Moves: ";
    for (std::string move : moves)
    {
        std::cout << move << " ";
    }
    std::cout << std::endl;
    GomokuAI AI(game, moves.size() % 2 == 0 ? X : O, DEPTH);
    // Suggest a move
    MoveEvaluation moveEvalutation = AI.suggest_move();
    // Get the best move
    std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    // Print the best move
    game.display_board();
    std::cout << "Best move: (" << bestMove.first << ", " << bestMove.second << ")" << std::endl;
    logMoveEvaluation(moveEvalutation);
    logTooManyEvaluationsList(moveEvalutation);
}

int main(int argc, char *argv[])
{
    // If no arguments are given, run the test_problems function
    // If an argument is given, run the test_problem function with the given argument
    if (argc == 1)
        test_problems();
    else
        test_problem(atoi(argv[1]));
    return 0;
}