import sys

sys.path.append("../lib")
import pygomoku


def main():
    game = pygomoku.GomokuGame(19)
    while not game.is_game_over():
        for x in range(0, game.get_board_size()):
            for y in range(0, game.get_board_size()):
                print(game.get_board_value(x, y), end=" ")
            print()
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
    print("Winner: ", game.get_winner())


if __name__ == "__main__":
    main()