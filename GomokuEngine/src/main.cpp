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

    out << indent << "Move: " << boardCoordinates[eval.move.first] << boardCoordinates[eval.move.second] << " | " << eval.score << "\n";
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

void display_moves(std::vector<std::string> moves)
{
    for (std::string move : moves)
    {
        std::cout << move << " ";
    }
    std::cout << std::endl;
}

void apply_moves(GomokuGame &game, std::vector<std::string> moves)
{
    for (std::string move : moves)
    {
        // Get the row and column from the line
        int row = getCoordinate(move[0]);
        int col = getCoordinate(move[1]);
        // Make the move
        game.make_move(row, col);
    }
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
        apply_moves(game, moves);
        GomokuAI AI(game, moves.size() % 2 == 0 ? X : O, DEPTH);
        // Suggest a move
        MoveEvaluation moveEvalutation = AI.suggest_move();
        // Get the best move
        std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);

        std::vector<std::string> expected_moves_string;
        std::vector<std::string> expected_moves;
        std::vector<std::string> forbidden_moves;

        if (problem[1].find("|") != std::string::npos)
        {
            expected_moves_string = split(problem[1], '|');
            expected_moves = split(expected_moves_string[0], ',');
            forbidden_moves = split(expected_moves_string[1], ',');
        }
        else
        {
            expected_moves = split(problem[1], ',');
        }

        std::string best_move_string = std::string(1, boardCoordinates[bestMove.first]) + std::string(1, boardCoordinates[bestMove.second]);
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
        std::cout << "Best move: " << best_move_string << std::endl;

        Timer::printAccumulatedTimes();
        Timer::reset();
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
    display_moves(moves);
    Player player = moves.size() % 2 ? O : X;

    GomokuGame game(19);
    apply_moves(game, moves);

    GomokuAI AI(game, player, DEPTH);
    // Suggest a move
    MoveEvaluation moveEvalutation = AI.suggest_move();
    // Get the best move
    std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    // Print the best move
    game.display_board();
    std::cout << "Best move for player " << player << ": " << boardCoordinates[bestMove.first] << boardCoordinates[bestMove.second] << std::endl;
    logMoveEvaluation(moveEvalutation);
    logTooManyEvaluationsList(moveEvalutation);
    Timer::printAccumulatedTimes();
    std::vector<std::vector<int>> patterns_count = game.get_patterns_count();
    for (int i = 0; i < patterns_count.size(); i++)
    {
        for (int j = 0; j < patterns_count[i].size(); j++)
        {
            std::cout << patterns_count[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void test_eval(std::string moves_string)
{
    GomokuGame game(19);
    std::vector<std::string> moves = split(moves_string, ',');
    display_moves(moves);
    apply_moves(game, moves);

    game.display_board();
    game.print_patterns();
    std::vector<std::vector<int>> patterns_count = game.get_patterns_count();
    for (int i = 1; i < patterns_count.size(); i++)
    {
        std::cout << "Patterns count for player " << (i == 1 ? "X: " : "O: ");
        for (int j = 0; j < patterns_count[i].size(); j++)
        {
            std::cout << patterns_count[i][j] << " ";
        }
        std::cout << std::endl;
    }
    Player player = moves.size() % 2 ? X : O;
    GomokuAI AI(game, player, DEPTH);
    int evaluation = AI.heuristic_evaluation();
    std::cout << "Board evaluation for " << player << ": " << evaluation << std::endl;
}

int main(int argc, char *argv[])
{
    // If no arguments are given, run the test_problems function
    // If an argument is given, run the test_problem function with the given argument
    if (argc == 1)
        test_problems();
    else if (std::string(argv[1]).compare("eval") == 0)
    {
        std::string moves_string = "44,55,45,00,35,40,46,53,47,25";
        test_eval(moves_string);
    }
    else
    {
        test_problem(atoi(argv[1]));
    }
    return 0;
}

// GomokuPatternReconizer::CellState random_state() {
//     return GomokuPatternReconizer::CellState(random() % 3);
// }

// void print_cell_state(GomokuPatternReconizer::CellState state)
// {
//     switch (state) {
//         case GomokuPatternReconizer::CellState::CELL_STATE_EMPTY:
//             std::cout << "_";
//             break;
//         case GomokuPatternReconizer::CellState::CELL_STATE_STONE:
//             std::cout << "O";
//             break;
//         case GomokuPatternReconizer::CellState::CELL_STATE_BLOCK:
//             std::cout << "X";
//             break;
//     }
// }

// int main()
// {
//     GomokuPatternReconizer reconizer;

//     GomokuPatternReconizer::CellData cell_data = GomokuPatternReconizer::CellData::pre_bound_element();
//     cell_data.print();
//     std::cout << std::endl;

//     GomokuPatternReconizer::CellState states[20] {
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_BLOCK,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_BLOCK,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_STONE,
//         GomokuPatternReconizer::CellState::CELL_STATE_EMPTY,
//     };
//     for (int i = 0; i < 20; i++)
//     {
//         // GomokuPatternReconizer::CellState new_state = random_state();
//         GomokuPatternReconizer::CellState new_state = states[i];
//         print_cell_state(new_state);
//         std::cout << " -> ";
//         cell_data = reconizer.cell_data_following(cell_data, new_state);
//         cell_data.print();
//         std::cout << std::endl;
//     }
// }
