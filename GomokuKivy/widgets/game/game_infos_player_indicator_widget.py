from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, Ellipse
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from app.shared_object import SharedObject
from core.gomoku_room import GameRoom, GomokuPlayer

from core.callback_center import CallbackCenter

WHITE_COLOR = (0.9, 0.9, 0.9)
BLACK_COLOR = (0.1, 0.1, 0.1)

class GameInfosPlayerIndicatorWidget(Widget):

    player = ObjectProperty(None)

    def __init__(self, **kwargs):
        super(GameInfosPlayerIndicatorWidget, self).__init__(**kwargs)
        Window.bind(size=self._on_window_resized)
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_gomokugame_modified)
        CallbackCenter.shared().add_callback("Application.draw", self.on_application_draw)

    def _on_window_resized(self, window, size):
        Clock.schedule_once(lambda _ : self.draw(), 0.1)

    def on_application_draw(self, _, __):
        self.draw()

    def get_room(self) -> GameRoom:
        return SharedObject.get_instance().get_room()

    def on_gomokugame_modified(self, _, room: GameRoom):
        if room == self.get_room():
            self.draw()

    def draw(self):
        room = self.get_room()
        if room is None:
            return

        gomoku_player = GomokuPlayer(self.player)
        player_color = WHITE_COLOR if gomoku_player == GomokuPlayer.WHITE else BLACK_COLOR

        my_turn = room.get_current_player() == gomoku_player

        wf, hf = (1, 1) if my_turn else (0.6, 0.6)
        width, height = wf * self.width, hf * self.height
        size = min(width, height)

        self.canvas.clear()
        with self.canvas:
            Color(*player_color)
            Ellipse(
                pos=(self.x + (self.width - size) / 2, self.y + (self.height - size) / 2),
                size=(size, size)
            )
