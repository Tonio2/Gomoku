#include "gtest/gtest.h"
#include "ai/gomoku_ai_minmaxv2.h"
#include "engine/gomoku_engine.h"

TEST(AiTest_MinMaxV2, Creation)
{
    AI::MinMaxV2::GomokuAiSettings settings;
    settings.depth = 3;
    settings.length = 2;
    settings.data = AI::MinMaxV2::GomokuAIData();

    AI::MinMaxV2::GomokuAI ai(settings);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
