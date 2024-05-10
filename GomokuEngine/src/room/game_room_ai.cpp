#include "room/game_room.h"
#include "ai/gomoku_ai.h"

static const GomokuAIData ai_data_default = GomokuAIData();

static const GomokuAIData ai_data_cpu1 = []()
{
    GomokuAIData data;
    data.values[0] = 0;
    data.values[1] = 2013470;
    data.values[2] = 7.8;
    data.values[3] = 487;
    data.values[4] = 0;
    data.values[5] = 0;
    data.values[6] = 4639;
    data.values[7] = 18.8;
    data.values[8] = 8538;
    data.values[9] = 353;
    data.values[10] = 565599;
    data.values[11] = 4545;
    data.values[12] = 2.8;
    data.values[13] = 0.5;
    data.values[14] = 0.1;
    return data;
}();

static const GomokuAIData ai_data_cpu2 = []()
{
    GomokuAIData data;
    data.values[0] = 0;
    data.values[1] = 1.18881e+09;
    data.values[2] = 175.305;
    data.values[3] = 0.488359;
    data.values[4] = 22.639;
    data.values[5] = 1.59943;
    data.values[6] = 375669;
    data.values[7] = 21.7746;
    data.values[8] = 5684.55;
    data.values[9] = 16355.2;
    data.values[10] = 7501.18;
    data.values[11] = 758.142;
    data.values[12] = 0.00172756;
    data.values[13] = 7.01941;
    data.values[14] = 74.5009;
    return data;
}();

static const GomokuAIData ai_data_cpu3 = []()
{
    GomokuAIData data;
    data.values[0] = 0;
    data.values[1] = 322.666;
    data.values[2] = 277.998;
    data.values[3] = 9.73701;
    data.values[4] = 5.72728;
    data.values[5] = 163.257;
    data.values[6] = 175.367;
    data.values[7] = 241.945;
    data.values[8] = 4736.6;
    data.values[9] = 2739.88;
    data.values[10] = 2588.87;
    data.values[11] = 1087.6;
    data.values[12] = 26.7119;
    data.values[13] = 5.10727;
    data.values[14] = 0.118801;
    return data;
}();

static const GomokuAIData ai_data_cpu4 = []()
{
    GomokuAIData data;
    data.values[0] = 0;
    data.values[1] = 911878;
    data.values[2] = 417;
    data.values[3] = 2153;
    data.values[4] = 1.5;
    data.values[5] = 4.7;
    data.values[6] = 37494;
    data.values[7] = 423;
    data.values[8] = 44508;
    data.values[9] = 1967;
    data.values[10] = 12800;
    data.values[11] = 5954;
    data.values[12] = 9.1;
    data.values[13] = 0.1;
    data.values[14] = 1.2;
    return data;
}();

static const std::map<std::string, GomokuAiSettings>
    ai_prefabs = {
        {"easy", {1, 1, ai_data_default}},
        {"medium", {2, 1, ai_data_default}},
        {"hard", {3, 2, ai_data_default}},
        {"deep", {5, 1, ai_data_default}},
        {"cpu1", {3, 2, ai_data_cpu1}},
        {"cpu2", {3, 2, ai_data_cpu2}},
        {"cpu3", {3, 2, ai_data_cpu3}},
        {"cpu4", {3, 2, ai_data_cpu4}},
};

static const std::vector<std::string> ai_names = {
    "easy",
    "medium",
    "hard",
    "deep",
    "cpu1",
    "cpu2",
    "cpu3",
    "cpu4",
};

GomokuAI *GameEntitySetting::make_ai() const
{
    if (!is_ai)
        return nullptr;

    auto it = ai_prefabs.find(ai_name);
    if (it == ai_prefabs.end())
        return nullptr;

    return new GomokuAI(it->second);
}

std::vector<std::string> get_ai_names_list()
{
    return ai_names;
}
