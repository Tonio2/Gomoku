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

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
