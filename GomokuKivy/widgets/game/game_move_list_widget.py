from kivy.uix.widget import Widget
from core.callback_center import CallbackCenter
from core.gomoku_game import GomokuGame, GomokuPlayer, GomokuMove
from app.shared_object import SharedObject
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.uix.label import Label


class GameMoveListWidget(Widget):

    grid_widget = ObjectProperty(None)

    def __init__(self, **kwargs):
        super(GameMoveListWidget, self).__init__(**kwargs)
        CallbackCenter.shared().add_callback("GomokuGame.modified", self.on_game_modified)

    def get_game(self) -> GomokuGame:
        return SharedObject.get_instance().get_game()

    def on_game_modified(self, message, game: GomokuGame):
        self.grid_widget.clear_widgets()

        for index, move in enumerate(game.move_list):
            label = Label(text=self.move_to_str(move))
            label.height = 22
            label.color = (0.9, 0.9, 0.9) if game.last_move_index == index else (0.5, 0.5, 0.5)
            self.grid_widget.add_widget(label)
        
        self.grid_widget.height = 22 * ((len(self.grid_widget.children) + 1) // 2)

    def move_to_str(self, move: GomokuMove) -> str:
        return f'{move.row}:{move.column}'
