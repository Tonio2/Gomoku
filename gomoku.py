import sys

sys.path.append("./build")
import pygomoku


def main():
    game = pygomoku.GomokuGame()
    while not game.is_game_over():
        board_str = game.get_board()
        print(board_str)
        illegal = True
        error = ""
        while illegal:
            try:
                row, col = input("Enter your move (row col): ").split()
                game.make_move(row, col)
                illegal = False
            except Exception as e:
                error = str(e)
                print(f"Error: {error}")
                illegal = True

    print("Game over!")


if __name__ == "__main__":
    main()
