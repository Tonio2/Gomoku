#include "ai/gomoku_ai_minmaxv3.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <random>

#include "utils/gomoku_utilities.h"

#ifdef LOGGING
#define LOG_MOVE_EVALUATION logMoveEvaluation(result, "/tmp/evals/" + std::to_string(currentMove) + ".txt");
#else
#define LOG_MOVE_EVALUATION
#endif

namespace AI::MinMaxV3
{

Move GomokuAI::suggest_move(const GomokuGame &board, int currentMove)
{
    MoveEvaluation result = suggest_move_evaluation(board);
    LOG_MOVE_EVALUATION
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
        moves.push_back(Move(result.row, result.col));
        maximizing = !maximizing;
    }

    return moves;
}

GomokuAI::GomokuAI(const GomokuAiSettings &settings)
    : game(0, 0), depth(settings.depth), length(settings.length), evaluation_data(settings.data)
{
    killer_moves = std::vector<std::pair<int, int>>(depth, {-1, -1});
}

void sortMovesUtil(MoveEvaluation &eval, bool maximizingPlayer)
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
    std::sort(eval.relevant_moves.begin() + eval.cur_move_id, eval.relevant_moves.end(), compare);
}

void GomokuAI::sortMoves(MoveEvaluation &eval, bool maximizingPlayer)
{
    TIMER
    for (auto it = eval.relevant_moves.begin(); it != eval.relevant_moves.end();) // no icrement here
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
            it = eval.relevant_moves.erase(it); // erase returns the next iterator
        }
    }

    sortMovesUtil(eval, maximizingPlayer);
}

void GomokuAI::evaluateNode(const MoveHeuristic &move, int move_id, int _depth, MoveEvaluation &eval, int &alpha, int &beta, bool maximizingPlayer, int &extremeEval, std::pair<int, int> &best_move, bool isFirstMove)
{
    MoveResult game_move = game.make_move(move.row, move.col, _depth > 1);
    eval.listMoves.push_back(MoveEvaluation{move.row, move.col});
    MoveEvaluation &evalNode = eval.listMoves.back();
    minimax(evalNode, _depth - 1, alpha, beta, !maximizingPlayer);
    game.reverse_move(game_move, _depth > 1);
    if (maximizingPlayer)
    {
        if (evalNode.score > extremeEval || isFirstMove)
        {
            extremeEval = evalNode.score;
            eval.score = extremeEval;
            alpha = std::max(alpha, extremeEval);
            best_move = {move.row, move.col};
            eval.best_move_id = move_id;
        }
    }
    else
    {
        if (evalNode.score < extremeEval || isFirstMove)
        {
            extremeEval = evalNode.score;
            eval.score = extremeEval;
            beta = std::min(beta, extremeEval);
            best_move = {move.row, move.col};
            eval.best_move_id = move_id;
        }
    }
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

    compute_relevant_moves(eval.relevant_moves, _depth);

    bool isFirstMove = true;
    int extremeEval = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    std::pair<int, int> best_move = {-1, -1};

    const std::pair<int, int> &killer_move = killer_moves[depth - _depth];
    if (killer_move.first == eval.relevant_moves[0].row && killer_move.second == eval.relevant_moves[0].col)
    {
        try
        {
            evaluateNode(eval.relevant_moves[0], eval.cur_move_id, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, best_move, isFirstMove);

            if (beta <= alpha)
            {
                return;
            }
            isFirstMove = false;
        }
        catch (std::exception &e)
        {
        }
        eval.cur_move_id++;
    }

    if (_depth > 1)
        sortMoves(eval, maximizingPlayer);

    while (eval.cur_move_id < eval.relevant_moves.size())
    {
        try
        {
            evaluateNode(eval.relevant_moves[eval.cur_move_id], eval.cur_move_id, _depth, eval, alpha, beta, maximizingPlayer, extremeEval, best_move, isFirstMove);

            if (beta <= alpha)
            {
                break;
            }
            isFirstMove = false;
        }
        catch (std::exception &e)
        {
        }
        eval.cur_move_id++;
    }

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
        minimax(result, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);
    else
        result.listMoves.push_back(MoveEvaluation{game.get_board_height() / 2, game.get_board_width() / 2});
    return result;
}

void GomokuAI::compute_relevant_moves(std::vector<MoveHeuristic> &out_relevant_moves, int _depth) const
{
    TIMER

    auto [min, max] = game.get_played_bounds(length);
    out_relevant_moves.reserve((max.row - min.row + 1) * (max.col - min.col + 1));

    const auto &killer_move = killer_moves[depth - _depth];
    bool killer_move_inserted = false;
    for (int row = min.row; row <= max.row; ++row)
    {
        for (int col = min.col; col <= max.col; ++col)
        {
            if (game.get_cell_relevancy(row, col) <= 0 || game.get_board_value(row, col) != E)
                continue;

            if (!killer_move_inserted && killer_move == std::pair<int, int>(row , col))
            {
                out_relevant_moves.emplace_back(MoveHeuristic{uint8_t(row), uint8_t(col), 0});
                killer_move_inserted = true;
            }
            else
            {
                out_relevant_moves.emplace_back(MoveHeuristic{uint8_t(row), uint8_t(col), 0});
            }
        }
    }

    // Move the killer move to the front if it was found
    if (killer_move_inserted)
    {
        auto it = std::find_if(out_relevant_moves.begin(), out_relevant_moves.end(), [&](const MoveHeuristic& move) {
            return move.row == killer_move.first && move.col == killer_move.second;
        });
        if (it != out_relevant_moves.begin())
        {
            std::iter_swap(out_relevant_moves.begin(), it);
        }
    }
}

const GomokuAIData &GomokuAI::get_evaluation_data() const
{
    return evaluation_data;
}

std::pair<int, int> getBestMove(const MoveEvaluation &eval, bool maximizingPlayer)
{
    if (eval.listMoves.empty())
        return std::make_pair(-1, -1);
    
    MoveEvaluation best_move_evaluation = eval.listMoves[eval.best_move_id];
    return std::make_pair(best_move_evaluation.row, best_move_evaluation.col);
}

const MoveEvaluation &getBestMoveEvaluation(const MoveEvaluation &eval, bool maximizingPlayer)
{
    return eval.listMoves[eval.best_move_id];
}

void writeMoveEvaluation(std::ostream &out, const MoveEvaluation &eval, std::vector<std::vector<std::string>> &csvData, int depth, int i)
{
    // Create indentation based on the depth
    std::string indent(depth * 4, ' '); // 4 spaces for each level of depth

    out << indent;
    if (i >= 0)
    {
        out << i << ": ";
    }
    out << "Move: "
        << coordinate_to_char(eval.row)
        << coordinate_to_char(eval.col)
        << " | "
        << eval.score
        << " | Initial score: " << eval.initial_score << "\n";


    if (eval.listMoves.size() > 0)
    {
        out << indent << "Stats: Best: " << eval.best_move_id << " | Evaluated: " << eval.cur_move_id << " | Total: " << eval.relevant_moves.size() << "\n";
        if (csvData.size() <= depth)
        {
            csvData.resize(depth + 1);
        }
        csvData[depth].push_back(std::to_string(eval.best_move_id) + "," + std::to_string(eval.cur_move_id) + "," + std::to_string(eval.relevant_moves.size()) + ",");
    }

    if (not eval.listMoves.empty())
    {
        out << indent << "ListMoves:\n";

        for (int i = 0; i < eval.listMoves.size(); i++)
        {
            const auto &move = eval.listMoves[i];
            // Increase the indentation for each level
            writeMoveEvaluation(out, move, csvData, depth + 1, i + 1);
        }
    }
}

void logMoveEvaluation(const MoveEvaluation &eval, std::string filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    // new filename with csv instead of txt

    std::vector<std::vector<std::string>> csvData(get_depth_from_env(), std::vector<std::string>());

    writeMoveEvaluation(out, eval, csvData);
    out.close();

    std::string csvFilename = filename.substr(0, filename.size() - 4) + ".csv";
    std::ofstream csvOut(csvFilename);
    if (!csvOut.is_open())
    {
        std::cerr << "Failed to open " << csvFilename << std::endl;
        return;
    }

    bool continueLoop = true;
    int line = 0;
    while (continueLoop)
    {
        continueLoop = false;
        for (int depth = 0; depth < csvData.size(); depth++)
        {
            if (line < csvData[depth].size())
            {
                csvOut << csvData[depth][line] << ",";
                continueLoop = true;
            }
            else
            {
                csvOut << ",,,,";
            }
        }
        if (continueLoop)
        {
            csvOut << "\n";
        }
        line++;
    }
    csvOut.close();
}


} // namespace AI::MinMaxV3


