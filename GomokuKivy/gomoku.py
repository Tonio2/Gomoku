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

# Old code
# import sys

# sys.path.append("./build")
# import pygomoku


# def main():
#     game = pygomoku.GomokuGame()
#     while not game.is_game_over():
#         board_str = game.get_board()
#         print(board_str)
#         row, col = map(int, input("Enter your move (row col): ").split())
#         game.make_move(row, col)
#         if game.check_win():
#             print("We have a winner!")
#             break


# if __name__ == "__main__":
#     main()
