from kivy.uix.widget import Widget
from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer, GomokuMove
from app.shared_object import SharedObject

class GameInfosWidget(Widget):

    current_player: GomokuPlayer

    def __init__(self, **kwargs):
        super(GameInfosWidget, self).__init__(**kwargs)
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_game_modified)

    def on_game_modified(self, message, game: GomokuGame):
        self.current_player = game.get_current_player()

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def reverse_move(self):
        game = self.get_game()

        if game is None:
            return
        
        game.reverse_last_move()

    def reapply_move(self):
        game = self.get_game()

        if game is None:
            return
        
        game.reapply_last_move()
