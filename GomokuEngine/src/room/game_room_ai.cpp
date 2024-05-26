#include "room/game_room.h"
#include "ai/gomoku_ai_minmaxv2.h"
#include "ai/gomoku_ai_minmaxv3.h"

static const GomokuAIData ai_data_default = GomokuAIData();

static const GomokuAIData ai_data_cpu1 = []()
{
    GomokuAIData data;
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

static const GomokuAIData ai_data_cpu2 = []()
{
    GomokuAIData data;
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

static const GomokuAIData ai_data_cpu3 = []()
{
    GomokuAIData data;
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

static const GomokuAIData ai_data_cpu4 = []()
{
    GomokuAIData data;
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

AI::IGomokuAI *create_easy_ai()
{
    return new AI::MinMaxV2::GomokuAI({1, 1, ai_data_default});
}

AI::IGomokuAI *create_medium_ai()
{
    return new AI::MinMaxV2::GomokuAI({2, 1, ai_data_default});
}

AI::IGomokuAI *create_hard_ai()
{
    return new AI::MinMaxV2::GomokuAI({3, 2, ai_data_default});
}

AI::IGomokuAI *create_hardcore_ai()
{
    return new AI::MinMaxV2::GomokuAI({4, 2, ai_data_default});
}

AI::IGomokuAI *create_ultra_hardcore_ai()
{
    return new AI::MinMaxV2::GomokuAI({5, 2, ai_data_default});
}

AI::IGomokuAI *create_giga_hardcore_ai()
{
    return new AI::MinMaxV2::GomokuAI({6, 2, ai_data_default});
}

AI::IGomokuAI *create_deep7()
{
    return new AI::MinMaxV2::GomokuAI({7, 2, ai_data_default});
}

AI::IGomokuAI *create_deep8()
{
    return new AI::MinMaxV2::GomokuAI({8, 2, ai_data_default});
}

AI::IGomokuAI *create_deep9()
{
    return new AI::MinMaxV2::GomokuAI({9, 2, ai_data_default});
}

AI::IGomokuAI *create_deep10()
{
    return new AI::MinMaxV2::GomokuAI({10, 2, ai_data_default});
}

AI::IGomokuAI *create_deep_ai()
{
    return new AI::MinMaxV2::GomokuAI({5, 1, ai_data_default});
}

AI::IGomokuAI *create_cpu1_ai()
{
    return new AI::MinMaxV2::GomokuAI({3, 2, ai_data_cpu1});
}

AI::IGomokuAI *create_cpu2_ai()
{
    return new AI::MinMaxV2::GomokuAI({3, 2, ai_data_cpu2});
}

AI::IGomokuAI *create_cpu3_ai()
{
    return new AI::MinMaxV2::GomokuAI({3, 2, ai_data_cpu3});
}

AI::IGomokuAI *create_cpu4_ai()
{
    return new AI::MinMaxV2::GomokuAI({3, 2, ai_data_cpu4});
}

AI::IGomokuAI *create_helper_ai()
{
    return new AI::MinMaxV2::GomokuAI({4, 2, ai_data_cpu2});
}

AI::IGomokuAI *create_test_ai()
{
    return new AI::MinMaxV3::GomokuAI({4, 2, ai_data_cpu2});
}

static const std::vector<std::pair<std::string, std::function<AI::IGomokuAI *()>>>
    ai_makers = {
        {"easy", create_easy_ai},
        {"medium", create_medium_ai},
        {"hard", create_hard_ai},
        {"hardcore", create_hardcore_ai},
        {"ultra_hardcore", create_ultra_hardcore_ai},
        {"giga_hardcore", create_giga_hardcore_ai},
        {"deep7", create_deep7},
        {"deep8", create_deep8},
        {"deep9", create_deep9},
        {"deep10", create_deep10},
        {"deep", create_deep_ai},
        {"cpu1", create_cpu1_ai},
        {"cpu2", create_cpu2_ai},
        {"cpu3", create_cpu3_ai},
        {"cpu4", create_cpu4_ai},
        {"helper", create_helper_ai},
        {"test", create_test_ai},
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
