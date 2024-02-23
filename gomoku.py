import sys

sys.path.append("./build")
import pygomoku


def main():
    game = pygomoku.GomokuGame()
    while not game.is_game_over():
        board_str = game.get_board()
        print(board_str)
        row, col = input("Enter your move (row col): ").split()
        game.make_move(row, col)
        if game.check_win():
            print("We have a winner!")
            break


if __name__ == "__main__":
    main()
