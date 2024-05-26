
#include "ai/gomoku_ai_data.h"
#include "ai/gomoku_ai_minmaxv2.h"
#include "arena/arena.h"
#include "engine/gomoku_engine.h"
#include "room/game_room.h"
#include "utils/gomoku_utilities.h"
#include <fstream>

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

        AI::GomokuAiSettings ai_settings;
        ai_settings.depth = get_depth_from_env();
        AI::MinMaxV2::GomokuAI AI(ai_settings);
        // Suggest a move
        AI::MoveEvaluation moveEvalutation = AI.suggest_move_evaluation(game);
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

        std::string best_move_string = std::string(1, coordinate_to_char(bestMove.first)) + std::string(1, coordinate_to_char(bestMove.second));
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
        // #ifndef NDEBUG
        //         std::cout << "Total move count: " << AI.move_count << std::endl;
        //         std::cout << "Total move evaluated count: " << AI.move_evaluated_count << std::endl;
        //         std::cout << "Total evaluation needed count: " << AI.evaluation_needed_count << std::endl;
        //         std::cout << "Percentage of moves evaluated: " << (AI.move_evaluated_count * 100) / AI.move_count << "%" << std::endl;
        //         std::cout << "Percentage of evaluations needed: " << (AI.evaluation_needed_count * 100) / AI.move_evaluated_count << "%" << std::endl;
        // #endif
        totalTime += Timer::getAccumulatedTime("suggest_move_evaluation");
        problemCount++;
        Timer::printAccumulatedTimes();
        Timer::reset();
    }
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Average time: " << totalTime / problemCount << " ms" << std::endl;
}

void test_problem(int problem_idx)
{
    if (problem_idx <= 0)
        return;

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
    if (problem_idx != 1)
        return;
    std::vector<std::string> problem = split(line, ':');
    std::vector<std::string> moves = split(problem[0], ',');
    std::cout << moves << std::endl;
    Player player = moves.size() % 2 ? O : X;

    GomokuGame game(19, 19);
    apply_moves(game, moves);

    AI::GomokuAiSettings ai_settings;
    ai_settings.depth = get_depth_from_env();
    AI::MinMaxV2::GomokuAI AI(ai_settings);
    // Suggest a move
    AI::MoveEvaluation moveEvalutation = AI.suggest_move_evaluation(game);
    // Get the best move
    std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    // Print the best move
    std::cout << to_string(game, true);
    std::cout << "Best move for player " << player << ": " << coordinate_to_char(bestMove.first) << coordinate_to_char(bestMove.second) << std::endl;
    logMoveEvaluation(moveEvalutation, "log.txt");
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

const GomokuAIData ai_data = []()
{
    GomokuAIData data;
    data.values[0] = 0;
    data.values[1] = 1.18881e+09;
    data.values[2] = 175.305;
    data.values[3] = 0.488359;
    data.values[4] = 22.639;
    data.values[5] = 1.59943;
    data.values[6] = 375669;
    data.values[7] = 21.7746;
    data.values[8] = 5684.55;
    data.values[9] = 16355.2;
    data.values[10] = 7501.18;
    data.values[11] = 758.142;
    data.values[12] = 0.00172756;
    data.values[13] = 7.01941;
    data.values[14] = 74.5009;
    return data;
}();

void test_eval(std::string moves_string)
{
    GomokuGame game(19, 19);

    std::vector<std::string> moves = split(moves_string, ',');
    apply_moves(game, moves);

    Player last_player = game.get_current_player();
    // AI::MinMaxV2::GomokuAiSettings ai_settings;
    // ai_settings.depth = get_depth_from_env();
    // AI::MinMaxV2::GomokuAI AI(ai_settings);
    AI::MinMaxV2::GomokuAI AI({4, 2, ai_data});
    int evaluation = AI.get_heuristic_evaluation(game, last_player);

    // Display moves
    std::cout << moves << std::endl;

    // Display the board
    std::cout << to_string(game, true, 2);
    auto bounds = game.get_played_bounds();
    std::cout << "Played bounds:"
              << "row[" << int(bounds.first.row) << "-" << int(bounds.second.row) << "]"
              << "col[" << int(bounds.first.col) << "-" << int(bounds.second.col) << "]"
              << std::endl;

    // Display if game is over and winner
    std::cout << "Is game over: " << game.is_game_over() << "\n";
    std::cout << "Winner: " << game.get_winner() << "\n";
    std::cout << "Scores: X:" << game.get_player_score(X) << " O:" << game.get_player_score(O) << std::endl;

    // Display patterns
    game.print_patterns();

    // Display player's to play next move
    std::cout << "Next move to: " << last_player << std::endl;

    // Display the evaluation
    std::cout << "Board evaluation for " << last_player << ": " << evaluation << std::endl;

    // Display the suggested move
    std::vector<AI::MoveHeuristic> relevant_moves = AI.get_relevant_moves(game);
    std::cout << "Relevant moves (" << relevant_moves.size() << "): [";
    for (auto move : relevant_moves)
    {
        std::cout << "(" << int(move.row) << "," << int(move.col) << "),";
    }
    std::cout << "]" << std::endl;

    AI::MoveEvaluation moveEvaluation = AI.suggest_move_evaluation(game);
    logMoveEvaluation(moveEvaluation, "log.txt");
    std::pair<int, int> bestMove = getBestMove(moveEvaluation, true);
    std::cout << "Suggested move: " << bestMove.first << "," << bestMove.second << std::endl;
}

std::ostream &operator<<(std::ostream &stream, std::vector<int> array)
{
    stream << '[';
    for (int value : array)
    {
        stream << (value > 0 ? "+" : value == 0 ? " "
                                                : "")
               << value << ' ';
    }
    stream << ']';
    return stream;
}

void test_line(const std::string &line)
{
    GomokuGame game(line.size(), 1);

    for (size_t i = 0; i < line.size(); i++)
    {
        Player cell_player;
        switch (line[i])
        {
        case 'x':
        case 'X':
            cell_player = X;
            break;
        case 'o':
        case 'O':
            cell_player = O;
            break;
        case '_':
        case ' ':
        default:
            cell_player = E;
        }

        game.set_board_value(0, i, cell_player);
    }

    GomokuPatternReconizer reconizer(O);

    reconizer.find_patterns_in_board(game);

    const Matrix<PatternCellData> &line_mat = reconizer.get_pattern_cell_matrix(PatternDirection::LeftToRight);

    for (int col = 0; col < line_mat.get_width(); ++col)
    {
        PatternCellData cell_data = line_mat(1, col);

        std::vector<int> all_structures(StructureType::COUNT_STRUCTURE_TYPE, 0);
        cell_data.get_structures_type_count(all_structures);

        char cell_state = (col > 0 && col <= int(line.size())) ? line[col - 1] : 'X';

        auto cell_pattern = reconizer.get_structure_at(GomokuCellIndex(0, col - 1), PatternDirection::LeftToRight);
        StructureType has_struct[3];
        for (int i = 0; i < 3; i++)
            has_struct[i] = reconizer.highest_structure_around(GomokuCellIndex(0, col - 1), i);

        std::cout << cell_state << " -> " << cell_data << " " << all_structures << " "
                  << "structat=" << cell_pattern.first << "[" << int(cell_pattern.second.col) << "]"
                  << "\t" << has_struct[0] << " " << has_struct[1] << " " << has_struct[2]
                  << std::endl;
    }
}

void test_board(const std::string &line)
{
    GomokuGame game(10, 10);

    std::vector<std::string> moves = split(line, ',');
    apply_moves(game, moves);

    // Display the board
    std::cout << to_string(game, true, -1);
    std::cout << to_string(game, true, -2);
    auto bounds = game.get_played_bounds();
    std::cout << "Played bounds:"
              << "row[" << int(bounds.first.row) << "-" << int(bounds.second.row) << "]"
              << "col[" << int(bounds.first.col) << "-" << int(bounds.second.col) << "]"
              << std::endl;

    // Display patterns
    game.print_patterns();
}

void clearLastNLines(int n)
{
    for (int i = 0; i < n; ++i)
    {
        // Move cursor up one line
        std::cout << "\033[A";
        // Clear the line
        std::cout << "\033[2K";
    }
    // Move cursor to the beginning of the last cleared line
    // std::cout << "\033[" << n << "A";
}

void fight(std::string ai_name1, std::string ai_name2)
{
    GameRoomSettings settings;
    settings.p1.ai_name = ai_name1;
    settings.p2.ai_name = ai_name2;
    settings.p1.is_ai = true;
    settings.p2.is_ai = true;

    GameRoom room(settings);
    bool isFirstMove = true;
    while (room.has_pending_action())
    {
        room.perform_pending_action();
        if (!isFirstMove)
            clearLastNLines(20);
        std::cout << to_string(room.get_game(), true, -1);
        isFirstMove = false;
    }

    std::cout << "Game over" << std::endl;
    std::cout << "Winner: " << room.get_game().get_winner() << std::endl;
    Timer::printAccumulatedTimes();
}

int main(int argc, char *argv[])
{
    // If no arguments are given, run the test_problems function
    // If an argument is given, run the test_problem function with the given argument
    if (argc == 1)
    {
        test_problems();
        return 0;
    }

    std::string arg1(argv[1]);

    if (arg1 == "eval")
    {
        std::string moves_string = std::string(argv[2]);
        test_eval(moves_string);
    }
    else if (arg1 == "board")
    {
        std::string line = std::string(argv[2]);
        test_board(line);
    }
    else if (arg1 == "line")
    {
        if (argc == 2)
            return -1;
        test_line(std::string(argv[2]));
    }
    else if (arg1 == "arena")
    {
        Arena().play(argc, argv);
    }
    else if (arg1 == "fight")
    {
        fight(argv[2], argv[3]);
    }
    else
    {
        test_problem(atoi(argv[1]));
    }
    return 0;
}
