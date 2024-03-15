#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>

#include "gomoku_engine.h"

std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

int get_coordinate_index(char c)
{
    for (int i = 0; i < boardCoordinates.size(); i++)
    {
        if (c == boardCoordinates[i])
        {
            return i;
        }
    }
    return -1;
}

int main()
{

    GomokuGame game(19);
    std::vector<std::pair<int, int>> moves;
    // Infinite loop to ask a move, make the move and print the board until player type exit
    while (true)
    {
        std::string input;
        bool valid_move = false;
        game.display_board();
        while (!valid_move)
        {
            std::cout << "Enter your move (e.g. A1): ";
            std::cin >> input;
            if (input == "exit")
            {
                break;
            }
            if (input.length() != 2)
            {
                std::cout << "Invalid input. Please enter a valid move (e.g. A1)" << std::endl;
                continue;
            }
            int row = get_coordinate_index(input[0]);
            int col = get_coordinate_index(input[1]);
            if (row == -1 or col == -1)
            {
                std::cout << "Invalid input. Please enter a valid move (e.g. A1)" << std::endl;
                continue;
            }
            try {
                game.make_move(row, col);
                valid_move = true;
                moves.push_back({row, col});
            } catch (std::runtime_error &e) {
                std::cout << e.what() << std::endl;
            }
        }
        
        if (input == "exit")
        {
            break;
        }
    }
    // Display moves
    std::cout << "Moves: ";
    for (auto move : moves)
    {
        std::cout << boardCoordinates[move.first] << boardCoordinates[move.second] << ",";
    }
    return 0;
}