from flask import Flask, request
from flask_socketio import SocketIO, emit
from flask_cors import CORS

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})
socketio = SocketIO(app, cors_allowed_origins="http://localhost:3000")


def calculate_winner(squares):
    lines = [
        [0, 1, 2],
        [3, 4, 5],
        [6, 7, 8],
        [0, 3, 6],
        [1, 4, 7],
        [2, 5, 8],
        [0, 4, 8],
        [2, 4, 6],
    ]
    for line in lines:
        a, b, c = line
        if squares[a] and squares[a] == squares[b] and squares[a] == squares[c]:
            return squares[a]
    return None


@socketio.on("connect")
def handle_connection():
    print("A user connected")
    board = [None] * 9
    current_player = "X"

    @socketio.on("board")
    def handle_board():
        emit("board", board, broadcast=True)

    @socketio.on("makeMove")
    def handle_make_move(index):
        nonlocal current_player
        if board[index] or calculate_winner(board):
            return {"success": False}
        board[index] = current_player
        current_player = "X" if current_player == "O" else "O"
        return {
            "success": True,
            "board": board,
            "winner": calculate_winner(board),
            "index": index,
        }

    @socketio.on("reverseMove")
    def handle_reverse_move(index):
        nonlocal current_player
        board[index] = None
        current_player = "X" if current_player == "O" else "O"
        return {
            "success": True,
            "board": board,
            "winner": calculate_winner(board),
            "index": index,
        }

    @socketio.on("reapplyMove")
    def handle_reapply_move(index):
        nonlocal current_player
        board[index] = current_player
        current_player = "X" if current_player == "O" else "O"
        return {
            "success": True,
            "board": board,
            "winner": calculate_winner(board),
            "index": index,
        }


if __name__ == "__main__":
    port = 5000  # You can choose the same port or a different one
    socketio.run(app, port=port, debug=True)
