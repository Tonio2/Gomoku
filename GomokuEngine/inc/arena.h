
#include "gomoku_engine.h"
#include "gomoku_ai.h"

class GomokuAIDataMutator
{

public:
    GomokuAIDataMutator();

    void mutate_data(GomokuAIData &data) const;

    void increase_impact();

    void decrease_impact();

private:
    /** Probability of modifying data */
    float _change_proba;

    /** Proportion of data modification range */
    float _proportion;
};

class Arena
{
public:
    Arena();

    void play();

    /** Play one game and return the winner */
    int play_game(const GomokuAIData &p1, const GomokuAIData &p2);

private:
    int _game_width;
    int _game_height;
};
