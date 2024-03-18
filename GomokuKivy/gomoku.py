from app.gomoku_app import GomokuApp

from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame

def display_moves(message, game: GomokuGame):
    move_string = ""
    for move in game.move_list:
        move_string += game.coordinates_name(move.row, move.column) + ','
    print(move_string[:-1])

if __name__ == '__main__':
    CallbackCenter.shared().add_callback("GomokuGame.modified", display_moves)
    GomokuApp().run()
