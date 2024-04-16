from kivy.uix.widget import Widget
from core.callback_center import CallbackCenter
from core.gomoku_room import GameRoom, GomokuPlayer, GomokuMove
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
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_game_modified)
        CallbackCenter.shared().add_callback("GomokuGame.time", self.on_game_time_modified)

    def on_game_modified(self, _, room: GameRoom):
        self.current_player = room.get_current_player()
        self.score_marker_black.text = str(room.get_player_score(GomokuPlayer.BLACK))
        self.score_marker_white.text = str(room.get_player_score(GomokuPlayer.WHITE))
        self.button_previous.disabled = not room.can_reverse_move()
        self.button_next.disabled = not room.can_reapply_move()

    def on_game_time_modified(self, _, room: GameRoom):
        if room is None:
            self.time_marker_black.text = ""
            self.time_marker_white.text = ""
            return

        def time_to_string(time: float) -> str:
            return "{:.1f}".format(time)

        black_time = room.get_player_time(GomokuPlayer.BLACK)
        self.time_marker_black.text = time_to_string(black_time)
        white_time = room.get_player_time(GomokuPlayer.WHITE)
        self.time_marker_white.text = time_to_string(white_time)

    def get_room(self) -> GameRoom:
        return SharedObject.get_instance().get_room()

    def reverse_move(self):
        room = self.get_room()

        if room is None:
            return

        room.reverse_last_move()

    def reapply_move(self):
        room = self.get_room()

        if room is None:
            return

        room.reverse_last_move()

