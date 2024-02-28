

from app.shared_object import SharedObject
from kivy.uix.screenmanager import ScreenManager, Screen
from widgets.game.game_root_widget import GameRootWidget

class GameScreen(Screen):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.root = GameRootWidget()

    def back_to_menu(self):
        self.manager.current = 'menu'

    def on_enter(self):
        game_board_widget = self.ids.game_board_widget
        game_board_widget.draw_board()
        print(f'Drawing with size: {SharedObject.get_instance().get_game().get_board_size()}')
