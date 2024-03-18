#include "AI1.h"
#include <algorithm>

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), depth(depth), ai_player(ai_player)
{
    human_player = (ai_player == X) ? O : X;
}

void GomokuAI::sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer, int depth)
{
    for (std::pair<std::pair<int, int>, int> &move : moves)
    {
        if (depth > 1)
        {
            MoveResult game_move = game.make_move(move.first.first, move.first.second);
            move.second = heuristic_evaluation();
            game.reverse_move(game_move);
        }
        // else
        // {
        //     move.second = pseudo_heuristic_evaluation(move.first);
        // }
    }
    if (maximizingPlayer)
        std::sort(moves.begin(), moves.end(), [](const std::pair<std::pair<int, int>, int> &a, const std::pair<std::pair<int, int>, int> &b)
                  { return a.second > b.second; });
    else
        std::sort(moves.begin(), moves.end(), [](const std::pair<std::pair<int, int>, int> &a, const std::pair<std::pair<int, int>, int> &b)
                  { return a.second < b.second; });
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    // If the depth is 0 or the game is over, return the heuristic evaluation of the current board.
    MoveEvaluation node;
    node.move = {row, col}; // Initialize with an invalid move.
    if (depth == 0 || game.is_game_over())
    {
        node.score = game.is_game_over() ? (maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1) : heuristic_evaluation();
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<std::pair<std::pair<int, int>, int>> moves = game.findRelevantMoves();
    node.totalEvalCount = moves.size();
    node.evaluatedMoves = moves.size();
    int moveIdx = 1;
    sortMoves(moves, maximizingPlayer, depth);

    // if (depth == 2)
    // {
    //     std::cout << row << " " << col << std::endl;
    // }

    // For each move, make the move, call minimax recursively and reverse the move.
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (auto moveWithScore : moves)
    {
        std::pair<int, int> move = moveWithScore.first;
        MoveResult game_move = game.make_move(move.first, move.second);
        MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, !maximizingPlayer, move.first, move.second);
        game.reverse_move(game_move);

        if (maximizingPlayer)
        {
            if (evalNode.score > extremeEval)
            {
                extremeEval = evalNode.score;
                node.score = extremeEval;
                alpha = std::max(alpha, evalNode.score);
                node.neededEvalCount = moveIdx;
            }
        }
        else
        {
            if (evalNode.score < extremeEval)
            {
                extremeEval = evalNode.score;
                node.score = extremeEval;
                beta = std::min(beta, evalNode.score);
                node.neededEvalCount = moveIdx;
            }
        }

        node.listMoves.push_back(evalNode);

        if (beta <= alpha)
        {
            node.evaluatedMoves = moveIdx;
            break;
        }
        moveIdx++;
    }
    return node;
}

int GomokuAI::heuristic_evaluation()
{
    Player player = ai_player;
    std::vector<std::vector<int>> patterns_count = game.get_patterns_count();
    int multiplier = 1;
    int score = 0;
    std::vector<int> capture_scores = {0, 100, 200, 300, 1000};

    for (int i = 0; i < 2; i++)
    {
        std::vector<int> player_patterns_count = patterns_count[player];
        score += player_patterns_count[OPEN_FOUR] * 10000 * multiplier;
        score += (player_patterns_count[OPEN_THREE] >= 2 or player_patterns_count[FOUR] >= 2 or (player_patterns_count[OPEN_THREE] >= 1 and player_patterns_count[FOUR] >= 1)) * 9000 * multiplier;
        score += player_patterns_count[FOUR] * 1000 * multiplier;
        score += player_patterns_count[OPEN_THREE] * 1000 * multiplier;
        score += player_patterns_count[THREE] * 500 * multiplier;
        score += player_patterns_count[OPEN_TWO] * 100 * multiplier;
        score += player_patterns_count[TWO] * 50 * multiplier;
        score += player_patterns_count[OPEN_ONE] * 10 * multiplier;
        score += player_patterns_count[ONE] * 5 * multiplier;

        int score_capture = game.get_player_score(player);
        score += capture_scores[score_capture / 2] * multiplier;
        player = human_player;
        multiplier = -1;
    }
    return score;
}

int GomokuAI::pseudo_heuristic_evaluation(std::pair<int, int> move)
{
    std::vector<std::vector<Structure>> structuresAll = game.get_structures();
    int score = 0;

    // Define the scores for each structure type for both the AI and the opponent.
    std::map<StructureType, int> structureScores = {
        {OPEN_FOUR, 100},
        {FOUR, 50},
        {OPEN_THREE, 25},
        {THREE, 10},
        {OPEN_TWO, 5},
        {TWO, 2},
        {OPEN_ONE, 1},
        {ONE, 0}};

    // Iterate through all structures to check their proximity to the move
    for (std::vector<Structure> &playerStructures : structuresAll)
    {
        for (auto &structure : playerStructures)
        {
            for (auto &cell : structure.cells)
            {
                if (game.get_board_value(cell.first, cell.second) == E)
                {
                    if (move.first == cell.first and move.second == cell.second)
                    {
                        score += structureScores[structure.type];
                    }
                }
            }
        }
    }
    return score;
}

MoveEvaluation GomokuAI::suggest_move()
{
    Timer timer("suggest_move");
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}
