
#include "test_engine.h"

void test_problems()
{
    double totalTime = 0;
    int problemCount = 0;
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
        totalTime += Timer::getAccumulatedTime("suggest_move");
        problemCount++;
        Timer::printAccumulatedTimes();
        Timer::reset();
    }
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Average time: " << totalTime / problemCount << " ms" << std::endl;
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
    apply_moves(game, moves);
    Player last_player = moves.size() % 2 ? X : O;
    GomokuAI AI(game, last_player, DEPTH);
    int evaluation = AI.heuristic_evaluation();

    // Display moves
    display_moves(moves);

    // Display the board
    game.display_board();

    // Display if game is over
    std::cout << "Is game over: " << game.is_game_over() << "\n";

    // Display patterns
    game.print_patterns();

    // Display player's to play next move
    std::cout << "Next move to: " << game.other_player(last_player) << std::endl;

    // Display the evaluation
    std::cout << "Board evaluation for " << last_player << ": " << evaluation << std::endl;
}

int main(int argc, char *argv[])
{
    // If no arguments are given, run the test_problems function
    // If an argument is given, run the test_problem function with the given argument
    if (argc == 1)
        test_problems();
    else if (std::string(argv[1]) == "eval")
    {
        std::string moves_string = std::string(argv[2]);
        test_eval(moves_string);
    }
    else if (std::string(argv[1]) == "engine")
    {
        test_engine();
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
