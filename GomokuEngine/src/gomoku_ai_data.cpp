
#include "gomoku_ai_data.h"

void GomokuAIData::initialize()
{
    for (int i = 0; i < VALUES_COUNT; i++)
    {
        values[i] = 1000;
    }
    values[STC + 2] = 1;
    values[STC + 3] = 1;
    values[STC + 4] = 1;
}

GomokuAIData::GomokuAIData()
{
    initialize();
}

GomokuAIData::GomokuAIData(const GomokuAIData &copy)
{
    for (int i = 0; i < VALUES_COUNT; i++)
        values[i] = copy.values[i];
}

GomokuAIData &GomokuAIData::operator=(const GomokuAIData &copy)
{
    if (this != &copy)
    {
        for (int i = 0; i < VALUES_COUNT; i++)
            values[i] = copy.values[i];
    }
    return *this;
}

GomokuAIData::~GomokuAIData()
{
}

float GomokuAIData::value_of_structure(int structure_type) const
{
    return values[structure_type];
}

float GomokuAIData::value_of_multiple_forced() const
{
    return values[STC + 0];
}

float GomokuAIData::value_of_multiple_o4() const
{
    return values[STC + 1];
}

float GomokuAIData::value_of_captures(int capture_count) const
{
    const float a = values[STC + 2];
    const float b = values[STC + 3];
    const float c = values[STC + 4];

    return a * (capture_count * capture_count) + b * (capture_count) + c;
}
