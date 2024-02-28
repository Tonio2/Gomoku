
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.screenmanager import ScreenManager

from widgets.menu.menu_screen import MenuScreen
from widgets.menu.menu_root_widget import MenuRootWidget
from widgets.game.game_screen import GameScreen
from widgets.game.game_root_widget import GameRootWidget
from widgets.game.game_board_widget import GameBoardWidget

class GomokuApp(App):

    def build(self):
        Builder.load_file('resources/gomoku.kv')
        screen_manager = ScreenManager()
        screen_manager.add_widget(MenuScreen(name='menu'))
        screen_manager.add_widget(GameScreen(name='game'))
        return screen_manager
