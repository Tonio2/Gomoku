
#pragma once

#include "ai/gomoku_ai_datav2.h"

class GomokuAIDataMutator
{

public:
    GomokuAIDataMutator();

    void mutate_data(AI::MinMaxV2::GomokuAIData &data) const;

    void increase_impact();

    void decrease_impact();

private:
    /** Probability of modifying data */
    float _change_proba;

    /** Proportion of data modification range */
    float _proportion;
};
