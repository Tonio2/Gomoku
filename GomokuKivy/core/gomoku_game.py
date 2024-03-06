from typing import Tuple, List, Dict
from enum import Enum

import sys
sys.path.append("../lib")
import pygomoku

from core.callback_center import CallbackCenter

class GomokuPlayer(Enum):
    EMPTY = 0
    BLACK = 1
    WHITE = 2

class GomokuMove:
    player: GomokuPlayer
    row: int
    column: int
    timestamp: float

class GomokuGame:

    game: pygomoku.GomokuGame

    move_list: List[GomokuMove]

    game_time: float
    players_time: Dict[GomokuPlayer, float]

    def __init__(self, size: int, player_time: float):
        self.game = pygomoku.GomokuGame(size)

        self.move_list = []

        self.game_time = 0
        self.players_time = {
            GomokuPlayer.WHITE: player_time,
            GomokuPlayer.BLACK: player_time,
        }

    def get_board_size(self) -> Tuple[int, int]:
        size = self.game.get_board_size()
        return (size, size)

    def get_board_value_at(self, row: int, col: int) -> GomokuPlayer:
        value = self.game.get_board_value(row, col)
        board_value = GomokuPlayer(value)
        return board_value

    def get_current_player(self) -> GomokuPlayer:
        return GomokuPlayer(self.game.get_current_player())

    def play_at(self, row: int, col: int):
        modified = False
        try:
            new_move = GomokuMove()
            new_move.player = self.get_current_player()
            new_move.row = row
            new_move.column = col
            new_move.timestamp = self.game_time
            
            new_move.move_result = self.game.make_move(row, col)
            modified = True

            self.last_move_index += 1
            self.resize_move_list(self.last_move_index + 1)

            self.move_list[self.last_move_index] = new_move

        except:
            print(f'Failed to play')
        
        if modified:
            CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def increase_time(self, delta_time: float):
        self.game_time += delta_time
        self.players_time[self.get_current_player()] -= delta_time
        CallbackCenter.shared().send_message("GomokuGame.time", self)

    def reverse_last_move(self):
        if (self.last_move_index < 0):
            return

        self.game.reverse_move(self.move_list[self.last_move_index].move_result)
        self.last_move_index -= 1
        CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def reapply_last_move(self):
        reapply_index = self.last_move_index + 1

        if (reapply_index >= len(self.move_list)):
            return

        self.game.reapply_move(self.move_list[reapply_index].move_result)
        self.last_move_index += 1
        CallbackCenter.shared().send_message("GomokuGame.modified", self)
