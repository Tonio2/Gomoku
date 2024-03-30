#include "test_room.h"

void test_rooms()
{
    test_room1();
    test_room2();
}

void test_room1()
{
    GameRoomSettings settings;

    GameRoom room(settings);

    std::string room_id = room.get_id();

    {
        GameActionResult action_result = room.perform_action_move(1, 9, 9);

        std::cout << "Tried to succeed an easy action: " << action_result.success << "-" << action_result.message << std::endl;
    }

    std::cout << "Room has pending actions: " << room.has_pending_action() << std::endl;

    room.perform_pending_action();

    std::cout << "Room did pending actions... Have none left: " << room.has_pending_action() << std::endl;

    {
        GameActionResult action_result = room.perform_action_move(1, 9, 9);

        std::cout << "Tried to failed a similar action: " << action_result.success << "-" << action_result.message << std::endl;
    }

    {
        GameActionResult action_result = room.perform_action_move(1, 10, 9);

        std::cout << "Tried to succeed another action: " << action_result.success << "-" << action_result.message << std::endl;
    }
}

void test_room2()
{
    GameRoomSettings settings;
    settings.rule_style = GameRoomRuleStyle::PRO;
    GameRoom room(settings);

    GameActionResult result;

    // Player 1 plays wrong
    result = room.perform_action_move(1, 4, 5);

    // result.success = false / result.message = "Stone should be in the middle"

    result = room.perform_action_move(1, 9, 9);
    // result.success = true

    room.perform_pending_action();

    room.perform_action_move(1, 15, 15);
    // result.success = false / result.message = "Stone should be 3 cells away from first stone"

    room.perform_action_move(1, 8, 10);
    // result.success = false
}

// void test_room3()
// {
//     Room room = Room(19, 19, arePlayersHuman = [ false, true ], RuleStyle.PRO);

//     ActionResult result = room.action(Player.PLAYER1, Action.Play, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
//     // Should be in the middle
//     result = room.action(Player.PLAYER1, Action.Play, {9, 8}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
//     if (!arePlayersHuman[result.next_action.first])
//     {
//         result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
//     }                                                                                  // Should be 3 cells away from first stone
// }

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