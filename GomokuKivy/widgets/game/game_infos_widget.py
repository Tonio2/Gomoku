from kivy.uix.widget import Widget
from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer, GomokuMove
from app.shared_object import SharedObject
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.clock import Clock


class GameInfosWidget(Widget):

    time_marker_black = ObjectProperty(None)
    time_marker_white = ObjectProperty(None)
    current_player: GomokuPlayer

    def __init__(self, **kwargs):
        super(GameInfosWidget, self).__init__(**kwargs)
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_game_modified)
        CallbackCenter.shared().add_callback("GomokuGame.time", self.on_game_time_modified)
        Clock.schedule_interval(self.tick_game_time, 1/30)

    def tick_game_time(self, dt):
        game = self.get_game()
        
        if game is None:
            return
        
        game.increase_time(dt)

    def on_game_modified(self, message, game: GomokuGame):
        self.current_player = game.get_current_player()

    def on_game_time_modified(self, message, game: GomokuGame):
        if game is None:
            self.time_marker_black.text = ""
            self.time_marker_white.text = ""
            return
        
        def time_to_string(time: float) -> str:
            return "{:.1f}".format(time)

        black_time = game.players_time[GomokuPlayer.BLACK]
        self.time_marker_black.text = time_to_string(black_time)
        white_time = game.players_time[GomokuPlayer.WHITE]
        self.time_marker_white.text = time_to_string(white_time)

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
        
        game.reverse_last_move()

    def reapply_move(self):
        game = self.get_game()

        if game is None:
            return
        
        game.reapply_last_move()
