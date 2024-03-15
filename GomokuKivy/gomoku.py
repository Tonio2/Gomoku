from app.gomoku_app import GomokuApp

from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame

coordinates = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']

def display_moves(message, game: GomokuGame):
    move_string = ""
    for move in game.move_list:
        move_string += coordinates[move.row] + coordinates[move.column] + ','
    print(move_string[:-1])

if __name__ == '__main__':
    CallbackCenter.shared().add_callback("GomokuGame.modified", display_moves)
    GomokuApp().run()
