
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
