#include "utils.h"

std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth)
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

void writeSurplusEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth)
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