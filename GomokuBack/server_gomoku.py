import random
import string
from flask import Flask, jsonify, request
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


class Mode:
    PVP_OFFLINE = 1
    PVP_ONLINE = 2
    PVIA = 3


class GomokuRoom:
    def __init__(self, size, mode, ruleStyle, roomCreatorIsFirstPlayer=True):
        if mode not in [Mode.PVP_OFFLINE, Mode.PVP_ONLINE, Mode.PVIA]:
            raise Exception("Invalid mode")
        arePlayersHuman = [True, True]
        if mode == Mode.PVIA:
            arePlayersHuman[int(roomCreatorIsFirstPlayer)] = False

        self.room = pygomoku.Room(size, size, arePlayersHuman, ruleStyle)
        self.mode = mode
        self.ruleStyle = ruleStyle
        self.playersIp = ["", ""]


rooms = {}


def uniqueID():
    isUnique = False
    while not isUnique:
        roomID = "".join(
            random.choices(
                string.ascii_uppercase + string.digits + string.ascii_lowercase, k=3
            )
        )
        if roomID not in rooms:
            isUnique = True
    return roomID


@app.route("/create_game", methods=["POST"])
def create_game():
    # Generate a string of 4 characters which will be the unique ID for the game
    roomID = uniqueID()
    size = request.json.get("size", 19)
    mode = request.json.get("mode", 0)
    ruleStyle = request.json.get("ruleStyle", 0)
    roomCreatorIsFirstPlayer = request.json.get("roomCreatorIsFirstPlayer", True)
    room = GomokuRoom(size, mode, ruleStyle, roomCreatorIsFirstPlayer)
    rooms[roomID] = room
    return jsonify({"success": True, "message": "Game created", "roomID": roomID})


@socketio.on("connect")
def handle_connection():
    print("A user connected")

    @socketio.on("joinRoom")
    def handle_join_room(roomID):
        if roomID not in rooms:
            return {"success": False, "message": "Room not found"}
        return {"success": True, "message": "Room found"}

    @socketio.on("board")
    def handle_board():
        emit("board", game.get_board(), broadcast=True)

    @socketio.on("makeMove")
    def handle_make_move(row, col):
        success, moveResult = game.make_move(row, col)
        print("Move result: ", moveResult)
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
