#include "AI1.h"
#include <algorithm>

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), depth(depth), ai_player(ai_player), move_count(0), move_evaluated_count(0)
{
    human_player = (ai_player == X) ? O : X;
}

void sortMovesUtil(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth)
{
    TIMER

    std::function<bool(const MoveHeuristic &, const MoveHeuristic &)> compare;

    if (maximizingPlayer)
    {
        compare = [](const MoveHeuristic &a, const MoveHeuristic &b)
        {
            return a.score > b.score;
        };
    }
    else
    {
        compare = [](const MoveHeuristic &a, const MoveHeuristic &b)
        {
            return a.score < b.score;
        };
    }
    std::sort(moves.begin(), moves.end(), compare);
}

void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, int depth)
{
    TIMER
    for (MoveHeuristic &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.row, move.col);
            move.score = heuristic_evaluation();
            game.reverse_move(game_move);
        }
        catch (std::exception &e)
        {
            move.score = maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1;
        }
    }

    sortMovesUtil(moves, maximizingPlayer, depth);
}

MoveEvaluation GomokuAI::minimax(int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    TIMER

    MoveEvaluation node = {{row, col}, 0, 0, 0, {}};

    if (depth == 0 || game.is_game_over())
    {
        if (game.is_game_over())
        {
            if (game.get_winner() == ai_player)
                node.score = std::numeric_limits<int>::max() - 1;
            else if (game.get_winner() == human_player)
                node.score = std::numeric_limits<int>::min() + 1;
            else
                node.score = 0;
        }
        else
            node.score = heuristic_evaluation();
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<MoveHeuristic> moves = find_relevant_moves();
    if (depth > 1)
        sortMoves(moves, maximizingPlayer, depth);
    int moveIdx = 1;
    node.totalEvalCount = moves.size();
    node.evaluatedMoves = moves.size();

    // For each move, make the move, call minimax recursively and reverse the move.
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (const MoveHeuristic &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.row, move.col);
            MoveEvaluation evalNode = minimax(depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
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
        catch (std::exception &e)
        {
            move_count--;
        }
    }
    move_count += moves.size();
    move_evaluated_count += node.evaluatedMoves;
    evaluation_needed_count += node.neededEvalCount;
    return node;
}

int GomokuAI::heuristic_evaluation()
{
    TIMER
    Player player = ai_player;
    int multiplier = 1;
    int score = 0;
    std::vector<int> capture_scores = {0, 100, 200, 300, 1000};

    for (int i = 0; i < 2; i++)
    {
        const std::vector<int> &player_patterns_count = game.get_patterns_count(player);
        score += player_patterns_count[FIVE_OR_MORE] * 100000 * multiplier;
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
    return 0;
}

MoveEvaluation GomokuAI::suggest_move()
{
    TIMER
#ifdef NOTIMER
    Timer timer("suggest_move");
#endif
    move_count = 0;
    move_evaluated_count = 0;
    evaluation_needed_count = 0;
    MoveEvaluation result = minimax(depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    return result;
}

std::vector<MoveHeuristic> GomokuAI::find_relevant_moves() const
{
    TIMER
    std::vector<MoveHeuristic> relevantMoves;

    // Directions to check around each cell (8 directions).
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int row = 0; row < game.get_board_height(); ++row)
    {
        for (int col = 0; col < game.get_board_width(); ++col)
        {
            if (game.get_board_value(row, col) == E)
            { // Empty cell
                bool foundStone = false;
                for (const auto &dir : directions)
                {
                    for (int step = 1; step <= 2; ++step)
                    {
                        int newRow = row + step * dir.first;
                        int newCol = col + step * dir.second;

                        if (game.coordinates_are_valid(newRow, newCol) && game.get_board_value(newRow, newCol) != E)
                        {
                            relevantMoves.push_back(
                                MoveHeuristic{uint8_t(row), uint8_t(col), 0});
                            foundStone = true;
                            break; // No need to check further if one stone is found near this cell.
                        }
                    }
                    if (foundStone)
                    {
                        break; // No need to check further if one stone is found near this cell.
                    }
                }
            }
        }
    }

    return relevantMoves;
}
