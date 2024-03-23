
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
