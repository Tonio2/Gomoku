from typing import Tuple, List, Dict
from enum import Enum
from datetime import datetime

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
    mode: int

    move_list: List[GomokuMove]
    last_move_index: int

    players_time: Dict[GomokuPlayer, float]

    start_turn: datetime
    start_game: datetime

    def __init__(self, width: int, height: int, player_time: float, mode: int):
        self.game = pygomoku.GomokuGame(width, height)
        self.mode = mode

        self.move_list = []
        self.last_move_index = -1

        self.players_time = {
            GomokuPlayer.WHITE: player_time,
            GomokuPlayer.BLACK: player_time,
        }

        self.players_time_since_start_turn = {
            GomokuPlayer.WHITE: 0,
            GomokuPlayer.BLACK: 0,
        }

        self.start_turn = datetime.now()
        self.start_game = datetime.now()

        # if mode == 1:
        #     move_str = "77,55,78,76,79,54,56,65,53,54,87,67,97,43,32,7A,58,68,85,A7,57,67,59,5A,55"
        #     moves = move_str.split(",")
        #     for i in range(len(moves)):
        #         row = int(moves[i][0], 16)
        #         col = int(moves[i][1], 16)
        #         self.play_at(row, col)

    def get_board_width(self) -> int:
        return self.game.get_board_width()

    def get_board_height(self) -> int:
        return self.game.get_board_height()

    def get_board_value_at(self, row: int, col: int) -> GomokuPlayer:
        value = self.game.get_board_value(row, col)
        board_value = GomokuPlayer(value)
        return board_value

    _index_names = [str(i) if i < 10 else chr(65 + i - 10) for i in range(36)]

    def coordinate_index_name(self, index: int) -> str:
        return self._index_names[index]

    def coordinates_name(self, row: int, col: int) -> str:
        return self.coordinate_index_name(row) + self.coordinate_index_name(col)

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

    def get_AI_suggestion(self):
        AI = pygomoku.GomokuAI(self.game, self.game.get_current_player(), 4)
        moveEvaluations = AI.suggest_move()
        bestMove = max(moveEvaluations.listMoves, key=lambda x: x.score).move
        return bestMove

    def handle_click(self, row: int, col: int):
        if self.mode == 1:
            self.play_at(row, col)
        else:
            if self.get_current_player() == GomokuPlayer.BLACK:
                self.play_at(row, col)

    def play_at(self, row: int, col: int):
        modified = False
        try:
            new_move = GomokuMove()
            new_move.player = self.get_current_player()
            new_move.row = row
            new_move.column = col
            new_move.timestamp = (datetime.now() - self.start_game).total_seconds()

            new_move.move_result = self.game.make_move(row, col)
            modified = True

            self.last_move_index += 1
            self.resize_move_list(self.last_move_index + 1)

            self.move_list[self.last_move_index] = new_move

        except:
            print(f"Failed to play at {row}:{col}")

        if modified:
            self.players_time[new_move.player] -= self.players_time_since_start_turn[
                new_move.player
            ]
            self.players_time_since_start_turn[new_move.player] = 0
            self.start_turn = datetime.now()
            CallbackCenter.shared().send_message("GomokuGame.modified", self)

            if self.game.is_game_over():
                CallbackCenter.shared().send_message("GomokuGame.gameover", self)
            CallbackCenter.shared().send_message("GomokuGame.endturn", self)

    def resize_move_list(self, new_size: int):
        current_size = len(self.move_list)
        if current_size < new_size:
            self.move_list.extend([None] * (new_size - current_size))
        elif current_size > new_size:
            self.move_list = self.move_list[:new_size]

    def update_time(self):
        self.players_time_since_start_turn[self.get_current_player()] = (
            datetime.now() - self.start_turn
        ).total_seconds()
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
