#include "AI1.h"
#include <algorithm>

GomokuAI::GomokuAI(GomokuGame game, Player ai_player, int depth) : game(game), depth(depth), ai_player(ai_player), move_count(0), move_evaluated_count(0)
{
    human_player = (ai_player == X) ? O : X;
}

void sortMovesUtil(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer, int depth)
{
    TIMER
    auto compare = [maximizingPlayer](const std::pair<std::pair<int, int>, int> &a, const std::pair<std::pair<int, int>, int> &b)
    {
        return maximizingPlayer ? a.second > b.second : a.second < b.second;
    };
    std::sort(moves.begin(), moves.end(), compare);
}

void GomokuAI::sortMoves(std::vector<std::pair<std::pair<int, int>, int>> &moves, bool maximizingPlayer, int depth)
{
    TIMER
    for (std::pair<std::pair<int, int>, int> &move : moves)
    {
        try
        {
            MoveResult game_move = game.make_move(move.first.first, move.first.second);
            move.second = heuristic_evaluation();
            game.reverse_move(game_move);
        }
        catch (std::exception &e)
        {
            move.second = maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1;
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
        node.score = game.is_game_over() ? (maximizingPlayer ? std::numeric_limits<int>::min() + 1 : std::numeric_limits<int>::max() - 1) : heuristic_evaluation();
        return node;
    }

    // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
    std::vector<std::pair<std::pair<int, int>, int>> moves = game.findRelevantMoves();
    if (depth > 1)
        sortMoves(moves, maximizingPlayer, depth);
    int moveIdx = 1;
    node.totalEvalCount = moves.size();
    node.evaluatedMoves = moves.size();

    // For each move, make the move, call minimax recursively and reverse the move.
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (auto moveWithScore : moves)
    {
        std::pair<int, int> move = moveWithScore.first;
        try
        {
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
