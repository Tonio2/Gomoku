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

    def to_str(self):
        match self:
            case GomokuPlayer.EMPTY:
                return "No One"
            case GomokuPlayer.BLACK:
                return "Black"
            case GomokuPlayer.WHITE:
                return "White"
        return "None"

class GomokuMove:
    player: GomokuPlayer
    row: int
    column: int
    timestamp: float
    move_result: pygomoku.MoveResult

class GomokuGame:

    game: pygomoku.GomokuGame

    move_list: List[GomokuMove]
    last_move_index: int

    game_time: float
    players_time: Dict[GomokuPlayer, float]

    def __init__(self, size: int, player_time: float):
        self.game = pygomoku.GomokuGame(size)

        self.move_list = []
        self.last_move_index = -1

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

    def get_player_score(self, player: GomokuPlayer) -> int:
        game_player = pygomoku.Player(player.value)
        return self.game.get_player_score(game_player)

    def get_winner(self) -> GomokuPlayer:
        if self.game.is_game_over():
            winning_player = self.game.get_winner()
            return GomokuPlayer(winning_player)
        return GomokuPlayer.EMPTY

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

            if self.game.is_game_over():
                CallbackCenter.shared().send_message("GomokuGame.gameover", self)

    def resize_move_list(self, new_size: int):
        current_size = len(self.move_list)
        if current_size < new_size:
            self.move_list.extend([None] * (new_size - current_size))
        elif current_size > new_size:
            self.move_list = self.move_list[:new_size]

    def increase_time(self, delta_time: float):
        self.game_time += delta_time
        self.players_time[self.get_current_player()] -= delta_time
        CallbackCenter.shared().send_message("GomokuGame.time", self)

    def can_reverse_move(self):
        return self.last_move_index >= 0

    def reverse_last_move(self):
        if not self.can_reverse_move():
            return

        self.game.reverse_move(self.move_list[self.last_move_index].move_result)
        self.last_move_index -= 1
        CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def can_reapply_move(self):
        return self.last_move_index + 1 < len(self.move_list)

    def reapply_last_move(self):
        if not self.can_reapply_move():
            return

        reapply_index = self.last_move_index + 1

        self.game.reapply_move(self.move_list[reapply_index].move_result)
        self.last_move_index += 1
        CallbackCenter.shared().send_message("GomokuGame.modified", self)
