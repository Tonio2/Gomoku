#include <iostream>
#include <fstream>
#include <utility>

#include "AI1.h"

std::vector<char> boardCoordinates = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

void displayBoard(GomokuGame &game)
{
    std::cout << " ";
    for (char coordinate : boardCoordinates)
    {
        std::cout << " " << coordinate;
    }
    std::cout << std::endl;
    for (int i = 0; i < game.get_board_size(); i++)
    {
        std::cout << boardCoordinates[i] << " ";
        for (int j = 0; j < game.get_board_size(); j++)
        {
            if (game.get_board_value(i, j) == X)
            {
                std::cout << "X ";
            }
            else if (game.get_board_value(i, j) == O)
            {
                std::cout << "O ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}

void writeMoveEvaluation(std::ofstream &out, const MoveEvaluation &eval, int depth = 0)
{
    // Create indentation based on the depth
    std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

    out << indent << "Move: (" << eval.move.first << ", " << eval.move.second << ")\n";
    out << indent << "Score: " << eval.score << "\n";
    if (not eval.listMoves.empty())
    {
        out << indent << "ListMoves:\n";

        for (const auto &move : eval.listMoves)
        {
            // Increase the indentation for each level
            writeMoveEvaluation(out, move, depth + 1);
        }
    } else {
        out << indent << "-----------\n";
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

#include "gomoku_structure_pattern.h"

int main()
{
    std::cout << "Hello, World!" << std::endl;
    GomokuGame game(19);
    game.make_move(4, 4);
    game.make_move(2, 2);
    game.make_move(5, 4);
    // game.update_structures(O);
    // game.make_move(0, 0);
    // game.make_move(0, 1);
    game.display_struct();

    GomokuPatternReconizer reconizer(Player::BLACK);
    reconizer.find_patterns_in_board(game);
    reconizer.print_patterns();
    // GomokuAI AI(game, O, 3);
    // MoveEvaluation moveEvalutation = AI.suggest_move();
    // logMoveEvaluation(moveEvalutation);
    // displayBoard(game);
    // std::pair<int, int> bestMove = getBestMove(moveEvalutation, true);
    // std::cout << "Best move: (" << bestMove.first << ", " << bestMove.second << ")" << std::endl;
    Timer::printAccumulatedTimes();
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
