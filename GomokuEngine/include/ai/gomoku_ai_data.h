
#pragma once

#include "engine/gomoku_engine_types.h"

namespace AI::MinMaxV2
{

class GomokuAIData
{
public:
    GomokuAIData();
    GomokuAIData(const GomokuAIData &copy);
    GomokuAIData &operator=(const GomokuAIData &copy);
    virtual ~GomokuAIData();

    void save_to_file(std::string filename) const;
    void load_from_file(std::string filename);

    float value_of_structure(int structure_type) const;
    float value_of_multiple_forced() const;
    float value_of_multiple_o4() const;
    float value_of_captures(int capture_count) const;

    static const int STC = StructureType::COUNT_STRUCTURE_TYPE;
    static const int VALUES_COUNT = STC + 5;
    float values[VALUES_COUNT];

private:
    friend class GomokuAIDataMutator;

    friend std::ostream &operator<<(std::ostream &stream, const GomokuAIData &ai_data);
};

std::ostream &operator<<(std::ostream &stream, const GomokuAIData &ai_data);

} // namespace AI::MinMaxV2
