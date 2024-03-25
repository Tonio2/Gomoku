
#include "arena.h"
#include <random>
#include <algorithm>
#include <filesystem>

static float random_float(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    return dis(gen);
}

GomokuAIDataMutator::GomokuAIDataMutator()
    : _change_proba(0.9), _proportion(1.1)
{
}

void GomokuAIDataMutator::mutate_data(GomokuAIData &data) const
{
    for (int i = 0; i < GomokuAIData::VALUES_COUNT; i++)
    {
        const bool modify = random_float(0, 1) <= _change_proba;

        if (!modify)
            return;

        const float modify_proportion = random_float(1, _proportion);

        const bool increase = random_float(0, 1) < 0.5;
        if (increase)
            data.values[i] *= modify_proportion;
        else
            data.values[i] /= modify_proportion;
    }
}

void GomokuAIDataMutator::increase_impact()
{
    _proportion *= 1.1;
}

void GomokuAIDataMutator::decrease_impact()
{
    _proportion /= 1.01;
}

Arena::Arena() : _game_width(19), _game_height(19)
{
}

void Arena::play(int argc, char *argv[])
{
    GomokuAIData p1;
    GomokuAIData p2;

    if (argc > 2)
        p1.load_from_file(argv[2]);
    else
    {
        try
        {
            p1.load_from_file("ai_data/p1_last.gkd");
        }
        catch (std::exception &e)
        {
        }
    }

    if (argc > 3)
        p2.load_from_file(argv[3]);
    else
    {
        try
        {
            p2.load_from_file("ai_data/p2_last.gkd");
        }
        catch (std::exception &e)
        {
        }
    }

    std::cout << "p1 => " << p1 << std::endl;
    std::cout << "p2 => " << p2 << std::endl;

    GomokuAIDataMutator mutator;

    int win_streak_p1 = 0;
    int win_streak_p2 = 0;

    int streaker_index = 0;

    std::string last_file = "";

    while (true)
    {
        int first_winner = play_game(p1, p2);
        int second_winner = play_game(p2, p1);
        second_winner = second_winner == 0 ? 0 : 3 - second_winner;

        int p1_wins = (first_winner == 1) + (second_winner == 1);
        int p2_wins = (first_winner == 2) + (second_winner == 2);

        auto player_win = [&mutator, &streaker_index, &last_file](std::string winner_name, GomokuAIData &winner_data, int &winner_streak, std::string loser_name, GomokuAIData &loser_data, int &loser_streak)
        {
            // mutator.decrease_impact();
            std::cout << winner_name << " win ~" << loser_name << std::endl;

            ++winner_streak;
            if (loser_streak >= 10)
            {
                streaker_index++;
                last_file = "";
            }
            loser_streak = 0;

            if (winner_streak >= 20)
            {
                if (last_file != "")
                {
                    try
                    {
                        std::filesystem::remove(last_file);
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "Error: Unable to delete file: " << e.what() << std::endl;
                    }
                }
                std::ostringstream filename;
                filename << "ai_data/streaker-" << streaker_index << "-" << winner_name << "-" << winner_streak << ".gkd";
                last_file = filename.str();
                winner_data.save_to_file(last_file);
                loser_data = winner_data;
            }

            mutator.mutate_data(loser_data);
        };

        if (p1_wins > p2_wins)
        {
            player_win("p1", p1, win_streak_p1, "p2", p2, win_streak_p2);
        }
        else if (p2_wins > p1_wins)
        {
            player_win("p2", p2, win_streak_p2, "p1", p1, win_streak_p1);
        }
        else
        {
            // mutator.increase_impact();
            // mutator.mutate_data(p1);

            std::cout << "draw " << p1_wins << ' ' << p2_wins;
            if (win_streak_p1 > 0)
            {
                win_streak_p1 = 1;
                mutator.mutate_data(p2);
                std::cout << " ~p2";
            }
            else if (win_streak_p2 > 0)
            {
                win_streak_p2 = 1;
                mutator.mutate_data(p1);
                std::cout << " ~p1";
            }
            else
            {
                mutator.mutate_data(p2);
                std::cout << " ~p2";
            }
            std::cout << std::endl;
        }

        p1.save_to_file("ai_data/p1_last.gkd");
        p2.save_to_file("ai_data/p2_last.gkd");
    }
}

int Arena::play_game(const GomokuAIData &p1, const GomokuAIData &p2)
{
    GomokuGame game(_game_width, _game_height);

    GomokuAI ai1(DEPTH, p1);
    GomokuAI ai2(DEPTH, p2);

    game.make_move(9, 9);

    while (!game.is_game_over())
    {
        GomokuAI *current_ai = game.get_current_player() == X ? &ai1 : &ai2;

        MoveEvaluation moves = current_ai->suggest_move(game, game.get_current_player());

        auto best_move = std::max_element(moves.listMoves.begin(), moves.listMoves.end(), [](const MoveEvaluation &a, const MoveEvaluation &b)
                                          { return a.score < b.score; });

        game.make_move(best_move->move.first, best_move->move.second);
    }

    if (game.get_winner() == X)
    {
        return 1;
    }
    else if (game.get_winner() == O)
    {
        return 2;
    }

    return 0;
}
