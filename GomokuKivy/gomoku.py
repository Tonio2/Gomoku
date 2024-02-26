import sys

sys.path.append("../lib")
import pygomoku

import kivy

from kivy.app import App
from kivy.uix.label import Label

class MyApp(App):

    def build(self):
        game = pygomoku.GomokuGame()
        return Label(text=game.get_board())

if __name__ == '__main__':
    MyApp().run()
