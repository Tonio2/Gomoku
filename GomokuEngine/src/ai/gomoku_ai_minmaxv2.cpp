#include "ai/gomoku_ai_minmaxv2.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

#include "utils/gomoku_utilities.h"

namespace AI::MinMaxV2
{

    Move GomokuAI::suggest_move(const GomokuGame &board, int currentMove)
    {
        MoveEvaluation result = suggest_move_evaluation(board);
#ifdef LOGGING
        std::string filename = "/tmp/move_evaluation/" + std::to_string(currentMove) + ".txt";
        logMoveEvaluation(result, filename);
#endif
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
        : game(0, 0), depth(settings.depth), length(settings.length), evaluation_data(settings.data), human_player(Player::EMPTY), ai_player(Player::EMPTY), killer_moves(std::vector<std::pair<int, int>>(depth, {-1, -1}))
    {
    }

    void sortMovesUtil(std::vector<MoveEvaluation> &moves, bool maximizingPlayer)
    {
        TIMER

        std::function<bool(const MoveEvaluation &, const MoveEvaluation &)> compare;

        if (maximizingPlayer)
        {
            compare = [](const MoveEvaluation &a, const MoveEvaluation &b)
            {
                return a.score > b.score;
            };
        }
        else
        {
            compare = [](const MoveEvaluation &a, const MoveEvaluation &b)
            {
                return a.score < b.score;
            };
        }
        std::sort(moves.begin(), moves.end(), compare);
    }

    //     void GomokuAI::sortMoves(std::vector<MoveHeuristic> &moves, bool maximizingPlayer, MoveEvaluation &eval)
    //     {
    //         TIMER
    //         for (auto it = moves.begin(); it != moves.end();) // no icrement here
    //         {
    //             // find move in eval.listMoves
    //             auto itEval = std::find_if(eval.listMoves.begin(), eval.listMoves.end(), [&it](const MoveEvaluation &moveEval)
    //                                        { return moveEval.move.first == it->row && moveEval.move.second == it->col; });

    //             if (itEval != eval.listMoves.end())
    //             {
    //                 it->score = itEval->score;
    //                 ++it;
    //             }
    //             else
    //             {
    //                 try
    //                 {
    //                     MoveResult game_move = game.make_move(it->row, it->col);
    //                     if (game.is_game_over())
    //                     {
    //                         if (game.get_winner() == ai_player)
    //                             it->score = std::numeric_limits<int>::max();
    //                         else if (game.get_winner() == human_player)
    //                             it->score = std::numeric_limits<int>::min();
    //                         else
    //                             it->score = 0;
    //                     }
    //                     else
    //                         it->score = _heuristic_evaluation();
    //                     game.reverse_move(game_move);
    //                     ++it; // Only increment here if the move was successful
    //                 }
    //                 catch (std::exception &e)
    //                 {
    //                     it = moves.erase(it); // erase returns the next iterator
    // #ifdef LOGGING
    //                     eval.totalEvalCount--;
    // #endif
    //                 }
    //             }
    //         }

    //         sortMovesUtil(moves, maximizingPlayer);
    //     }

    bool GomokuAI::evaluateNode(int moveId, int _depth, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremeEval, std::pair<int, int> &bestMove, bool isFirstMove)
    {
        try
        {
            MoveEvaluation &evalNode = eval.listMoves[moveId];
#ifdef LOGGING
            evalNode.initialScore = evalNode.score;
#endif
            MoveResult game_move = game.make_move(evalNode.move.first, evalNode.move.second);
            minimax(evalNode, _depth - 1, alpha, beta, !maximizingPlayer);
            game.reverse_move(game_move);

            if (maximizingPlayer)
            {
                if (evalNode.score > extremeEval || isFirstMove)
                {
                    extremeEval = evalNode.score;
                    eval.score = extremeEval;
                    alpha = std::max(alpha, evalNode.score);
                    bestMove = evalNode.move;
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
                    bestMove = evalNode.move;
#ifdef LOGGING
                    eval.neededEvalCount = moveId + 1 + eval.killerMoveHasBeenEvaluated;
#endif
                }
            }
        }
        catch (std::exception &e)
        {
            eval.listMoves.erase(eval.listMoves.begin() + moveId);
            return false;
        }
        return true;
    }

    void GomokuAI::minimax(MoveEvaluation &eval, int _depth, int alpha, int beta, bool maximizingPlayer)
    {
        TIMER

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

        if (eval.listMoves.size() == 0)
            find_relevant_moves(eval, maximizingPlayer, _depth);

        int moveId = 0;
        int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        std::pair<int, int> bestMove = {-1, -1};

        const std::pair<int, int> &killer_move = killer_moves[_depth];
        if (killer_move.first == eval.listMoves[0].move.first && killer_move.second == eval.listMoves[0].move.second)
        {
            if (evaluateNode(moveId, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, bestMove, moveId == 0))
                moveId++;

            if (beta <= alpha)
            {
                return;
            }
        }
#ifdef LOGGING
        eval.totalEvalCount = eval.listMoves.size();
#endif

        if (_depth > 1)
            sortMovesUtil(eval.listMoves, maximizingPlayer);

        while (moveId < eval.listMoves.size())
        {
            if (evaluateNode(moveId, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, bestMove, moveId == 0))
                moveId++;

            if (beta <= alpha)
            {
                killer_moves[_depth] = bestMove;
                break;
            }
        }
        killer_moves[_depth] = bestMove;
#ifdef LOGGING
        eval.evaluatedEvalCount = std::min((int)eval.listMoves.size(), moveId + 1) + eval.killerMoveHasBeenEvaluated;
#endif
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
        if (board.has_player_bounds())
        {
            auto start_time = std::chrono::high_resolution_clock::now();
            int current_depth = 1;
            while (true)
            {
                while (killer_moves.size() <= current_depth)
                    killer_moves.push_back({-1, -1});
                minimax(result, current_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);

                auto current_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = current_time - start_time;

                if (elapsed.count() > 0.2 || result.score == std::numeric_limits<int>::max() || result.score == std::numeric_limits<int>::min())
                {
                    break;
                }

                ++current_depth;
            }
        }
        else
            result.listMoves.push_back(MoveEvaluation{{game.get_board_height() / 2, game.get_board_width() / 2}, 1});
        return result;
    }

    void GomokuAI::find_relevant_moves(MoveEvaluation &eval, bool maximizingPlayer, int _depth)
    {
        TIMER

        int extremEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        auto [min, max] = game.get_played_bounds(length);

        for (int row = min.row; row <= max.row; ++row)
        {
            for (int col = min.col; col <= max.col; ++col)
            {
                if (game.get_board_value(row, col) != E)
                    continue;

                if (is_cell_relevant(row, col))
                {
                    if (killer_moves[_depth] == std::pair<int, int>{row, col})
                        eval.listMoves.insert(eval.listMoves.begin(),
                                              MoveEvaluation{{uint8_t(row), uint8_t(col)}, extremEval});
                    else
                        eval.listMoves.push_back(
                            MoveEvaluation{{uint8_t(row), uint8_t(col)}, extremEval});
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

    // std::vector<MoveHeuristic> GomokuAI::get_relevant_moves(const GomokuGame &board)
    // {
    //     game = board;
    //     ai_player = board.get_current_player();
    //     human_player = board.other_player(ai_player);

    //     std::vector<MoveHeuristic> moves;
    //     find_relevant_moves(moves, depth);
    //     return moves;
    // }

    const GomokuAIData &GomokuAI::get_evaluation_data() const
    {
        return evaluation_data;
    }

} // namespace AI::MinMaxV2
