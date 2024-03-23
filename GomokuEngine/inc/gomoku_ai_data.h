
#pragma once

#include "gomoku_engine_types.h"

class GomokuAIData
{
public:
    GomokuAIData();

    GomokuAIData(const GomokuAIData &copy);

    GomokuAIData &operator=(const GomokuAIData &copy);

    virtual ~GomokuAIData();

    float value_of_structure(int structure_type) const;

    float value_of_multiple_forced() const;

    float value_of_multiple_o4() const;

    float value_of_captures(int capture_count) const;

private:
    static const int STC = StructureType::COUNT_STRUCTURE_TYPE;
    static const int VALUES_COUNT = STC + 5;
    float values[VALUES_COUNT];

    void initialize();

    friend class GomokuAIDataMutator;
};
