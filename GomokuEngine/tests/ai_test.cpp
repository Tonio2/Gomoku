#include "gtest/gtest.h"
#include "ai/gomoku_ai.h"
#include "engine/gomoku_engine.h"

TEST(AiTest, Creation)
{
    GomokuAiSettings settings;
    settings.depth = 3;
    settings.length = 2;
    settings.data = GomokuAIData();

    GomokuAI ai(settings);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
