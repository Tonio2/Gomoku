from flask import Flask, request
from flask_socketio import SocketIO, emit
from flask_cors import CORS
import sys

sys.path.append("../lib")
import pygomoku

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})
socketio = SocketIO(app, cors_allowed_origins="http://localhost:3000")


def deserialize_cell_change(cell_change):
    ret = pygomoku.CellChange()
    ret.row = cell_change["row"]
    ret.col = cell_change["col"]
    ret.old_value = pygomoku.Player(cell_change["old_value"])
    ret.new_value = pygomoku.Player(cell_change["new_value"])
    return ret


def deserialize_move_result(moveResult):
    cell_changes = []
    for cell_change in moveResult["cell_changes"]:
        cell_changes.append(deserialize_cell_change(cell_change))
    ret = pygomoku.MoveResult()
    ret.cell_changes = cell_changes
    ret.white_score_change = moveResult["white_score_change"]
    ret.black_score_change = moveResult["black_score_change"]
    return ret


def serialize_player(player):
    if player == pygomoku.Player.BLACK:
        return 1
    elif player == pygomoku.Player.WHITE:
        return 2
    else:
        return 0


def serialize_move_result(moveResult):
    cell_changes = []
    for cell_change in moveResult.cell_changes:
        cell_changes.append(
            {
                "row": cell_change.row,
                "col": cell_change.col,
                "old_value": serialize_player(cell_change.old_value),
                "new_value": serialize_player(cell_change.new_value),
            }
        )
    return {
        "cell_changes": cell_changes,
        "white_score_change": moveResult.white_score_change,
        "black_score_change": moveResult.black_score_change,
    }


def serialize_moveEvaluation(moveEvaluation):
    if moveEvaluation.listMoves is None:
        return {
            "move": (moveEvaluation.move[0], moveEvaluation.move[1]),
            "score": moveEvaluation.score,
        }
    listMoves = []
    for move in moveEvaluation.listMoves:
        listMoves.append(serialize_moveEvaluation(move))
    return {
        "move": (moveEvaluation.move[0], moveEvaluation.move[1]),
        "score": moveEvaluation.score,
        "listMoves": listMoves,
    }


# Create a python class that will handle the game logic
class GomokuGame:
    def __init__(self, mode):
        self.game = pygomoku.GomokuGame(19, 19)
        self.current_player = 1
        self.mode = mode

    def make_move(self, row, col):
        try:
            moveResult = self.game.make_move(row, col)
            self.current_player = 1 if self.current_player == 2 else 2
            return True, serialize_move_result(moveResult)
        except Exception as e:
            return False, None

    def reverse_move(self, moveResult):
        return self.game.reverse_move(deserialize_move_result(moveResult))

    def reapply_move(self, moveResult):
        return self.game.reapply_move(deserialize_move_result(moveResult))

    def get_board(self):
        return self.game.get_board()

    def get_winner(self):
        return serialize_player(self.game.get_winner())

    def is_game_over(self):
        return self.game.is_game_over()

    def get_current_player(self):
        return self.current_player

    def get_game(self):
        return self.game


class AI:
    def __init__(self, game, player, depth):
        self.ai = pygomoku.GomokuAI(game, pygomoku.Player(player), depth)

    def get_suggestion(self):
        return serialize_moveEvaluation(self.ai.suggest_move())


@socketio.on("connect")
def handle_connection():
    print("A user connected")
    game = GomokuGame(1)

    @socketio.on("board")
    def handle_board():
        emit("board", game.get_board(), broadcast=True)

    @socketio.on("makeMove")
    def handle_make_move(row, col):
        success, moveResult = game.make_move(row, col)
        if not success:
            return {"success": False}
        return {
            "success": True,
            "board": game.get_board(),
            "is_game_over": game.is_game_over(),
            "winner": game.get_winner(),
            "moveResult": moveResult,
        }

    @socketio.on("reverseMove")
    def handle_reverse_move(moveResult):
        game.reverse_move(moveResult)
        return {"success": True, "board": game.get_board()}

    @socketio.on("reapplyMove")
    def handle_reapply_move(moveResult):
        game.reapply_move(moveResult)
        return {"success": True, "board": game.get_board()}

    @socketio.on("getSuggestion")
    def handle_get_suggestion():
        ai = AI(game.get_game(), game.get_current_player(), 3)
        moveEvaluation = ai.get_suggestion()
        emit("suggestion", {"success": True, "moveEvaluation": moveEvaluation})
