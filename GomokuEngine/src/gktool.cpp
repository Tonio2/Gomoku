
#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai.h"
#include "arena/arena.h"
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
        GomokuAI AI(get_depth_from_env());
        // Suggest a move
        MoveEvaluation moveEvalutation = AI.suggest_move(game);
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

    GomokuAI AI(get_depth_from_env());
    // Suggest a move
    MoveEvaluation moveEvalutation = AI.suggest_move(game);
    // Get the best move
    std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    // Print the best move
    std::cout << to_string(game, true);
    std::cout << "Best move for player " << player << ": " << coordinate_to_char(bestMove.first) << coordinate_to_char(bestMove.second) << std::endl;
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

    Player last_player = game.get_current_player();
    GomokuAI AI(get_depth_from_env());
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
    std::cout << "Next move to: " << game.other_player(last_player) << std::endl;

    // Display the evaluation
    std::cout << "Board evaluation for " << last_player << ": " << evaluation << std::endl;

    // Display the suggested move
    std::vector<MoveHeuristic> relevant_moves = AI.get_relevant_moves(game);
    std::cout << "Relevant moves (" << relevant_moves.size() << "): [";
    for (MoveHeuristic move : relevant_moves)
    {
        std::cout << "(" << int(move.row) << "," << int(move.col) << "),";
    }
    std::cout << "]" << std::endl;

    MoveEvaluation moveEvaluation = AI.suggest_move(game);
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
        bool has_struct[3];
        for (int i = 0; i < 3; i++)
            has_struct[i] = reconizer.has_structure_around(GomokuCellIndex(0, col - 1), i);

        std::cout << cell_state << " -> " << cell_data << " " << all_structures << " "
                  << "structat=" << cell_pattern.first << "[" << int(cell_pattern.second.col) << "]"
                  << "\t" << has_struct[0] << " " << has_struct[1] << " " << has_struct[2]
                  << std::endl;
    }
}

std::vector<std::vector<Player>> generate_flanked_structures(std::vector<Player> s, StructureType type)
{
    std::vector<std::vector<Player>> ret = {};
    std::vector<Player> flanks = {X, E, O};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            bool push = true;
            std::vector<Player> flanked_struct = {flanks[i]};
            flanked_struct.insert(flanked_struct.end(), s.begin(), s.end());
            flanked_struct.push_back(flanks[j]);

            if (type == StructureType::TWO || type == StructureType::OPEN_TWO)
            {
                if ((flanks[i] == O && s[0] == E) || (flanks[j] == O && s[s.size() - 1] == E))
                {
                    push = false;
                }
            }

            if (push)
                ret.push_back(flanked_struct);
        }
    }

    return ret;
}

bool test_struct(std::vector<Player> s, StructureType expected_type)
{
    GomokuGame game(s.size(), 1);

    for (int i = 0; i < s.size(); i++)
        game.set_board_value(0, i, s[i]);

    GomokuPatternReconizer reconizer(O);

    reconizer.find_patterns_in_board(game);

    GomokuCellIndex idx(0, 2);

    StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

    if (type != expected_type)
    {
        std::cout << "Found " << type << " but expected " << expected_type;
        return false;
    }

    return true;
}

bool test_walled_struct(std::vector<Player> s, StructureType expected_type)
{
    GomokuGame game(s.size(), 1);

    for (int i = 0; i < s.size(); i++)
        game.set_board_value(0, i, s[i]);

    GomokuPatternReconizer reconizer(O);

    reconizer.find_patterns_in_board(game);

    GomokuCellIndex idx(0, 1);

    StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

    if (type != expected_type)
    {
        std::cout << "Found " << type << " but expected " << expected_type;
        return false;
    }

    return true;
}

void test_get_structure_at()
{

    std::vector<std::pair<std::vector<Player>, StructureType>> structures = {
        {{E, O, X}, StructureType::ONE},
        {{X, O, E}, StructureType::ONE},
        {{E, O, E}, StructureType::OPEN_ONE},
        {{E, O, O, X}, StructureType::TWO},
        {{X, O, O, E}, StructureType::TWO},
        {{E, O, O, E}, StructureType::OPEN_TWO},
        {{E, O, O, O, X}, StructureType::THREE},
        {{X, O, O, O, E}, StructureType::THREE},
        {{E, O, O, O, E}, StructureType::OPEN_THREE},
        {{E, O, E, O, O, X}, StructureType::THREE},
        {{X, O, E, O, O, E}, StructureType::THREE},
        {{E, O, E, O, O, E}, StructureType::OPEN_THREE},
        {{E, O, O, E, O, X}, StructureType::THREE},
        {{X, O, O, E, O, E}, StructureType::THREE},
        {{E, O, O, E, O, E}, StructureType::OPEN_THREE},
        {{E, O, O, O, O, X}, StructureType::FOUR},
        {{X, O, O, O, O, E}, StructureType::FOUR},
        {{E, O, O, O, O, E}, StructureType::OPEN_FOUR},
        {{E, O, O, O, O, O, X}, StructureType::FIVE_OR_MORE},
        {{X, O, O, O, O, O, E}, StructureType::FIVE_OR_MORE},
        {{E, O, O, O, O, O, E}, StructureType::FIVE_OR_MORE},
    };

    for (int i = 0; i < structures.size(); i++)
    {
        std::cout << "TEMPLATE : " << structures[i].first << " " << structures[i].second << std::endl;
        std::vector<std::vector<Player>> flanked_structures = generate_flanked_structures(structures[i].first, structures[i].second);
        for (std::vector<Player> flanked_structure : flanked_structures)
        {
            std::cout << flanked_structure << " : ";
            if (test_struct(flanked_structure, structures[i].second))
                std::cout << "OK";
            std::cout << std::endl;
        }
        std::cout << "|" << structures[i].first << "|" << " : ";
        if (test_walled_struct(structures[i].first, structures[i].second))
            std::cout << "OK";
        std::cout << std::endl;
    }
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
    else if (arg1 == "get_structure_at")
    {
        test_get_structure_at();
    }
    else
    {
        test_problem(atoi(argv[1]));
    }
    return 0;
}
