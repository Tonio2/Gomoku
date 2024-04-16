from flask import Flask, request, jsonify
from flask_cors import CORS
from enum import Enum
import sys

sys.path.append("../lib")
import pygomoku

BLACK = 0
WHITE = 1

class Action(Enum):
    MOVE = 0
    SWAP = 1
    
class RuleStyle(Enum):
    STANDARD = 0
    PRO = 1
    SWAP = 2

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})


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
    def __init__(self, mode, ruleStyle, AiIsFirst, size=19):
        self.game = pygomoku.GomokuGame(size, size)
        self.next_player = 0
        self.next_action = Action.MOVE
        self.mode = mode
        self.ruleStyle = ruleStyle
        self.listMoves = []
        self.current_move = 0
        self.players = [
            {
                "color": BLACK,
                "isAI": AiIsFirst,
                "score": 0,
                "time": 0,
            },
            {
                "color": WHITE,
                "isAI": not AiIsFirst,
                "score": 0,
                "time": 0,
            },
        ]
        self.size = size
    
    def is_move_legal(self, row, col):
        if RuleStyle == RuleStyle.PRO:
            if self.current_move == 0:
                return row == self.size / 2 and col == self.size / 2
            

    def make_move(self, row, col):
        if not is_move_legal(row,col):
            return False
        try:
            moveResult = self.game.make_move(row, col)
            # Determine next player and next action and current move
            if self.ruleStyle == RuleStyle.SWAP and self.current_move <= 2:
                if self.current_move == 0 or self.current_move == 1:
                    # player does not change
                    self.next_action = Action.MOVE
                if self.current_move == 2:
                    self.next_action = Action.SWAP
                    self.next_player = 1 - self.next_player
            else:
                self.next_player = 1 - self.next_player
                self.next_action = Action.MOVE
            
            # Update listMoves
            self.currentMove += 1
            self.listMoves.append({
                "row": row,
                "col": col,
                "moveResult": moveResult,
            })
            return True
        except Exception as e:
            return False
        

    def reverse_move(self, moveResult):
        self.game.reverse_move(deserialize_move_result(moveResult))
        self.current_player = 1 if self.current_player == 2 else 2

    def reapply_move(self, moveResult):
        self.game.reapply_move(deserialize_move_result(moveResult))
        self.current_player = 1 if self.current_player == 2 else 2

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

    def reset(self):
        self.game = pygomoku.GomokuGame(self.size, self.size)
        self.current_player = 1


class AI:
    def __init__(self, game, player, depth, length):
        self.ai = pygomoku.GomokuAI(game, pygomoku.Player(player), depth, length)

    def get_suggestion(self):
        return serialize_moveEvaluation(self.ai.suggest_move())


games = {}  # Dictionary to store game instances / Store them 3 days


@app.route("/create_game", methods=["POST"])
def create_game():
    try:
        user_id = request.json["userId"]  # Unique identifier for the user/session
        size = request.json.get("size", 19)
        mode = request.json.get("mode", 0)
        ruleStyle = request.json.get("ruleStyle", 0)
        AiIsFirst = request.json.get("AiIsFirst", 0)
        game = games.get(user_id)
        if not game:
            game = GomokuGame(mode, size, ruleStyle, AiIsFirst)
            games[user_id] = game
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})
    return jsonify({"success": True, "message": "Game created", "user_id": user_id})


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

    try:
        moveResult = game.make_move(row, col)
        return jsonify(
            {
                "success": True,
                "newBoard": game.get_board(),
                "newIsGameOver": game.is_game_over(),
                "newWinner": game.get_winner(),
                "moveResult": moveResult,
            }
        )
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})


@app.route("/reverse_move", methods=["POST"])
def reverse_move():
    user_id = request.json["user_id"]
    moveResult = request.json["moveResult"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    try:
        game.reverse_move(moveResult)
        return jsonify(
            {
                "success": True,
                "newBoard": game.get_board(),
                "is_game_over": game.is_game_over(),
                "newWinner": game.get_winner(),
            }
        )
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})


@app.route("/reapply_move", methods=["POST"])
def reapply_move():
    user_id = request.json["user_id"]
    moveResult = request.json["moveResult"]
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    try:
        game.reapply_move(moveResult)
        return jsonify(
            {
                "success": True,
                "newBoard": game.get_board(),
                "is_game_over": game.is_game_over(),
                "newWinner": game.get_winner(),
            }
        )
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})


@app.route("/get_suggestion", methods=["GET"])
def get_suggestion():
    user_id = request.args.get("user_id")
    game = games.get(user_id)
    if not game:
        return jsonify({"success": False, "message": "Game not found"})

    try:
        ai = AI(game.get_game(), game.get_current_player(), 5, 3)
        moveEvaluation = ai.get_suggestion()
        return jsonify({"success": True, "moveEvaluation": moveEvaluation})
    except Exception as e:
        return jsonify({"success": False, "message": str(e)})
