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
        std::string filename = "/tmp/evals/" + std::to_string(currentMove) + ".txt";
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
                    it->isThreat = _heuristic_evaluation(it->score, maximizingPlayer);
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
        minimax(evalNode, _depth - 1, alpha, beta, !maximizingPlayer, move.row, move.col);
        game.reverse_move(game_move);
#ifdef LOGGING
        evalNode.initialScore = move.score;
#endif
        evalNode.isThreat = move.isThreat;
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
                eval.isThreat = _heuristic_evaluation(eval.score, maximizingPlayer);
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

    bool GomokuAI::_heuristic_evaluation(int &score, int maximizingPlayer)
    {
        TIMER
        bool threatening = score_player(ai_player, score, 1);
        bool threatened = score_player(human_player, score, -1);
        return maximizingPlayer ? threatening : threatened;
    }

    bool GomokuAI::score_player(Player player, int &score, int multiplier)
    {
        const std::vector<int> &patterns_count = game.get_patterns_count(player);

        for (int i = 0; i < StructureType::COUNT_STRUCTURE_TYPE; i++)
        {
            score += multiplier * (patterns_count[i] * evaluation_data.value_of_structure(i));
        }
        score += multiplier * ((patterns_count[OPEN_THREE] + patterns_count[FOUR] + patterns_count[OPEN_FOUR] >= 2) ? evaluation_data.value_of_multiple_forced() : 0);
        score += multiplier * ((patterns_count[OPEN_FOUR] >= 2 ? evaluation_data.value_of_multiple_o4() : 0));
        int capture_score = game.get_player_score(player);
        score += multiplier * (evaluation_data.value_of_captures(capture_score));

        return patterns_count[FOUR] > 0 || patterns_count[OPEN_FOUR] > 0 || patterns_count[OPEN_THREE] > 0 || (capture_score == 8 && patterns_count[TWO] > 0);
    }

    int GomokuAI::get_heuristic_evaluation(const GomokuGame &board, Player player)
    {
        TIMER

        game = board;
        ai_player = player;
        human_player = board.other_player(player);
        MoveEvaluation eval;
        int score = 0;
        _heuristic_evaluation(score, true);
        return score;
    }

    MoveEvaluation GomokuAI::suggest_move_evaluation(const GomokuGame &board)
    {
        Timer timer(__FUNCTION__);

        game = board;
        ai_player = board.get_current_player();
        human_player = board.other_player(ai_player);

        MoveEvaluation result;
        if (board.has_player_bounds())
            minimax(result, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, -1, -1);
        else
            result.listMoves.push_back(MoveEvaluation{{game.get_board_height() / 2, game.get_board_width() / 2}, 1});
        return result;
    }

    void GomokuAI::find_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves, int _depth) const
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
                    if (killer_moves[depth - _depth] == std::pair<int, int>(row, col))
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
        find_relevant_moves(moves, depth);
        return moves;
    }

    const GomokuAIData &GomokuAI::get_evaluation_data() const
    {
        return evaluation_data;
    }

} // namespace AI::MinMaxV2
