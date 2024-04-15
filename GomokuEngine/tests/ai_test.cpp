#include "gtest/gtest.h"
#include "ai/gomoku_ai.h"
#include "engine/gomoku_engine.h"

TEST(AiTest, Creation)
{
    GomokuAI ai(3);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}