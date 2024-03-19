#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "AI1.h"

#ifndef DEPTH
#define DEPTH 3 // Default depth value
#endif

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
    for (size_t i = 0; i < boardCoordinates.size(); i++)
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

void print_list(std::vector<double> list)
{
    std::cout << "[";
    for (double value : list)
    {
        std::cout << value << ", ";
    }
    std::cout << "]" << std::endl;
}

void test_problems()
{
    std::vector<double> times;
    std::vector<double> move_counts;
    std::vector<double> move_evaluated_counts;
    std::vector<double> evaluation_needed_counts;
    std::vector<double> percentage_moves_evaluated;
    std::vector<double> percentage_evaluations_needed;
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
        GomokuGame game(19, 19);
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
        std::cout << line << std::endl;
        std::cout << "Best move: " << best_move_string << std::endl;
        std::cout << "Total move count: " << AI.move_count << std::endl;
        std::cout << "Total move evaluated count: " << AI.move_evaluated_count << std::endl;
        std::cout << "Total evaluation needed count: " << AI.evaluation_needed_count << std::endl;
        std::cout << "Percentage of moves evaluated: " << (AI.move_evaluated_count * 100) / AI.move_count << "%" << std::endl;
        std::cout << "Percentage of evaluations needed: " << (AI.evaluation_needed_count * 100) / AI.move_evaluated_count << "%" << std::endl;
        // double time = Timer::getAccumulatedTime("suggest_move");
        // std::cout << "Ration time / move count: " << time / AI.move_count << std::endl;
        // std::cout << "Ration time / move evaluated: " << time / AI.move_evaluated_count << std::endl;
        // std::cout << "Ration time / evaluation needed: " << time / AI.evaluation_needed_count << std::endl;

        // times.push_back(time);
        // move_counts.push_back(AI.move_count);
        // move_evaluated_counts.push_back(AI.move_evaluated_count);
        // evaluation_needed_counts.push_back(AI.evaluation_needed_count);
        // percentage_moves_evaluated.push_back((AI.move_evaluated_count * 100) / AI.move_count);
        // percentage_evaluations_needed.push_back((AI.evaluation_needed_count * 100) / AI.move_evaluated_count);
        Timer::printAccumulatedTimes();
        Timer::reset();
    }
    // print_list(times);
    // print_list(move_counts);
    // print_list(move_evaluated_counts);
    // print_list(evaluation_needed_counts);
    // print_list(percentage_moves_evaluated);
    // print_list(percentage_evaluations_needed);
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

    GomokuGame game(19, 19);
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
    std::vector<int> X_patterns = game.get_patterns_count(X);
    std::vector<int> O_patterns = game.get_patterns_count(O);
    std::cout << "X patterns: ";
    for (int pattern : X_patterns)
    {
        std::cout << pattern << " ";
    }
    std::cout << std::endl;
    std::cout << "O patterns: ";
    for (int pattern : O_patterns)
    {
        std::cout << pattern << " ";
    }
    std::cout << std::endl;
}

void test_eval(std::string moves_string)
{
    GomokuGame game(19, 19);
    std::vector<std::string> moves = split(moves_string, ',');
    display_moves(moves);
    apply_moves(game, moves);

    // game.display_board();
    // game.print_patterns();
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

// PatternCellState random_state()
// {
//     return PatternCellState(random() % 3);
// }

// std::ostream &operator<<(std::ostream &stream, std::vector<int> array)
// {
//     stream << '[';
//     for (int value : array)
//     {
//         stream << (value > 0 ? "+" : value == 0 ? " "
//                                                 : "")
//                << value << ' ';
//     }
//     stream << ']';
//     return stream;
// }

// int main()
// {
//     srandom(time(nullptr));
//     GomokuPatternReconizer reconizer(X);

//     PatternCellData cell_data = PatternCellData::pre_bound_element();
//     std::cout << cell_data << std::endl;

//     PatternCellState states[20]{
//         PatternCellState::Empty,
//         PatternCellState::Stoned,
//         PatternCellState::Empty,
//         PatternCellState::Blocked,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Empty,
//         PatternCellState::Stoned,
//         PatternCellState::Empty,
//         PatternCellState::Empty,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Stoned,
//         PatternCellState::Empty,
//     };
//     for (int i = 0; i < 20; i++)
//     {
//         // PatternCellState new_state = random_state();
//         PatternCellState new_state = states[i];

//         cell_data = reconizer.cell_data_following_memoized(cell_data, new_state);

//         std::vector<int> all_structures(StructureType::COUNT_STRUCTURE_TYPE, 0);

//         cell_data.get_structures_type_count(all_structures);

//         std::cout << new_state << " -> " << cell_data << " " << all_structures << std::endl;
//     }
// }
