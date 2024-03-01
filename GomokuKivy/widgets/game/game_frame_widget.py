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

    def __init__(self, **kwargs):
        super(GameFrameWidget, self).__init__(**kwargs)

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def draw_frame(self):
        pass
