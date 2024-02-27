import sys

sys.path.append("../lib")
import pygomoku

coordinates = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']

def main():
    current_player = pygomoku.Player.BLACK
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
                if current_player == pygomoku.Player.BLACK:
                    print("Black's turn")
                    row, col = input("Enter your move (row col): ").split()
                    row = coordinates.index(row)
                    col = coordinates.index(col)
                    game.make_move(row, col)
                    illegal = False
                    current_player = pygomoku.Player.WHITE
                else:
                    print("White's turn")
                    AI = pygomoku.GomokuAI(game, pygomoku.Player.WHITE, 2)
                    [row, col] = AI.suggest_move()
                    game.make_move(row, col)
                    illegal = False
                    current_player = pygomoku.Player.BLACK
            except Exception as e:
                error = str(e)
                print(f"Error: {error}")
                illegal = True

    print("Game over!")
    print("Winner: ", game.get_winner())


if __name__ == "__main__":
    main()