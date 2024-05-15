
#include "utils/gomoku_utilities.h"
#include <sstream>
#include <iostream>
#include <fstream>

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
std::string to_string(const GomokuGame &game, bool with_coordinates, int distance)
{
    std::stringstream ss;

    auto print_at = [&ss, &game, distance](int row, int col)
    {
        Player value = game.get_board_value(row, col);
        if (value == E && distance >= 0)
        {
            StructureType black_struct = game.get_pattern_reconizer(X)
                                             .highest_structure_around(GomokuCellIndex(row, col), distance);
            StructureType white_struct = game.get_pattern_reconizer(O)
                                             .highest_structure_around(GomokuCellIndex(row, col), distance);
            if (black_struct > StructureType::NONE || white_struct > StructureType::NONE)
                ss << "*";
            else
                ss << ".";
        }
        else
        {
            ss << value;
        }
    };

    if (with_coordinates)
    {
        ss << " ";
        for (int i = 0; i < game.get_board_width(); i++)
            ss << " " << coordinate_to_char(i);
        ss << std::endl;

        for (int row = 0; row < game.get_board_height(); ++row)
        {
            ss << coordinate_to_char(row) << " ";
            for (int col = 0; col < game.get_board_width(); ++col)
            {
                print_at(row, col);
                if (col < game.get_board_width() - 1)
                    ss << ' ';
            }
            ss << std::endl;
        }
    }
    else
    {
        for (int row = 0; row < game.get_board_height(); row++)
        {
            for (int col = 0; col < game.get_board_width(); col++)
            {
                print_at(row, col);
            }
            ss << std::endl;
        }
    }

    return ss.str();
}

void apply_moves(GomokuGame &game, std::vector<std::string> moves)
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

int get_depth_from_env(int default_depth)
{
    char *env_depth = getenv("DEPTH");

    if (env_depth)
    {
        int value = atoi(env_depth);
        if (value != 0)
            return value;
    }
    return default_depth;
}

void writeMoveEvaluation(std::ostream &out, const MoveEvaluation &eval, int depth, int i)
{
    // Create indentation based on the depth
    std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

    out << indent;
    if (i >= 0)
    {
        out << i << ": ";
    }
    out << "Move: " << coordinate_to_char(eval.move.first) << coordinate_to_char(eval.move.second) << " | " << eval.score << "\n";

#ifndef NDEBUG
    if (depth < get_depth_from_env())
    {
        out << indent << "Evals: " << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << " | initial score: " << eval.initialScore << "\n";
    }
#endif

    if (not eval.listMoves.empty())
    {
        out << indent << "ListMoves:\n";

        for (int i = 0; i < eval.listMoves.size(); i++)
        {
            const auto &move = eval.listMoves[i];
            // Increase the indentation for each level
            writeMoveEvaluation(out, move, depth + 1, i);
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

void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth)
{
    if (depth == get_depth_from_env())
    {
        return;
    }

#ifndef NDEBUG
    if (eval.neededEvalCount != eval.evaluatedMoves)
    {
        out << eval.neededEvalCount << " / " << eval.evaluatedMoves << " / " << eval.totalEvalCount << "\n";
    }
#endif

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

int getBestMoveIndex(const MoveEvaluation &eval, bool maximizingPlayer)
{
    int bestScore = std::numeric_limits<int>::min();
    int worstScore = std::numeric_limits<int>::max();
    int bestIndex = -1;

    for (size_t i = 0; i < eval.listMoves.size(); i++)
    {
        const auto &move = eval.listMoves[i];
        if (maximizingPlayer)
        {
            if (move.score > bestScore || bestIndex == -1)
            {
                bestScore = move.score;
                bestIndex = static_cast<int>(i);
            }
        }
        else
        {
            if (move.score < worstScore || bestIndex == -1)
            {
                worstScore = move.score;
                bestIndex = static_cast<int>(i);
            }
        }
    }

    return bestIndex;
}

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer)
{
    int bestIndex = getBestMoveIndex(eval, maximizingPlayer);
    if (bestIndex == -1)
        return std::make_pair(-1, -1);
    return eval.listMoves[bestIndex].move;
}

std::ostream &operator<<(std::ostream &stream, std::vector<std::string> moves)
{
    for (const std::string &move : moves)
    {
        stream << move << ' ';
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, std::vector<Player> structure)
{
    for (const Player player : structure)
    {
        stream << player;
    }
    return stream;
}
