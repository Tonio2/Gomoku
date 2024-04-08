import random
import string
import functools

from flask import Blueprint, jsonify, request, session
from .room_list import get_all_rooms

from .game_room import GameRoom

bp = Blueprint("rooms", __name__, url_prefix="/rooms")

def new_room_id() -> str:
    """Generate a new unique room ID"""
    return ''.join(random.choices(string.ascii_uppercase + string.digits, k=6))

@bp.route("/", methods = ["GET"])
def get_all():
    """Fetch all rooms"""
    room_list = get_all_rooms()
    return jsonify(room_list)

@bp.route("/create", methods=["POST"])
def create():
    """Create a new room"""
    game_id = new_room_id()
    # room_data = GameRoom()
    return jsonify({"game_id": game_id})

@bp.route("/join", methods=["POST"])
def join():
    """Join a room"""
    data = request.json
    game_id = data.get('game_id')
    return jsonify({"success": True})
