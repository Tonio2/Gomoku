from flask import Flask, request, jsonify
from flask_cors import CORS
from functools import wraps
from flask_socketio import SocketIO, join_room, leave_room, emit
import random
import string

from gomoku_room import GomokuRoom, RoomError, OnlineRoom

import sys

sys.path.append("../lib")
import pygomoku  # type: ignore

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})
socketio = SocketIO(app, cors_allowed_origins="*")


def handle_exceptions(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        try:
            return f(*args, **kwargs)
        except RoomError as e:
            return jsonify({"success": False, "message": str(e)})

    return decorated_function


def handle_socket_exception(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        try:
            return f(*args, **kwargs)
        except RoomError as e:
            return False, str(e)

    return decorated_function


rooms = {}  # Dictionary to store room instances / TODO: Store them 3 days
online_rooms = {}


def generate_room_id():
    room_id = ""
    characters = string.ascii_uppercase + string.digits
    while room_id == "" or online_rooms.get(room_id) != None:
        room_id = "".join(random.choices(characters, k=3))
    return room_id


@app.route("/create_online_room", methods=["POST"])
@handle_exceptions
def create_online_room():
    size = request.json.get("size", 19)
    rule_style = request.json.get("rule_style", 0)
    room_id = generate_room_id()
    online_room = OnlineRoom(size, rule_style)
    online_rooms[room_id] = online_room
    return jsonify({"success": True, "roomId": room_id})


@app.route("/reset_online_room", methods=["POST"])
@handle_exceptions
def reset_online_room():
    room_id = request.json["roomId"]
    room = online_rooms.get(room_id)
    if not room:
        raise RoomError("Room not found")
    room.reset()
    socketio.emit("update", room.get_state(), to=room_id)
    return True

@app.route("/get_online_rooms", methods=["GET"])
@handle_exceptions
def get_online_rooms():
    return [{"roomId": room_id, "availableRoles": room.get_available_roles()} for room_id, room in online_rooms.items()]


@socketio.on("whoami")
@handle_socket_exception
def whoami(data):
    room_id = data["room_id"]
    room = online_rooms.get(room_id)
    if not room:
        raise RoomError("Room not found")
    player_id = room.get_player_id_from_ip(request.sid)
    available_roles = room.get_available_roles()
    emit("update", room.get_state())
    if True not in available_roles[1:]:
        emit("connected", 0, to=room_id)
    join_room(room_id)
    return True, "", player_id, available_roles


@socketio.on("join")
@handle_socket_exception
def on_join(data):
    room_id = data["room_id"]
    player_id = data["player_id"]
    print(room_id, player_id, request.sid)
    room = online_rooms.get(room_id)
    if not room:
        raise RoomError("Room not found")
    if player_id != 0:
        room.connect(request.sid, player_id)
    emit("update", room.get_state(), to=room_id)
    emit("connected", player_id, to=room_id)
    return True, ""


@socketio.on("disconnect")
def test_disconnect():
    for room_id, room in online_rooms.items():
        player_id = room.disconnect(request.sid)
        if player_id != 0:
            emit("disconnected", {"playerId": player_id}, to=room_id)
    print("Client disconnected: ", request.sid)


@socketio.on("make_move")
@handle_socket_exception
def on_make_move(data):
    room_id = data["room_id"]
    ip = request.sid
    row = data["row"]
    col = data["col"]
    room = online_rooms.get(room_id)
    print(ip, row, col)
    if not room:
        raise RoomError("Room not found")
    room.make_move(ip, row, col)
    emit("update", room.get_state(), to=room_id)
    return True, ""


# TODO : Disconnect


@app.route("/create_room", methods=["POST"])
@handle_exceptions
def create_room():
    user_id = request.json["userId"]  # Unique identifier for the user/session
    size = request.json.get("size", 19)
    mode = request.json.get("mode", 0)
    rule_style = request.json.get("rule_style", 0)
    ai_player = request.json.get("ai_player", 2)
    room = rooms.get(user_id)
    if not room:
        room = GomokuRoom(size, mode, rule_style, ai_player)
        rooms[user_id] = room
    state = room.get_state()
    state["success"] = True
    state["message"] = ""
    return jsonify(state)


@app.route("/reset_room", methods=["POST"])  # TODO
@handle_exceptions
def reset_room():
    user_id = request.json["userId"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    room.reset()  # TODO
    state = room.get_state()
    state["success"] = True
    state["message"] = ""
    return jsonify(state)


@app.route("/make_move", methods=["POST"])
@handle_exceptions
def make_move():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    # if room.is_room_over():
    #    return jsonify({"success": False, "message": "Game is over"})

    row = request.json["row"]
    col = request.json["col"]

    room.make_move(row, col)
    state = room.get_state()
    state["success"] = True
    state["message"] = ""
    return jsonify(state)


@app.route("/reverse_move", methods=["POST"])
@handle_exceptions
def reverse_move():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    room.reverse_last_action()
    state = room.get_state()
    state["success"] = True
    return jsonify(state)


@app.route("/reapply_move", methods=["POST"])
@handle_exceptions
def reapply_move():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    room.reapply_last_action()
    state = room.get_state()
    state["success"] = True
    return jsonify(state)


@app.route("/ai_turn", methods=["POST"])
def ai_turn():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    if not room.has_pending_action():
        return jsonify({"success": False, "message": "not AI's turn"})

    room.perform_pending_action()
    state = room.get_state()
    state["success"] = True
    return jsonify(state)


@app.route("/swap", methods=["POST"])
def swap():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    swap = request.json["swap"]
    room.swap(swap)
    state = room.get_state()
    state["success"] = True
    return jsonify(state)


@app.route("/get_suggestion", methods=["GET"])
@handle_exceptions
def get_suggestion():
    user_id = request.args.get("user_id")
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    move_evaluation = room.get_suggestion(3, 3)
    return jsonify({"success": True, "moveEvaluation": move_evaluation})
