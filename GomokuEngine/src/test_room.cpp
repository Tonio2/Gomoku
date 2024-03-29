#include "utils.h"
#include "room.h"

void test_room1()
{
    Room room = Room(19, 19, arePlayersHuman = [ true, false ], RuleStyle.STANDARD);

    // Send game id
    std::string room_id = room.get_id();

    // Ask action
    std::pair<Player, Action> action = room.get_action(); // [Player.Player1, Action.PLAY]

    // Other actions fails
    ActionResult result = room.action(Player.PLAYER2, Action.PLAY, {4, 5}); // result.success = false / result.message = "Player 1 is expected to play"
    result = room.action(Player.PLAYER1, Action.SWAP_CHOICE, {Team.Black}); // result.success = false / result.message = "Player 1 is expected to play"

    // Player 1 plays
    result = room.action(Player.PLAYER1, Action.Play, {4, 5}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
    if (!arePlayersHuman[result.next_action.first])
    {
        result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    }
}

void test_room2()
{
    Room room = Room(19, 19, arePlayersHuman = [ true, false ], RuleStyle.PRO);

    // Player 1 plays wrong
    ActionResult result = room.action(Player.PLAYER1, Action.Play, {4, 5}); // result.success = false / result.message = "Stone should be in the middle"
    result = room.action(Player.PLAYER1, Action.Play, {9, 9});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
    if (!arePlayersHuman[result.next_action.first])
    {
        result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    }

    result = room.action(Player.PLAYER1, Action.Play, {9, 8}); // result.success = false / result.message = "Stone should be 3 cells away from first stone"
}

void test_room3()
{
    Room room = Room(19, 19, arePlayersHuman = [ false, true ], RuleStyle.PRO);

    ActionResult result = room.action(Player.PLAYER1, Action.Play, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
    // Should be in the middle
    result = room.action(Player.PLAYER1, Action.Play, {9, 8}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    if (!arePlayersHuman[result.next_action.first])
    {
        result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.PLAY}
    }                                                                                  // Should be 3 cells away from first stone
}

void test_room4()
{
    Room room = Room(19, 19, arePlayersHuman = [ true, false ], RuleStyle.SWAP);

    ActionResult result = room.action(Player.PLAYER1, Action.Play, {4, 5}); // result.success = false / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    result = room.action(Player.PLAYER1, Action.PLAY, {9, 9});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    result = room.action(Player.PLAYER1, Action.PLAY, {9, 8});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.SWAP_CHOICE}
    if (!arePlayersHuman[result.next_action.first])
    {
        result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
        if (!arePlayersHuman[result.next_action.first])
        {
            result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
        }
    }
}

void test_room5()
{
    Room room = Room(19, 19, arePlayersHuman = [ false, true ], RuleStyle.SWAP);

    ActionResult result = room.action(Player.PLAYER1, Action.Play, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
    if (!arePlayersHuman[result.next_action.first])
    {
        result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
        if (!arePlayersHuman[result.next_action.first])
        {
            result = room.action(result.next_action.first, result.next_action.second, {}); // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER2, Action.SWAP_CHOICE}
        }
    }
    result = room.action(Player.PLAYER2, Action.SWAP_CHOICE, {Team.Black}); // result.success = true / result.message = "" / result.moveResult = {} / next_action = {Player.PLAYER2, Action.PLAY}
    result = room.action(Player.PLAYER2, Action.Play, {7, 9});              // result.success = true / result.message = "" / result.moveResult = {...} / next_action = {Player.PLAYER1, Action.PLAY}
}