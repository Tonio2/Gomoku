from kivy.uix.gridlayout import GridLayout
from core.callback_center import CallbackCenter
from core.gomoku_room import GameRoom, GomokuPlayer, GomokuMove
from app.shared_object import SharedObject
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty
from kivy.uix.label import Label
from kivy.uix.widget import Widget


class GameMoveListWidget(GridLayout):

    scroll_view = ObjectProperty(None)

    def __init__(self, **kwargs):
        super(GameMoveListWidget, self).__init__(**kwargs)
        CallbackCenter.shared().add_callback(
            "GomokuGame.modified", self.on_game_modified
        )
        self.bind(minimum_height=self.setter("height"))

    def get_room(self) -> GameRoom:
        return SharedObject.get_instance().get_room()

    def on_game_modified(self, _, room: GameRoom):
        self.clear_widgets()

        for _ in range(int(self.scroll_view.height / float(self.row_default_height))):
            self.add_widget(Widget())

        target_widget = None

        for index, move in enumerate(room.get_move_list()):
            label = Label(text=self.move_to_str(move))
            is_target_widget = room.get_move_index() == index
            label.color = (
                (0.9, 0.9, 0.9) if is_target_widget else (0.5, 0.5, 0.5)
            )
            self.add_widget(label)
            if is_target_widget:
                target_widget = label
        
        if target_widget is not None:
            self.scroll_view.scroll_to(target_widget)

    def move_to_str(self, move: GomokuMove) -> str:
        room = self.get_room()
        if room is None:
            return f"{move.row}:{move.column} ({move.timestamp})"

        return f"{room.coordinates_name(move.row, move.column)} ({move.timestamp})"
