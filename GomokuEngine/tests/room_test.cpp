#include "gtest/gtest.h"
#include "room/game_room.h"

TEST(RoomTest, Creation)
{
    GameRoomSettings settings;
    settings.width = 10;
    settings.height = 10;
    GameRoom room(settings);

    EXPECT_EQ(room.get_settings().width, 10);
    EXPECT_EQ(room.get_settings().height, 10);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
