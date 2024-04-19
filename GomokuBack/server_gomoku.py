from flask import Flask, request, jsonify
from flask_cors import CORS
import sys
from gomoku_game import GomokuGame
from constants import *

sys.path.append("../lib")
import pygomoku # type: ignore

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})


games = {}  # Dictionary to store game instances / TODO: Store them 3 days


@app.route("/create_game", methods=["POST"])
def create_game():
    try:
        user_id = request.json["userId"]  # Unique identifier for the user/session
        size = request.json.get("size", 19)
        mode = request.json.get("mode", 0)
        rule_style = request.json.get("rule_style", 0)
        ai_is_first = request.json.get("AiIsFirst", 0)
        game = games.get(user_id)
        if not game:
            game = GomokuGame(Mode(mode), rule_style, ai_is_first, size)
            games[user_id] = game
        state = game.get_state()
        state["success"] = True
        state["message"] = "Game created"
        return jsonify(state)
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})

@app.route("/reset_game", methods=["POST"])
def reset_game():
    user_id = request.json["userId"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    try:
        game.reset()
        return jsonify({"success": True, "message": "Game reset"})
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})


@app.route("/make_move", methods=["POST"])
def make_move():
    user_id = request.json["user_id"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    if game.is_game_over():
        return jsonify({"success": False, "message": "Game is over"})

    row = request.json["row"]
    col = request.json["col"]

    success, msg = game.make_move(row, col)
    state = game.get_state()
    state["success"] = success
    state["message"] = msg
    return jsonify(state)


@app.route("/reverse_move", methods=["POST"])
def reverse_move():
    user_id = request.json["user_id"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    success, msg = game.reverse_move()
    state = game.get_state()
    state["success"] = success
    state["message"] = msg
    return jsonify(state)

@app.route("/reapply_move", methods=["POST"])
def reapply_move():
    user_id = request.json["user_id"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    success, msg = game.reapply_move()
    state = game.get_state()
    state["success"] = success
    state["message"] = msg
    return jsonify(state)

@app.route("/ai_turn", methods=["POST"])
def ai_turn():
    user_id = request.json["user_id"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    if not game.players[game.next_player]["isAI"]:
        return jsonify({"success": False, "message": "not AI's turn"})

    if game.next_action == Action.MOVE:
        try:
            move_evaluation = game.get_suggestion(3, 3)
            best_move_evaluation = max(move_evaluation["list_moves"], key = lambda move_evaluation: move_evaluation["score"])
            success, msg = game.make_move(best_move_evaluation["move"][0], best_move_evaluation["move"][1])
            state = game.get_state()
            state["success"] = success
            state["message"] = msg
            return jsonify(state)
        except Exception as e:
            print("Error while playing AI turn: ", e)
            return jsonify({"success": False, "message": "Server error"})


@app.route("/get_suggestion", methods=["GET"])
def get_suggestion():
    user_id = request.args.get("user_id")
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    try:
        move_evaluation = game.get_suggestion(3, 3)
        return jsonify({"success": True, "moveEvaluation": move_evaluation})
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})

