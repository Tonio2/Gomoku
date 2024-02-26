from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.core.window import Window

from widgets.gomoku_board_widget import GomokuBoardWidget
from widgets.game_infos_widget import GameInfosWidget

class RootWidget(Widget):
    board = ObjectProperty(None)

    def __init__(self, **kwargs):
        super(RootWidget, self).__init__(**kwargs)
        self._keyboard = Window.request_keyboard(self._on_keyboard_closed, self)
        self._keyboard.bind(on_key_down=self._on_keyboard_down)
        Window.bind(size=self._on_window_resized)

    def _on_window_resized(self, window, size):
        self.board.draw_board()

    def _on_keyboard_closed(self):
        self._keyboard.unbind(on_key_down=self._on_keyboard_down)
        self._keyboard = None

    def _on_keyboard_down(self, keyboard, keycode, text, modifiers):
        return False
