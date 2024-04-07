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

    // Player 2 plays incorrectly
    action_result = room.perform_action_move(2, 13, 13);

    EXPECT_FALSE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 1);

    // Player 2 plays correctly
    action_result = room.perform_action_move(2, 10, 9);

    EXPECT_TRUE(action_result.success);
    EXPECT_EQ(room.get_actions_history().size(), 2);

    // Player 1 plays too far from first stones
    action_result = room.perform_action_move(1, 15, 15);

    EXPECT_FALSE(action_result.success);

}

TEST(RoomRuleStyleSwapTest, PerformActions)
{
    GameRoomSettings settings;
    settings.rule_style = ::SWAP;
    settings.p2.is_ai = false;
    GameRoom room(settings);

    room.perform_action_move(1, 4, 5);

    room.perform_action_move(2, 5, 6);

}

TEST(RoomRuleStyleSwapTest, FullGameTry)
{
    GameRoomSettings settings;
    settings.rule_style = ::SWAP;
    settings.p2.is_ai = false;
    GameRoom room(settings);

    GameActionResult act;
    EXPECT_EQ(to_string(room.get_game(), true),
    "  0 1 2 3 4 5 6 7 8 9 A B C D E F G H I\n"
    "0 . . . . . . . . . . . . . . . . . . .\n"
    "1 . . . . . . . . . . . . . . . . . . .\n"
    "2 . . . . . . . . . . . . . . . . . . .\n"
    "3 . . . . . . . . . . . . . . . . . . .\n"
    "4 . . . . . . . . . . . . . . . . . . .\n"
    "5 . . . . . . . . . . . . . . . . . . .\n"
    "6 . . . . . . . . . . . . . . . . . . .\n"
    "7 . . . . . . . . . . . . . . . . . . .\n"
    "8 . . . . . . . . . . . . . . . . . . .\n"
    "9 . . . . . . . . . . . . . . . . . . .\n"
    "A . . . . . . . . . . . . . . . . . . .\n"
    "B . . . . . . . . . . . . . . . . . . .\n"
    "C . . . . . . . . . . . . . . . . . . .\n"
    "D . . . . . . . . . . . . . . . . . . .\n"
    "E . . . . . . . . . . . . . . . . . . .\n"
    "F . . . . . . . . . . . . . . . . . . .\n"
    "G . . . . . . . . . . . . . . . . . . .\n"
    "H . . . . . . . . . . . . . . . . . . .\n"
    "I . . . . . . . . . . . . . . . . . . .\n"
    );

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

    // Not player1 turn
    act = room.perform_action_move(1, 0, 0);
    ASSERT_FALSE(act.success);

    act = room.perform_action_move(2, 7, 6);
    ASSERT_TRUE(act.success);

    // Not player2 turn
    act = room.perform_action_move(2, 6, 5);
    ASSERT_FALSE(act.success);


}

// void test_room5()
// {
//     Room room = Room(19, 19, arePlayersHuman = [ false, true ], RuleStyle.SWAP);

//     ActionResult result = room.action(Player.PLAYER1, Action.Play, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
//     if (!arePlayersHuman[result.next_action.first])
//     {
//         result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
//         if (!arePlayersHuman[result.next_action.first])
//         {
//             result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.SWAP_CHOICE}
//         }
//     }
//     result = room.action(Player.PLAYER2, Action.SWAP_CHOICE, {Team.Black}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
//     result = room.action(Player.PLAYER2, Action.Play, {7, 9});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
// }

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
