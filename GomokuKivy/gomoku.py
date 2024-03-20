from app.gomoku_app import GomokuApp
from app.shared_object import SharedObject

from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer

def get_game() -> GomokuGame:
        return SharedObject.get_instance().get_game()

def display_moves(message, game: GomokuGame):
    move_string = ""
    for move in game.move_list:
        move_string += game.coordinates_name(move.row, move.column) + ','
    print(move_string[:-1])
    
def get_AI_suggestion(message, game: GomokuGame):
    gomoku_game = get_game()
    if gomoku_game is None:
        return
    
    return gomoku_game.get_AI_suggestion()

def handle_end_turn(message, game: GomokuGame):
    gomoku_game = get_game()
    if gomoku_game is None:
        return
    
    if gomoku_game.get_current_player() == GomokuPlayer.WHITE:
        move = get_AI_suggestion(message, game)
        gomoku_game.play_at(move[0], move[1])

if __name__ == '__main__':
    CallbackCenter.shared().add_callback("GomokuGame.modified", display_moves)
    CallbackCenter.shared().add_callback("GomokuGame.endturn", handle_end_turn)
    GomokuApp().run()
