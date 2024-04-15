#include "gtest/gtest.h"
#include "room/game_room.h"
#include "utils/gomoku_utilities.h"

TEST(RoomTest, Creation)
{
    GameRoomSettings settings;
    settings.width = 10;
    settings.height = 10;
    GameRoom room(settings);

    EXPECT_EQ(room.get_settings().width, 10);
    EXPECT_EQ(room.get_settings().height, 10);
}

TEST(RoomTest, MakeDifferentIds)
{
    GameRoomSettings settings;
    GameRoom room1(settings);
    GameRoom room2(settings);
    GameRoom room3(settings);

    EXPECT_NE(room1.get_id(), room2.get_id());
    EXPECT_NE(room1.get_id(), room3.get_id());
    EXPECT_NE(room2.get_id(), room3.get_id());
}

TEST(RoomTest, PerformActions)
{
    GameRoomSettings settings;
    GameRoom room(settings);
    GameActionResult action_result;

    EXPECT_EQ(room.get_actions_history().size(), 0);

    // Perform valid action with correct player
    action_result = room.perform_action_move(1, 9, 9);

    EXPECT_TRUE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 1);
    EXPECT_TRUE(room.has_pending_action());

    EXPECT_NO_THROW({
        room.perform_pending_action();
    });

    EXPECT_EQ(room.get_actions_history().size(), 2);

    // Perform invalid action with correct player
    action_result = room.perform_action_move(1, 9, 9);

    EXPECT_FALSE(action_result.success);

    action_result = room.perform_action_move(1, 10, 9);

    EXPECT_TRUE(action_result.success);
}

TEST(RoomRuleStyleProTest, PerformActions)
{
    GameRoomSettings settings;
    settings.rule_style = GameRoomRuleStyle::PRO;
    settings.p2.is_ai = false;
    GameRoom room(settings);

    GameActionResult action_result;

    // Player 1 plays wrong
    action_result = room.perform_action_move(1, 4, 5);

    EXPECT_FALSE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 0);

    // Player 1 plays correctly
    action_result = room.perform_action_move(1, 9, 9);

    EXPECT_TRUE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 1);

    // Player 2 plays correctly
    action_result = room.perform_action_move(2, 10, 10);

    EXPECT_TRUE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 2);

    // Player 1 plays inccorectly in inner circle
    action_result = room.perform_action_move(1, 9, 10);

    EXPECT_FALSE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 2);

    // Player 1 plays correctly
    action_result = room.perform_action_move(1, 5, 8);

    EXPECT_TRUE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 3);

    EXPECT_EQ(to_string(room.get_game(), true),
              "  0 1 2 3 4 5 6 7 8 9 A B C D E F G H I\n"
              "0 . . . . . . . . . . . . . . . . . . .\n"
              "1 . . . . . . . . . . . . . . . . . . .\n"
              "2 . . . . . . . . . . . . . . . . . . .\n"
              "3 . . . . . . . . . . . . . . . . . . .\n"
              "4 . . . . . . . . . . . . . . . . . . .\n"
              "5 . . . . . . . . X . . . . . . . . . .\n"
              "6 . . . . . . . . . . . . . . . . . . .\n"
              "7 . . . . . . . . . . . . . . . . . . .\n"
              "8 . . . . . . . . . . . . . . . . . . .\n"
              "9 . . . . . . . . . X . . . . . . . . .\n"
              "A . . . . . . . . . . O . . . . . . . .\n"
              "B . . . . . . . . . . . . . . . . . . .\n"
              "C . . . . . . . . . . . . . . . . . . .\n"
              "D . . . . . . . . . . . . . . . . . . .\n"
              "E . . . . . . . . . . . . . . . . . . .\n"
              "F . . . . . . . . . . . . . . . . . . .\n"
              "G . . . . . . . . . . . . . . . . . . .\n"
              "H . . . . . . . . . . . . . . . . . . .\n"
              "I . . . . . . . . . . . . . . . . . . .\n");
}

TEST(RoomRuleStyleSwapTest, FullGameTry)
{
    GameRoomSettings settings;
    settings.rule_style = GameRoomRuleStyle::SWAP;
    settings.p2.is_ai = false;
    GameRoom room(settings);

    GameActionResult act;

    // Not player2 turn
    act = room.perform_action_move(2, 4, 5);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(1, 4, 5);
    ASSERT_TRUE(act.success);

    // Not player2 turn
    act = room.perform_action_move(2, 9, 9);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(1, 9, 9);
    ASSERT_TRUE(act.success);

    // Not player2 turn
    act = room.perform_action_move(2, 9, 8);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(1, 9, 8);
    ASSERT_TRUE(act.success);

    // Not player1 turn and a swap is expected
    act = room.perform_action_move(1, 12, 12);
    ASSERT_FALSE(act.success);

    // a swap is expected
    act = room.perform_action_move(2, 12, 12);
    ASSERT_FALSE(act.success);

    // Not up to player 1 to choose
    act = room.perform_action_swap(1, false);
    ASSERT_FALSE(act.success);

    act = room.perform_action_swap(2, true);
    ASSERT_TRUE(act.success);

    // Not player2 turn
    act = room.perform_action_move(2, 7, 6);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(1, 0, 0);
    ASSERT_TRUE(act.success);

    // Not player1 turn
    act = room.perform_action_move(1, 1, 1);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(2, 6, 5);
    ASSERT_TRUE(act.success);

    EXPECT_EQ(to_string(room.get_game(), true),
              "  0 1 2 3 4 5 6 7 8 9 A B C D E F G H I\n"
              "0 O . . . . . . . . . . . . . . . . . .\n"
              "1 . . . . . . . . . . . . . . . . . . .\n"
              "2 . . . . . . . . . . . . . . . . . . .\n"
              "3 . . . . . . . . . . . . . . . . . . .\n"
              "4 . . . . . X . . . . . . . . . . . . .\n"
              "5 . . . . . . . . . . . . . . . . . . .\n"
              "6 . . . . . X . . . . . . . . . . . . .\n"
              "7 . . . . . . . . . . . . . . . . . . .\n"
              "8 . . . . . . . . . . . . . . . . . . .\n"
              "9 . . . . . . . . X O . . . . . . . . .\n"
              "A . . . . . . . . . . . . . . . . . . .\n"
              "B . . . . . . . . . . . . . . . . . . .\n"
              "C . . . . . . . . . . . . . . . . . . .\n"
              "D . . . . . . . . . . . . . . . . . . .\n"
              "E . . . . . . . . . . . . . . . . . . .\n"
              "F . . . . . . . . . . . . . . . . . . .\n"
              "G . . . . . . . . . . . . . . . . . . .\n"
              "H . . . . . . . . . . . . . . . . . . .\n"
              "I . . . . . . . . . . . . . . . . . . .\n");
}

TEST(RoomRuleStyleSwapTest, TryNotSwaping)
{
    GameRoomSettings settings;
    settings.rule_style = GameRoomRuleStyle::SWAP;
    settings.width = 7;
    settings.height = 7;
    GameRoom room(settings);

    GameActionResult r;

    r = room.perform_action_move(1, 0, 0);
    ASSERT_TRUE(r.success);

    r = room.perform_action_move(1, 2, 3);
    ASSERT_TRUE(r.success);

    r = room.perform_action_move(1, 3, 3);
    ASSERT_TRUE(r.success);

    r = room.perform_action_swap(2, false);
    ASSERT_TRUE(r.success);

    r = room.perform_action_move(2, 3, 4);
    ASSERT_TRUE(r.success);

    r = room.perform_action_move(1, 4, 4);
    ASSERT_TRUE(r.success);

    ASSERT_EQ(to_string(room.get_game()),
              "X......\n"
              ".......\n"
              "...O...\n"
              "...XO..\n"
              "....X..\n"
              ".......\n"
              ".......\n");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
