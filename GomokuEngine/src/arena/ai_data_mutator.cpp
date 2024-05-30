
#include "arena/ai_data_mutator.h"
#include <algorithm>
#include <filesystem>
#include <random>
#include <sstream>

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

void GomokuAIDataMutator::mutate_data(AI::MinMaxV2::GomokuAIData &data) const
{
    for (int i = 0; i < AI::MinMaxV2::GomokuAIData::VALUES_COUNT; i++)
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
