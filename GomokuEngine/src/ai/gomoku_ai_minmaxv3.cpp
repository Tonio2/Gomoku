#include "ai/gomoku_ai_minmaxv3.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>
#include <chrono>
#include "utils/gomoku_utilities.h"

namespace AI::MinMaxV3
{

Move GomokuAI::suggest_move(const GomokuGame &board, int currentMove)
{
    auto time = std::chrono::system_clock::now().time_since_epoch();
    MoveEvaluation result = suggest_move_evaluation(board);
    auto time2 = std::chrono::system_clock::now().time_since_epoch();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time).count();
#ifdef LOGGING
    std::string filename = "/tmp/evals/" + std::to_string(currentMove) + ".txt";
    logMoveEvaluation(result, filename, duration);
#endif
    auto [row, col] = getBestMove(result);
    return Move(row, col);
}

std::vector<Move> GomokuAI::suggest_move_sequence(const GomokuGame &board)
{
    std::vector<Move> moves;
    MoveEvaluation result = suggest_move_evaluation(board);
#ifdef LOGGING
    std::string filename = "/tmp/evals/help.txt";
    logMoveEvaluation(result, filename);
#endif
    bool maximizing = true;
    MoveEvaluation *currentMove = &result;
    while (currentMove->listMoves.size() > 0)
    {
        int bestIndex = getBestMoveIndex(*currentMove, maximizing);
        currentMove = &currentMove->listMoves[bestIndex];
        moves.push_back(Move(currentMove->move.first, currentMove->move.second));
        maximizing = !maximizing;
    }

    return moves;
}

GomokuAI::GomokuAI(const GomokuAiSettings &settings)
    : game(0, 0), depth(settings.depth), length(settings.length), evaluation_data(settings.data)
{
    killer_moves = std::vector<std::pair<int, int>>(depth, {-1, -1});
}

void sortMovesUtil(std::vector<MoveHeuristic> &moves, bool maximizingPlayer)
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

void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, MoveEvaluation &eval)
{
    TIMER
    for (auto it = moves.begin(); it != moves.end();) // no icrement here
    {
        try
        {
            MoveResult game_move = game.make_move(it->row, it->col);
            if (game.is_game_over())
            {
                if (game.get_winner() == ai_player)
                    it->score = std::numeric_limits<int>::max();
                else if (game.get_winner() == human_player)
                    it->score = std::numeric_limits<int>::min();
                else
                    it->score = 0;
            }
            else
                it->score = _heuristic_evaluation();
            game.reverse_move(game_move);
            ++it; // Only increment here if the move was successful
        }
        catch (std::exception &e)
        {
            it = moves.erase(it); // erase returns the next iterator
#ifdef LOGGING
            eval.totalEvalCount--;
#endif
        }
    }

    sortMovesUtil(moves, maximizingPlayer);
}

void GomokuAI::evaluateNode(const MoveHeuristic &move, int moveId, int _depth, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremeEval, std::pair<int, int> &best_move, bool isFirstMove)
{
    MoveResult game_move = game.make_move(move.row, move.col);
    eval.listMoves.push_back(MoveEvaluation());
    MoveEvaluation &evalNode = eval.listMoves.back();
    evalNode.initialScore = move.score;
    minimax(evalNode, _depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
    game.reverse_move(game_move);
    
    if (maximizingPlayer)
    {
        if (evalNode.score > extremeEval || isFirstMove)
        {
            extremeEval = evalNode.score;
            eval.score = extremeEval;
            alpha = std::max(alpha, evalNode.score);
            best_move = {move.row, move.col};
#ifdef LOGGING
            eval.neededEvalCount = moveId + 1 + eval.killerMoveHasBeenEvaluated;
#endif
        }
    }
    else
    {
        if (evalNode.score < extremeEval || isFirstMove)
        {
            extremeEval = evalNode.score;
            eval.score = extremeEval;
            beta = std::min(beta, evalNode.score);
            best_move = {move.row, move.col};
#ifdef LOGGING
            eval.neededEvalCount = moveId + 1 + eval.killerMoveHasBeenEvaluated;
#endif
        }
    }
}

void GomokuAI::minimax(MoveEvaluation &eval, int _depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
{
    TIMER

    eval.move.first = row;
    eval.move.second = col;

    if (_depth == 0 || game.is_game_over())
    {
        if (game.is_game_over())
        {
            if (game.get_winner() == ai_player)
                eval.score = std::numeric_limits<int>::max();
            else if (game.get_winner() == human_player)
                eval.score = std::numeric_limits<int>::min();
            else
                eval.score = 0;
        }
        else
            eval.score = _heuristic_evaluation();
        return;
    }

    std::vector<MoveHeuristic> moves;
    find_relevant_moves(moves, _depth);
#ifdef LOGGING
    eval.totalEvalCount = moves.size();
#endif

    bool isFirstMove = true;
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    std::pair<int, int> best_move = {-1, -1};

    const std::pair<int, int> &killer_move = killer_moves[depth - _depth];
    if (killer_move.first == moves[0].row && killer_move.second == moves[0].col)
    {
        try
        {
            evaluateNode(moves[0], 0, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, best_move, isFirstMove);

            if (maximizingPlayer)
            {
                if (eval.listMoves[0].score >= eval.initialScore) return;
            }

            if (beta <= alpha)
            {
                return;
            }
            isFirstMove = false;
        }
        catch (std::exception &e)
        {
        }
#ifdef LOGGING
        eval.killerMoveHasBeenEvaluated = 1;
#endif
        moves.erase(moves.begin());
    }

    if (_depth > 1)
        sortMoves(moves, maximizingPlayer, eval);

    int moveId = 0;
    while (moveId < moves.size())
    {
        try
        {
            evaluateNode(moves[moveId], moveId, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, best_move, isFirstMove);

            if (maximizingPlayer)
            {
                if (eval.listMoves.back().score >= eval.initialScore) break;
            }

            if (beta <= alpha)
            {
                break;
            }
            isFirstMove = false;
        }
        catch (std::exception &e)
        {
        }
        moveId++;
    }
#ifdef LOGGING
    eval.evaluatedEvalCount = std::min((int)moves.size(), moveId + 1) + eval.killerMoveHasBeenEvaluated;
#endif

    killer_moves[depth - _depth] = {best_move.first, best_move.second};
}

int GomokuAI::_heuristic_evaluation()
{
    TIMER
    return score_player(ai_player) - score_player(human_player);
}

int GomokuAI::score_player(Player player)
{
    int score = 0;
    const std::vector<int> &patterns_count = game.get_patterns_count(player);

    for (int i = 0; i < StructureType::COUNT_STRUCTURE_TYPE; i++)
    {
        score += patterns_count[i] * evaluation_data.value_of_structure(i);
    }
    score += (patterns_count[OPEN_THREE] + patterns_count[FOUR] + patterns_count[OPEN_FOUR] >= 2) ? evaluation_data.value_of_multiple_forced() : 0;
    score += (patterns_count[OPEN_FOUR] >= 2 ? evaluation_data.value_of_multiple_o4() : 0);
    score += evaluation_data.value_of_captures(game.get_player_score(player));
    return score;
}

int GomokuAI::get_heuristic_evaluation(const GomokuGame &board, Player player)
{
    TIMER

    game = board;
    ai_player = player;
    human_player = board.other_player(player);
    return _heuristic_evaluation();
}

MoveEvaluation GomokuAI::suggest_move_evaluation(const GomokuGame &board)
{
    Timer timer(__FUNCTION__);

    game = board;
    ai_player = board.get_current_player();
    human_player = board.other_player(ai_player);

    MoveEvaluation result;
    result.initialScore = _heuristic_evaluation();
    if (board.has_player_bounds())
        minimax(result, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
    else
        result.listMoves.push_back(MoveEvaluation{{game.get_board_height() / 2, game.get_board_width() / 2}, 1});
    return result;
}

static const std::vector<std::pair<int, int>> _directions_offsets = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

void GomokuAI::find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves, int _depth) const
{
    TIMER

    auto [min, max] = game.get_played_bounds(length);

    // Pre-calculate the relevant cells
    std::vector<std::vector<bool>> relevant_cells(max.row - min.row + 1, std::vector<bool>(max.col - min.col + 1, false));
    
    for (int row = min.row; row <= max.row; ++row)
    {
        for (int col = min.col; col <= max.col; ++col)
        {
            if (game.get_board_value(row, col) != E)
            {
                for (const auto &dir : _directions_offsets)
                {
                    for (int step = 1; step <= length; ++step)
                    {
                        int newRow = row + step * dir.first;
                        int newCol = col + step * dir.second;

                        if (game.coordinates_are_valid(newRow, newCol) && game.get_board_value(newRow, newCol) == E)
                        {
                            int relRow = newRow - min.row;
                            int relCol = newCol - min.col;
                            relevant_cells[relRow][relCol] = true;
                        }
                    }
                }
            }
        }
    }

    for (int row = min.row; row <= max.row; ++row)
    {
        for (int col = min.col; col <= max.col; ++col)
        {
            int relRow = row - min.row;
            int relCol = col - min.col;
            if (!relevant_cells[relRow][relCol])
                continue;

            if (killer_moves[depth - _depth] == std::pair<int, int>(row, col))
                out_relevant_moves.emplace(out_relevant_moves.begin(), MoveHeuristic{uint8_t(row), uint8_t(col), 0});
            else
                out_relevant_moves.emplace_back(MoveHeuristic{uint8_t(row), uint8_t(col), 0});
        }
    }
}



bool GomokuAI::is_cell_relevant(int row, int col) const
{
    for (const auto &dir : _directions_offsets)
    {
        for (int step = 1; step <= length; ++step)
        {
            int newRow = row + step * dir.first;
            int newCol = col + step * dir.second;

            if (game.coordinates_are_valid(newRow, newCol) && game.get_board_value(newRow, newCol) != E)
            {
                return true;
            }
        }
    }

    return false;
}

std::vector<MoveHeuristic> GomokuAI::get_relevant_moves(const GomokuGame &board)
{
    game = board;
    ai_player = board.get_current_player();
    human_player = board.other_player(ai_player);

    std::vector<MoveHeuristic> moves;
    find_relevant_moves(moves, depth);
    return moves;
}

const GomokuAIData &GomokuAI::get_evaluation_data() const
{
    return evaluation_data;
}

} // namespace AI::MinMaxV2
