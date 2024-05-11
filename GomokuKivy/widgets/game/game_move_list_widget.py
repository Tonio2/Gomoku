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
        CallbackCenter.shared().add_callback(
            "GomokuGame.start", self.on_game_modified
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
            label = Label(text=self.move_to_str(move), size_hint_x=0.9)
            label.halign = "left"
            label.text_size = (self.width, None)
            label.padding_x = 10
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
        coord_name = room.coordinates_name(move.row, move.column) if room is not None else f"{move.row}:{move.column}"
        player_name = f"{move.player.to_str()}" 
        timestamp = time_to_str(move.timestamp)

        score_change = move.move_result.black_score_change if move.player == GomokuPlayer.BLACK else move.move_result.white_score_change
        score_change_str = f"/x" if score_change != 0 else ""

        return f"{timestamp} - {player_name} => {coord_name}{score_change_str}"

def time_to_str(time: float) -> str:
    minutes = int(time // 60)
    seconds = int(time % 60)
    milliseconds = int((time - int(time)) * 100)
    return f"{minutes:02d}:{seconds:02d}.{milliseconds:02d}"
