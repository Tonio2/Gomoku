
from kivy.app import App
from kivy.lang import Builder
from widgets.root_widget import RootWidget
from widgets.gomoku_board_widget import GomokuBoardWidget
from core.gomoku_game import GomokuGame

class GomokuApp(App):

    def build(self):
        Builder.load_file('resources/gomoku.kv')
        return RootWidget()

    def on_start(self):
        self.root.board.gomoku_game = GomokuGame()
