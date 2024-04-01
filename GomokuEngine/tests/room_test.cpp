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
// void test_room4()
// {
//     Room room = Room(19, 19, arePlayersHuman = [ true, false ], RuleStyle.SWAP);

//     ActionResult result = room.action(Player.PLAYER1, Action.Play, {4, 5}); // result.success = false / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
//     result = room.action(Player.PLAYER1, Action.PLAY, {9, 9});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
//     result = room.action(Player.PLAYER1, Action.PLAY, {9, 8});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.SWAP_CHOICE}
//     if (!arePlayersHuman[result.next_action.first])
//     {
//         result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
//         if (!arePlayersHuman[result.next_action.first])
//         {
//             result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
//         }
//     }
// }

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
