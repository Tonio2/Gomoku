from app.gomoku_app import GomokuApp

from core.callback_center import CallbackCenter
from core.gomoku_room import GameRoom


import asyncio


def display_moves(message, game: GameRoom):
    move_string = ""
    for move in game.move_list:
        move_string += game.coordinates_name(move.row, move.column) + ","
    print(move_string[:-1])


async def main():

    CallbackCenter.shared().add_callback("GomokuGame.modified", display_moves)
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
