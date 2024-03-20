from app.gomoku_app import GomokuApp
from app.shared_object import SharedObject

from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer


import asyncio


def get_game() -> GomokuGame:
    return SharedObject.get_instance().get_game()


def display_moves(message, game: GomokuGame):
    move_string = ""
    for move in game.move_list:
        move_string += game.coordinates_name(move.row, move.column) + ","
    print(move_string[:-1])


async def get_AI_suggestion():
    gomoku_game = get_game()
    if gomoku_game is None:
        return

    loop = asyncio.get_running_loop()
    moveEvaluation = await loop.run_in_executor(None, gomoku_game.get_AI_suggestion)

    gomoku_game.play_at(moveEvaluation[0], moveEvaluation[1])


def handle_end_turn(message, game: GomokuGame):
    gomoku_game = get_game()
    if gomoku_game is None:
        return

    if gomoku_game.get_current_player() == GomokuPlayer.WHITE:
        asyncio.create_task(get_AI_suggestion())


async def main():

    CallbackCenter.shared().add_callback("GomokuGame.modified", display_moves)
    CallbackCenter.shared().add_callback("GomokuGame.endturn", handle_end_turn)
    app = GomokuApp()
    app_task = asyncio.ensure_future(app.async_run(async_lib="asyncio"))

    # Wait for the Kivy app to finish running
    await app_task

    # After the app is done, clean up the asyncio loop
    if not app_task.cancelled():
        app_task.cancel()
    await asyncio.sleep(0)


if __name__ == "__main__":
    asyncio.run(main())
