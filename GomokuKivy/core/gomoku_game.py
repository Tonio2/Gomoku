from typing import Tuple, List, Dict
from enum import Enum

import sys
sys.path.append("../lib")
import pygomoku

from core.callback_center import CallbackCenter

class GomokuPlayer(Enum):
    EMPTY = 0
    WHITE = 1
    BLACK = 2

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
        try:
            self.game.make_move(row, col)
            CallbackCenter.shared().send_message("GomokuGame.modified", self)
        except:
            print(f'Failed to play')

    def increase_time(self, delta_time: float):
        self.game_time += delta_time
        self.players_time[self.get_current_player()] -= delta_time
