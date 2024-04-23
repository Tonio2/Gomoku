from flask import Flask, request, jsonify
from flask_cors import CORS
from functools import wraps

import sys
from gomoku_room import GomokuRoom

sys.path.append("../lib")
import pygomoku # type: ignore

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})

def handle_exceptions(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        try:
            return f(*args, **kwargs)
        except Exception as e:
            # Log the exception if needed
            return jsonify({"success": False, "message": str(e)})
    return decorated_function

rooms = {}  # Dictionary to store room instances / TODO: Store them 3 days


@app.route("/create_room", methods=["POST"])
@handle_exceptions
def create_room():
    user_id = request.json["userId"]  # Unique identifier for the user/session
    size = request.json.get("size", 19)
    mode = request.json.get("mode", 0)
    rule_style = request.json.get("ruleStyle", 0)
    ai_player = request.json.get("aiPlayer", 2)
    room = rooms.get(user_id)
    if not room:
        room = GomokuRoom(size, mode, rule_style, ai_player)
        rooms[user_id] = room
    state = room.get_state()
    state["success"] = True
    state["message"] = ""
    return jsonify(state)

@app.route("/reset_room", methods=["POST"]) # TODO
@handle_exceptions
def reset_room():
    user_id = request.json["userId"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    room.reset() # TODO
    return jsonify({"success": True, "message": "Game reset"})


@app.route("/make_move", methods=["POST"])
@handle_exceptions
def make_move():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    #if room.is_room_over():
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

    room.reverse_move()
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

    room.reapply_move()
    state = room.get_state()
    state["success"] = True
    return jsonify(state)

@app.route("/ai_turn", methods=["POST"])
def ai_turn():
    user_id = request.json["user_id"]
    room = rooms.get(user_id)
    if not room:
        return jsonify({"success": False, "message": "Game not found"})

    if not room.players[room.next_player]["isAI"]:
        return jsonify({"success": False, "message": "not AI's turn"})

    if room.next_action == Action.MOVE:
        move_evaluation = room.get_suggestion(5, 3)
        best_move_evaluation = max(move_evaluation["list_moves"], key = lambda move_evaluation: move_evaluation["score"])
        msg = room.make_move(best_move_evaluation["move"][0], best_move_evaluation["move"][1])
        state = room.get_state()
        state["success"] = True
        state["message"] = msg
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
