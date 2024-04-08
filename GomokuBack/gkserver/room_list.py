import multiprocessing
from typing import Dict, Optional
from .game_room import GameRoom
from flask import current_app

def init_app(app):
    manager = multiprocessing.Manager()
    app.room_list = manager.dict()
    app.room_list_lock = manager.Lock()

def add_room(room_id: str, room_data: GameRoom):
    with current_app.room_list_lock:
        current_app.room_list[room_id] = room_data

def get_room(room_id: str) -> Optional[GameRoom]:
    with current_app.room_list_lock:
        return current_app.room_list.get(room_id)

def remove_room(room_id: str):
    with current_app.room_list_lock:
        current_app.room_list.pop(room_id, None)

def get_all_rooms() -> list[str]:
    with current_app.room_list_lock:
        return list(current_app.room_list.keys())
