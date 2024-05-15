#include "ai/gomoku_ai_minmaxv2.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

#include "utils/gomoku_utilities.h"

namespace AI::MinMaxV2
{

    Move GomokuAI::suggest_move(const GomokuGame &board)
    {
        MoveEvaluation result = suggest_move_evaluation(board);
        auto [row, col] = getBestMove(result);
        return Move(row, col);
    }

    std::vector<Move> GomokuAI::suggest_move_sequence(const GomokuGame &board)
    {
        std::vector<Move> moves;
        MoveEvaluation result = suggest_move_evaluation(board);
        bool maximizing = true;
        while (result.listMoves.size() > 0)
        {
            result = getBestMoveEvaluation(result, maximizing);
            moves.push_back(Move(result.move.first, result.move.second));
            maximizing = !maximizing;
        }

        return moves;
    }

    GomokuAI::GomokuAI(const GomokuAiSettings &settings)
        : game(0, 0), depth(settings.depth), length(settings.length), evaluation_data(settings.data)
    {
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

    void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer)
    {
        TIMER
        for (auto it = moves.begin(); it != moves.end();) // no icrement here
        {
            try
            {
                MoveResult game_move = game.make_move(it->row, it->col);
                it->score = _heuristic_evaluation();
                game.reverse_move(game_move);
                ++it; // Only increment here if the move was successful
            }
            catch (std::exception &e)
            {
                it = moves.erase(it); // erase returns the next iterator
            }
        }

        sortMovesUtil(moves, maximizingPlayer);
    }

    void GomokuAI::evaluateNode(MoveHeuristic &move, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremeEval, std::pair<int, int> &best_move, int moveIdx)
    {
        //         MoveResult game_move = game.make_move(move.row, move.col);
        //         eval.listMoves.push_back(MoveEvaluation());
        //         MoveEvaluation &evalNode = eval.listMoves.back();
        //         minimax(evalNode, depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
        //         game.reverse_move(game_move);
        //
        //         evalNode.initialScore = move.score;
        //

        //         if (maximizingPlayer)
        //         {
        //             if (evalNode.score >= extremeEval)
        //             {
        //                 extremeEval = evalNode.score;
        //                 eval.score = extremeEval;
        //                 alpha = std::max(alpha, evalNode.score);
        //                 best_move = {move.row, move.col};
        //
        //                 if (evalNode.score > extremeEval)
        //                     eval.neededEvalCount = moveIdx;
        //
        //             }
        //         }
        //         else
        //         {
        //             if (evalNode.score <= extremeEval)
        //             {
        //                 extremeEval = evalNode.score;
        //                 eval.score = extremeEval;
        //                 beta = std::min(beta, evalNode.score);
        //                 best_move = {move.row, move.col};
        //
        //                 if (evalNode.score < extremeEval)
        //                     eval.neededEvalCount = moveIdx;
        //
        //             }
        //         }
    }

    void GomokuAI::minimax(MoveEvaluation &eval, int depth, int alpha, int beta, bool maximizingPlayer, int row, int col)
    {
        TIMER

        eval.move.first = row;
        eval.move.second = col;

        if (depth == 0 || game.is_game_over())
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

        // Else find all the relevant moves and sort them by their heuristic evaluation if the depth is not 1.
        std::vector<MoveHeuristic> moves;
        find_relevant_moves(moves);

        int moveIdx = 1;
        int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        bool firstMove = true;
        std::pair<int, int> best_move = {-1, -1};

        if (killer_move.first == moves[0].row && killer_move.second == moves[0].col)
        {
            try
            {
                MoveResult game_move = game.make_move(moves[0].row, moves[0].col);
                eval.listMoves.push_back(MoveEvaluation());
                MoveEvaluation &evalNode = eval.listMoves.back();
                minimax(evalNode, depth - 1, alpha, beta, !maximizingPlayer, moves[0].row, moves[0].col);
                game.reverse_move(game_move);

                if (maximizingPlayer)
                {
                    extremeEval = evalNode.score;
                    eval.score = extremeEval;
                    alpha = std::max(alpha, evalNode.score);
                    eval.neededEvalCount = moveIdx;
                    best_move = {moves[0].row, moves[0].col};
                }
                else
                {
                    if (evalNode.score <= extremeEval)
                    {
                        extremeEval = evalNode.score;
                        eval.score = extremeEval;
                        beta = std::min(beta, evalNode.score);
                        eval.neededEvalCount = moveIdx;
                        best_move = {moves[0].row, moves[0].col};
                    }
                }

                if (beta <= alpha)
                {
                    eval.evaluatedMoves = moveIdx;
                    return;
                }
                moveIdx++;
            }
            catch (std::exception &e)
            {
                move_count--;
            }

            moves.erase(moves.begin());
            firstMove = false;
        }

        if (depth > 1)
            sortMoves(moves, maximizingPlayer);

        eval.totalEvalCount = moves.size();
        eval.evaluatedMoves = moves.size();

        for (const MoveHeuristic &move : moves)
        {
            try
            {

                MoveResult game_move = game.make_move(move.row, move.col);
                eval.listMoves.push_back(MoveEvaluation());
                MoveEvaluation &evalNode = eval.listMoves.back();
                minimax(evalNode, depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
                game.reverse_move(game_move);

                evalNode.initialScore = move.score;

                if (maximizingPlayer)
                {
                    if (evalNode.score > extremeEval || firstMove)
                    {
                        extremeEval = evalNode.score;
                        eval.score = extremeEval;
                        alpha = std::max(alpha, evalNode.score);
                        best_move = {move.row, move.col};
                        eval.neededEvalCount = moveIdx;
                    }
                }
                else
                {
                    if (evalNode.score <= extremeEval || firstMove)
                    {
                        extremeEval = evalNode.score;
                        eval.score = extremeEval;
                        beta = std::min(beta, evalNode.score);
                        best_move = {move.row, move.col};
                        eval.neededEvalCount = moveIdx;
                    }
                }

                if (beta <= alpha)
                {

                    eval.evaluatedMoves = moveIdx;
                    break;
                }
                moveIdx++;
                firstMove = false;
            }
            catch (std::exception &e)
            {
                move_count--;
            }
        }
        if (best_move.first != -1 && best_move.second != -1)
        {
            killer_move = {best_move.first, best_move.second};
        }

        move_count += moves.size();
        move_evaluated_count += eval.evaluatedMoves;
        evaluation_needed_count += eval.neededEvalCount;
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

    int GomokuAI::_heuristic_evaluation()
    {
        TIMER
        return score_player(ai_player) - score_player(human_player);
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

        move_count = 0;
        move_evaluated_count = 0;
        evaluation_needed_count = 0;

        MoveEvaluation result;
        if (board.has_player_bounds())
            minimax(result, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
        else
            result.listMoves.push_back(MoveEvaluation{{game.get_board_height() / 2, game.get_board_width() / 2}, 1});
        return result;
    }

    void GomokuAI::find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves) const
    {
        TIMER

        auto [min, max] = game.get_played_bounds(length);

        for (int row = min.row; row <= max.row; ++row)
        {
            for (int col = min.col; col <= max.col; ++col)
            {
                if (game.get_board_value(row, col) != E)
                    continue;

                if (is_cell_relevant(row, col))
                {
                    if (killer_move == std::pair<int, int>(row, col))
                        out_relevant_moves.insert(out_relevant_moves.begin(), MoveHeuristic{uint8_t(row), uint8_t(col), 0});
                    else
                        out_relevant_moves.push_back(
                            MoveHeuristic{uint8_t(row), uint8_t(col), 0});
                }
            }
        }
    }

    static const std::vector<std::pair<int, int>> _directions_offsets = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

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
        find_relevant_moves(moves);
        return moves;
    }

    const GomokuAIData &GomokuAI::get_evaluation_data() const
    {
        return evaluation_data;
    }

} // namespace AI::MinMaxV2
