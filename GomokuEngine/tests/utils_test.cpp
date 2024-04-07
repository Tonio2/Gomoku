#include "gtest/gtest.h"
#include "utils/gomoku_utilities.h"

TEST(GomokuUtilitiesTest, CharToCoordinate)
{
    EXPECT_EQ(char_to_coordinate('0'), 0);
    EXPECT_EQ(char_to_coordinate('9'), 9);
    EXPECT_EQ(char_to_coordinate('A'), 10);
    EXPECT_EQ(char_to_coordinate('Z'), 35);
}

TEST(GomokuUtilitiesTest, CoordinateToChar)
{
    EXPECT_EQ(coordinate_to_char(0), '0');
    EXPECT_EQ(coordinate_to_char(9), '9');
    EXPECT_EQ(coordinate_to_char(10), 'A');
    EXPECT_EQ(coordinate_to_char(35), 'Z');
}

TEST(GomokuUtilitiesTest, GameToString)
{
    GomokuGame game(7, 7);

    EXPECT_EQ(to_string(game),
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    );

    game.make_move(4, 4);

    EXPECT_EQ(to_string(game),
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    "....X..\n"
    ".......\n"
    ".......\n"
    );

    game.make_move(5, 5);

    EXPECT_EQ(to_string(game),
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    "....X..\n"
    ".....O.\n"
    ".......\n"
    );

    game.make_move(6, 0);

    EXPECT_EQ(to_string(game),
    ".......\n"
    ".......\n"
    ".......\n"
    ".......\n"
    "....X..\n"
    ".....O.\n"
    "X......\n"
    );

    game.make_move(0, 6);

    EXPECT_EQ(to_string(game),
    "......O\n"
    ".......\n"
    ".......\n"
    ".......\n"
    "....X..\n"
    ".....O.\n"
    "X......\n"
    );

    EXPECT_EQ(to_string(game, true),
    "  0 1 2 3 4 5 6\n"
    "0 . . . . . . O\n"
    "1 . . . . . . .\n"
    "2 . . . . . . .\n"
    "3 . . . . . . .\n"
    "4 . . . . X . .\n"
    "5 . . . . . O .\n"
    "6 X . . . . . .\n"
    );

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
