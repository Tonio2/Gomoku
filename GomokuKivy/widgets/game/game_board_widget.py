from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, Ellipse
from kivy.clock import Clock
from kivy.core.window import Window

from app.shared_object import SharedObject
from core.gomoku_game import GomokuGame, GomokuPlayer

from core.callback_center import CallbackCenter

GRID_COLOR = (0, 0, 0)
LIGHT_COLOR = (0.7, 0.7, 0.7)
DARK_COLOR = (0.5, 0.5, 0.5)

WHITE_COLOR = (0.9, 0.9, 0.9)
BLACK_COLOR = (0.1, 0.1, 0.1)

class GameBoardWidget(Widget):

    def __init__(self, **kwargs):
        super(GameBoardWidget, self).__init__(**kwargs)
        Window.bind(size=self._on_window_resized)
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_gomokugame_modified)
        CallbackCenter.shared().add_callback("Application.draw", self.on_application_draw)

    def _on_window_resized(self, window, size):
        Clock.schedule_once(lambda _ : self.draw_board(), 0.1)

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def on_gomokugame_modified(self, message, game):
        if game == self.get_game():
            self.draw_board()

    def on_application_draw(self, message, _):
        self.draw_board()

    def draw_board(self):
        self.canvas.clear()

        gomoku_game = self.get_game()
        if gomoku_game is None:
            return

        board_size_y, board_size_x = gomoku_game.get_board_height(), gomoku_game.get_board_width()

        cell_size_x = self.width / board_size_x
        cell_size_y = self.height / board_size_y

        with self.canvas:
            # Draw the squares
            for x in range(board_size_x):
                for y in range(board_size_y):
                    square_color = LIGHT_COLOR if (x + y) % 2 == 0 else DARK_COLOR
                    Color(*square_color)
                    Rectangle(pos=(self.x + x * cell_size_x, self.y + y * cell_size_y),
                            size=(cell_size_x, cell_size_y))

            # # Draw board grid
            Color(*GRID_COLOR)
            for x in range(board_size_x):
                Rectangle(pos=(self.x + x * cell_size_x, self.y), size=(1, self.height))
            for y in range(board_size_y):
                Rectangle(pos=(self.x, self.y + y * cell_size_y), size=(self.width, 1))

            # Draw the pieces
            for x in range(board_size_x):
                for y in range(board_size_y):
                    cell_value = gomoku_game.get_board_value_at(board_size_y - y - 1, x)
                    match cell_value:
                        case GomokuPlayer.WHITE:
                            Color(*WHITE_COLOR)
                            Ellipse(pos=(self.x + x * cell_size_x, self.y + y * cell_size_y),
                                    size=(cell_size_x, cell_size_y))
                        case GomokuPlayer.BLACK:
                            Color(*BLACK_COLOR)
                            Ellipse(pos=(self.x + x * cell_size_x, self.y + y * cell_size_y),
                                    size=(cell_size_x, cell_size_y))

    def on_touch_down(self, touch):

        gomoku_game = self.get_game()
        if gomoku_game is None:
            return

        if self.collide_point(*touch.pos):
            board_size_x, board_size_y = gomoku_game.get_board_width(), gomoku_game.get_board_height()

            cell_size_x = self.width / board_size_x
            cell_size_y = self.height / board_size_y

            col = int((touch.pos[0] - self.x) / cell_size_x)
            row = int((touch.pos[1] - self.y) / cell_size_y)

            gomoku_game.handle_click(board_size_y - 1 - row, col)
