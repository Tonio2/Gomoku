#include "ai/gomoku_ai_minmaxv2.h"
#include "ai/gomoku_ai_minmaxv3.h"
#include "room/game_room.h"

static const AI::MinMaxV2::GomokuAIData ai_data_default = AI::MinMaxV2::GomokuAIData();

static const AI::MinMaxV3::GomokuAIData ai_data_defaultv3 = AI::MinMaxV3::GomokuAIData();

static const AI::MinMaxV2::GomokuAIData ai_data_cpu1 = []()
{
    AI::MinMaxV2::GomokuAIData data;
    data.values[StructureType::NONE] = 0;
    data.values[StructureType::FIVE_OR_MORE] = 2013470;
    data.values[StructureType::OPEN_ONE] = 7.8;
    data.values[StructureType::ONE] = 487;
    data.values[StructureType::OPEN_TWO] = 0;
    data.values[StructureType::TWO] = 0;
    data.values[StructureType::OPEN_THREE] = 4639;
    data.values[StructureType::THREE] = 18.8;
    data.values[StructureType::OPEN_FOUR] = 8538;
    data.values[StructureType::FOUR] = 353;
    data.values[Pattern::FORK] = 565599;
    data.values[Pattern::CAPTURE] = 2.8;
    return data;
}();

static const AI::MinMaxV2::GomokuAIData ai_data_cpu2 = []()
{
    AI::MinMaxV2::GomokuAIData data;
    data.values[StructureType::NONE] = 0;
    data.values[StructureType::FIVE_OR_MORE] = 1.18881e+09;
    data.values[StructureType::OPEN_ONE] = 175.305;
    data.values[StructureType::ONE] = 0.488359;
    data.values[StructureType::OPEN_TWO] = 22.639;
    data.values[StructureType::TWO] = 1.59943;
    data.values[StructureType::OPEN_THREE] = 375669;
    data.values[StructureType::THREE] = 21.7746;
    data.values[StructureType::OPEN_FOUR] = 5684.55;
    data.values[StructureType::FOUR] = 16355.2;
    data.values[Pattern::FORK] = 7501.18;
    data.values[Pattern::CAPTURE] = 0.00172756;
    return data;
}();

static const AI::MinMaxV2::GomokuAIData ai_data_cpu3 = []()
{
    AI::MinMaxV2::GomokuAIData data;
    data.values[StructureType::NONE] = 0;
    data.values[StructureType::FIVE_OR_MORE] = 322.666;
    data.values[StructureType::OPEN_ONE] = 277.998;
    data.values[StructureType::ONE] = 9.73701;
    data.values[StructureType::OPEN_TWO] = 5.72728;
    data.values[StructureType::TWO] = 163.257;
    data.values[StructureType::OPEN_THREE] = 175.367;
    data.values[StructureType::THREE] = 241.945;
    data.values[StructureType::OPEN_FOUR] = 4736.6;
    data.values[StructureType::FOUR] = 2739.88;
    data.values[Pattern::FORK] = 2588.87;
    data.values[Pattern::CAPTURE] = 26.7119;
    return data;
}();

static const AI::MinMaxV2::GomokuAIData ai_data_cpu4 = []()
{
    AI::MinMaxV2::GomokuAIData data;
    data.values[StructureType::NONE] = 0;
    data.values[StructureType::FIVE_OR_MORE] = 911878;
    data.values[StructureType::OPEN_ONE] = 417;
    data.values[StructureType::ONE] = 2153;
    data.values[StructureType::OPEN_TWO] = 1.5;
    data.values[StructureType::TWO] = 4.7;
    data.values[StructureType::OPEN_THREE] = 37494;
    data.values[StructureType::THREE] = 423;
    data.values[StructureType::OPEN_FOUR] = 44508;
    data.values[StructureType::FOUR] = 1967;
    data.values[Pattern::FORK] = 12800;
    data.values[Pattern::CAPTURE] = 9.1;
    return data;
}();

// Function to create AI
AI::IGomokuAI *create_aiv2(int depth, int length, const AI::MinMaxV2::GomokuAIData &data)
{
    return new AI::MinMaxV2::GomokuAI({depth, length, data});
}

AI::IGomokuAI *create_aiv3(int depth, int length, const AI::MinMaxV3::GomokuAIData &data)
{
    return new AI::MinMaxV3::GomokuAI({depth, length, data});
}

// Helper macros to simplify AI creation functions
#define CREATE_AI_FUNC(name, depth, length, data) \
    AI::IGomokuAI *create_##name##_ai()           \
    {                                             \
        return create_aiv2(depth, length, data);  \
    }

#define CREATE_AI_FUNCV3(name, depth, length, data) \
    AI::IGomokuAI *create_##name##_ai()             \
    {                                               \
        return create_aiv2(depth, length, data);    \
    }

CREATE_AI_FUNC(default, 3, 2, ai_data_default)
CREATE_AI_FUNC(default_d4, 4, 2, ai_data_default)
CREATE_AI_FUNC(default_d5, 5, 2, ai_data_default)
CREATE_AI_FUNC(default_d6, 6, 2, ai_data_default)

CREATE_AI_FUNCV3(quiescent, 3, 2, ai_data_default)
CREATE_AI_FUNCV3(quiescent_d4, 4, 2, ai_data_default)
CREATE_AI_FUNCV3(quiescent_d5, 5, 2, ai_data_default)
CREATE_AI_FUNCV3(quiescent_d6, 6, 2, ai_data_default)

CREATE_AI_FUNC(cpu1, 3, 2, ai_data_cpu1)
CREATE_AI_FUNC(cpu1_d4, 4, 2, ai_data_cpu1)
CREATE_AI_FUNC(cpu1_d5, 5, 2, ai_data_cpu1)
CREATE_AI_FUNC(cpu1_d6, 6, 2, ai_data_cpu1)

CREATE_AI_FUNC(cpu2, 3, 2, ai_data_cpu2)
CREATE_AI_FUNC(cpu2_d4, 4, 2, ai_data_cpu2)
CREATE_AI_FUNC(cpu2_d5, 5, 2, ai_data_cpu2)
CREATE_AI_FUNC(cpu2_d6, 6, 2, ai_data_cpu2)

CREATE_AI_FUNC(cpu3, 3, 2, ai_data_cpu3)
CREATE_AI_FUNC(cpu3_d4, 4, 2, ai_data_cpu3)
CREATE_AI_FUNC(cpu3_d5, 5, 2, ai_data_cpu3)
CREATE_AI_FUNC(cpu3_d6, 6, 2, ai_data_cpu3)

CREATE_AI_FUNC(cpu4, 3, 2, ai_data_cpu4)
CREATE_AI_FUNC(cpu4_d4, 4, 2, ai_data_cpu4)
CREATE_AI_FUNC(cpu4_d5, 5, 2, ai_data_cpu4)
CREATE_AI_FUNC(cpu4_d6, 6, 2, ai_data_cpu4)

static const std::vector<std::pair<std::string, std::function<AI::IGomokuAI *()>>>
    ai_makers = {
        {"default", create_default_ai},
        {"default_d4", create_default_d4_ai},
        {"default_d5", create_default_d5_ai},
        {"default_d6", create_default_d6_ai},
        {"cpu1", create_cpu1_ai},
        {"cpu1_d4", create_cpu1_d4_ai},
        {"cpu1_d5", create_cpu1_d5_ai},
        {"cpu1_d6", create_cpu1_d6_ai},
        {"cpu2", create_cpu2_ai},
        {"cpu2_d4", create_cpu2_d4_ai},
        {"cpu2_d5", create_cpu2_d5_ai},
        {"cpu2_d6", create_cpu2_d6_ai},
        {"cpu3", create_cpu3_ai},
        {"cpu3_d4", create_cpu3_d4_ai},
        {"cpu3_d5", create_cpu3_d5_ai},
        {"cpu3_d6", create_cpu3_d6_ai},
        {"cpu4", create_cpu4_ai},
        {"cpu4_d4", create_cpu4_d4_ai},
        {"cpu4_d5", create_cpu4_d5_ai},
        {"cpu4_d6", create_cpu4_d6_ai},
        {"quiescent", create_quiescent_ai},
        {"quiescent_d4", create_quiescent_d4_ai},
        {"quiescent_d5", create_quiescent_d5_ai},
        {"quiescent_d6", create_quiescent_d6_ai},
};

static const std::vector<std::string> ai_names = []
{
    std::vector<std::string> names;
    for (const auto &[name, _] : ai_makers)
        names.push_back(name);
    return names;
}();

AI::IGomokuAI *GameEntitySetting::make_ai() const
{
    if (!is_ai)
        return nullptr;

    for (const auto &[name, maker] : ai_makers)
    {
        if (name == ai_name)
            return maker();
    }

    return nullptr;
}

std::vector<std::string> get_ai_names_list()
{
    return ai_names;
}
