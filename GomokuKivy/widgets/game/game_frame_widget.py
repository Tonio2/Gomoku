from kivy.uix.widget import Widget
from kivy.uix.label import Label
from kivy.graphics import Color, Rectangle, Ellipse
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from app.shared_object import SharedObject
from core.gomoku_room import GameRoom, GomokuPlayer

from core.callback_center import CallbackCenter

BOARD_FRAME_COLOR = (0.5, 0.3, 0.1)

class GameFrameWidget(Widget):

    board_widget = ObjectProperty(None)
    margin = NumericProperty(None)

    def __init__(self, **kwargs):
        super(GameFrameWidget, self).__init__(**kwargs)
        Window.bind(size=self._on_window_resized)
        CallbackCenter.shared().add_callback("Application.draw", self.on_application_draw)

    def __del__(self):
        Window.unbind(size=self._on_window_resized)
        CallbackCenter.shared().remove_callback("Application.draw", self.on_application_draw)

    def _on_window_resized(self, window, size):
        Clock.schedule_once(lambda _ : self.draw_board_frame(), 0.1)

    def get_room(self) -> GameRoom:
        return SharedObject.get_instance().get_room()

    def on_application_draw(self, _, __):
        self.draw_board_frame()
        Clock.schedule_once(lambda _ : self.get_room().perform_pending_actions(), 0.1)

    def update_widget_layouts(self):

        room = self.get_room()
        game_ratio = room.get_board_width() / room.get_board_height()
        frame_ratio = self.parent.width / self.parent.height

        if game_ratio > frame_ratio:
            self.size = self.parent.width, self.parent.width / game_ratio
        else:
            self.size = self.parent.height * game_ratio, self.parent.height

        self.pos[0] = self.parent.pos[0] + (self.parent.width - self.width) / 2
        self.pos[1] = self.parent.pos[1] + (self.parent.height - self.height) / 2

        self.board_widget.size = self.width - 2 * self.margin, self.height - 2 * self.margin
        self.board_widget.pos = self.pos[0] + self.margin, self.pos[1] + self.margin

    def draw_board_frame(self):
        self.update_widget_layouts()

        self.board_widget.draw_board()

        room = self.get_room()
        board_size_y, board_size_x = room.get_board_height(), room.get_board_width()
        cell_size_x = self.board_widget.width / board_size_x
        cell_size_y = self.board_widget.height / board_size_y

        CallbackCenter.shared().remove_all_callbacks("Board.mouse")
        self.clear_widgets()

        for x in range(board_size_x):
            coordinate_label = BoardCoordinateLabel(text=room.coordinate_index_name(x))
            coordinate_label.index = x
            coordinate_label.horizontal = True
            coordinate_label.pos = self.board_widget.pos[0] + x * cell_size_x, self.board_widget.pos[1] + self.board_widget.size[1]
            coordinate_label.size = cell_size_x, 20
            self.add_widget(coordinate_label)

        for y in range(board_size_y):
            coordinate_label = BoardCoordinateLabel(text=room.coordinate_index_name(board_size_y - y - 1))
            coordinate_label.index = y
            coordinate_label.horizontal = False
            coordinate_label.pos = self.board_widget.pos[0] - 20, self.board_widget.pos[1] + y * cell_size_y
            coordinate_label.size = 20, cell_size_y
            self.add_widget(coordinate_label)


HOVERED_COLOR = (0.3, 0.3, 0.3)
UNHOVERED_COLOR = (0.1, 0.1, 0.1)
class BoardCoordinateLabel(Label):

    index: int
    horizontal: bool

    def __init__(self, **kwargs):
        super(BoardCoordinateLabel, self).__init__(**kwargs)
        self.font_size = 14
        self.color = UNHOVERED_COLOR
        CallbackCenter.shared().add_callback("Board.mouse", self.on_mouse_board)

    def __del__(self):
        CallbackCenter.shared().remove_callback("Board.mouse", self.on_mouse_board)

    def on_mouse_board(self, _, cell):
        if self.horizontal:
            if cell.col == self.index:
                self.color = HOVERED_COLOR
            else:
                self.color = UNHOVERED_COLOR
        else:
            if cell.row == self.index:
                self.color = HOVERED_COLOR
            else:
                self.color = UNHOVERED_COLOR
