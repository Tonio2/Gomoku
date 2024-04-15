from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, Ellipse
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from app.shared_object import SharedObject
from core.gomoku_game import GomokuGame, GomokuPlayer

from core.callback_center import CallbackCenter

GRID_COLOR = (0, 0, 0)
LIGHT_COLOR = (0.7, 0.7, 0.7)
DARK_COLOR = (0.5, 0.5, 0.5)

WHITE_COLOR = (0.9, 0.9, 0.9)
BLACK_COLOR = (0.1, 0.1, 0.1)

class GameFrameWidget(Widget):

    board_widget = ObjectProperty(None)
    margin = NumericProperty(None)

    def __init__(self, **kwargs):
        super(GameFrameWidget, self).__init__(**kwargs)
        Window.bind(size=self._on_window_resized)
        CallbackCenter.shared().add_callback("Application.draw", self.on_application_draw)

    def _on_window_resized(self, window, size):
        Clock.schedule_once(lambda _ : self.draw_frame(), 0.1)

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def on_application_draw(self, message, _):
        self.draw_frame()

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def draw_frame(self):
        self.size = min(self.parent.width, self.parent.height), min(self.parent.width, self.parent.height)
        self.pos = self.parent.pos[0] + (self.parent.width - self.width) / 2, self.parent.pos[1] + (self.parent.height - self.height) / 2

        self.board_widget.size = self.width - self.margin, self.height - self.margin
        game = self.get_game()
        if game is not None:
            if game.get_board_width() > game.get_board_height():
                self.board_widget.height *= game.get_board_height() / game.get_board_width()
            else:
                self.board_widget.width *= game.get_board_width() / game.get_board_height()

        self.board_widget.pos[0] = self.pos[0] + (self.width - self.board_widget.width) / 2
        self.board_widget.pos[1] = self.pos[1] + (self.height - self.board_widget.height) / 2
        self.board_widget.draw_board()
