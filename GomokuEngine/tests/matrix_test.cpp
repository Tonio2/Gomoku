#include "gtest/gtest.h"
#include "matrix/Matrix.hpp"

TEST(MatrixTest, Size)
{
    Matrix<int> m(5, 5);

    EXPECT_EQ(m.get_width(), 5);
    EXPECT_EQ(m.get_height(), 5);
}

TEST(MatrixTest, Values)
{
    Matrix<int> m(5, 5);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            m(i, j) = i * 5 + j;
            EXPECT_EQ(m(i, j), i * 5 + j);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
