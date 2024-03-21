from kivy.uix.widget import Widget
from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer, GomokuMove
from app.shared_object import SharedObject
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty
from kivy.clock import Clock


class GameInfosWidget(Widget):

    time_marker_black = ObjectProperty(None)
    time_marker_white = ObjectProperty(None)

    score_marker_black = ObjectProperty(None)
    score_marker_white = ObjectProperty(None)

    button_previous = ObjectProperty(None)
    button_next = ObjectProperty(None)

    current_player: GomokuPlayer

    def __init__(self, **kwargs):
        super(GameInfosWidget, self).__init__(**kwargs)
        CallbackCenter.shared().add_callback(
            "GomokuGame.modified", self.on_game_modified
        )
        CallbackCenter.shared().add_callback(
            "GomokuGame.time", self.on_game_time_modified
        )

    def on_game_modified(self, message, game: GomokuGame):
        self.current_player = game.get_current_player()
        self.score_marker_black.text = str(game.get_player_score(GomokuPlayer.BLACK))
        self.score_marker_white.text = str(game.get_player_score(GomokuPlayer.WHITE))
        self.button_previous.disabled = not game.can_reverse_move()
        self.button_next.disabled = not game.can_reapply_move()

    def on_game_time_modified(self, message, game: GomokuGame):
        if game is None:
            self.time_marker_black.text = ""
            self.time_marker_white.text = ""
            return

        def time_to_string(time: float) -> str:
            return "{:.1f}".format(time)

        black_time = (
            game.players_time[GomokuPlayer.BLACK]
            + game.players_time_since_start_turn[GomokuPlayer.BLACK]
        )
        self.time_marker_black.text = time_to_string(black_time)
        white_time = (
            game.players_time[GomokuPlayer.WHITE]
            + game.players_time_since_start_turn[GomokuPlayer.WHITE]
        )
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
