#include "gtest/gtest.h"
#include "engine/gomoku_engine.h"

TEST(EngineTest, Creation)
{
    GomokuGame game(10, 10);

    EXPECT_EQ(game.get_board_width(), 10);
    EXPECT_EQ(game.get_board_height(), 10);
}

TEST(EngineTest, ValidCoordinates)
{
    GomokuGame game(10, 10);

    EXPECT_FALSE(game.coordinates_are_valid(-1, -1));
    EXPECT_FALSE(game.coordinates_are_valid(10, -1));
    EXPECT_FALSE(game.coordinates_are_valid(-1, 10));
    EXPECT_FALSE(game.coordinates_are_valid(10, 10));
    EXPECT_TRUE(game.coordinates_are_valid(0, 0));
    EXPECT_TRUE(game.coordinates_are_valid(9, 0));
    EXPECT_TRUE(game.coordinates_are_valid(0, 9));
    EXPECT_TRUE(game.coordinates_are_valid(9, 9));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
