import asyncio
from typing import Tuple, List, Dict
from enum import Enum
from datetime import datetime
from kivy.clock import Clock

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

class GomokuRuleStyle(Enum):
    STANDARD = 0
    PRO = 1
    LONG_PRO = 2
    SWAP = 3
    SWAP2 = 4

class GameRoomSettings:

    settings: pygomoku.GameRoomSettings

    def __init__(self):
        self.settings = pygomoku.GameRoomSettings()

    def set_width(self, width: int):
        self.settings.width = width
    
    def get_width(self) -> int:
        return self.settings.width

    def set_height(self, height: int):
        self.settings.height = height
    
    def get_height(self) -> int:
        return self.settings.height

    def set_rule_style(self, ruleStyle: GomokuRuleStyle):
        self.settings.ruleStyle = ruleStyle

    def get_rule_style(self) -> GomokuRuleStyle:
        return GomokuRuleStyle(self.settings.ruleStyle)

    def set_is_p1_ai(self, isAi: bool):
        self.settings.p1.is_ai = isAi

    def get_is_p1_ai(self) -> bool:
        return self.settings.p1.is_ai

    def set_is_p2_ai(self, isAi: bool):
        self.settings.p2.is_ai = isAi

    def get_is_p2_ai(self) -> bool:
        return self.settings.p2.is_ai

    def set_p1_ai_depth(self, depth: int):
        self.settings.p1.ai_depth = depth

    def get_p1_ai_depth(self) -> int:
        return self.settings.p1.ai_depth

    def set_p2_ai_depth(self, depth: int):
        self.settings.p2.ai_depth = depth

    def get_p2_ai_depth(self) -> int:
        return self.settings.p2.ai_depth

class GameRoom:

    room: pygomoku.GameRoom
    move_list: List[GomokuMove]
    last_move_index: int

    players_time: Dict[GomokuPlayer, float]
    players_time_since_start_turn: Dict[GomokuPlayer, float]

    start_turn: datetime
    start_game: datetime

    def __init__(self, settings: GameRoomSettings):
        self.room = pygomoku.GameRoom(settings.settings)
        self.move_list = []
        self.last_move_index = -1
        self.players_time = {
            GomokuPlayer.WHITE: 0,
            GomokuPlayer.BLACK: 0,
        }
        self.start_turn = datetime.now()
        self.start_game = datetime.now()
        self.players_time_since_start_turn = {
            GomokuPlayer.WHITE: 0,
            GomokuPlayer.BLACK: 0,
        }
        Clock.schedule_interval(self.update_time, 1 / 20)

    def get_settings(self) -> GameRoomSettings:
        settings = GameRoomSettings()
        settings.settings = self.room.get_settings()
        return settings

    def get_board_width(self) -> int:
        return self.room.get_game().get_board_width()

    def get_board_height(self) -> int:
        return self.room.get_game().get_board_height()

    def get_board_value_at(self, row: int, col: int) -> GomokuPlayer:
        return GomokuPlayer(self.room.get_game().get_board_value(row, col))

    def get_current_player(self) -> GomokuPlayer:
        return GomokuPlayer(self.room.get_game().get_current_player())

    def coordinate_index_name(self, index: int) -> str:
        return str(pygomoku.coordinate_to_char(index))

    def coordinates_name(self, row: int, col: int) -> str:
        return self.coordinate_index_name(row) + self.coordinate_index_name(col)

    def get_player_score(self, player: GomokuPlayer) -> int:
        return self.room.get_game().get_player_score(player.value)

    def get_winner(self) -> GomokuPlayer:
        if self.room.get_game().is_game_over():
            return GomokuPlayer(self.room.get_game().get_winner())
        return GomokuPlayer.EMPTY

    def update_time(self, dt: float):
        self.players_time_since_start_turn[self.get_current_player()] = (
            datetime.now() - self.start_turn
        ).total_seconds()
        CallbackCenter.shared().send_message("GomokuGame.time", self)

    def get_player_time(self, player: GomokuPlayer) -> float:
        return self.players_time[player] + self.players_time_since_start_turn[player]

    async def perform_pending_actions(self):
        loop = asyncio.get_running_loop()

        await loop.run_in_executor(None, self._perform_pending_actions)

    def _perform_pending_actions(self):
        last_index = len(self.room.get_actions_history()) - 1
        while self.room.perform_pending_action():
            self.update_actions_list_since_index(last_index)
            last_index = len(self.room.get_actions_history()) - 1

    def update_actions_list_since_index(self, index: int) -> bool:
        modified = False
        for i in range(index + 1, len(self.room.get_actions_history())):
            action = self.room.get_actions_history()[i]
            if action.action_type == pygomoku.GameActionType.MOVE:
                move = GomokuMove()
                move.player = GomokuPlayer(self.room.gomoku_player_from_id(action.player))
                move.row = action.action_value.move.row
                move.column = action.action_value.move.col
                move.timestamp = datetime.now().timestamp()
                move.move_result = action.action_value.move.result
                self.move_list.append(move)
                modified = True
        return modified

    def handle_click(self, row: int, col: int):
        current_gomoku_player = self.room.get_game().get_current_player()
        current_playerid = self.room.id_from_gomoku_player(current_gomoku_player)

        last_index = len(self.room.get_actions_history()) - 1
        result = self.room.perform_action_move(current_playerid, row, col)
        print("result=", result.success, result.message)
        if self.update_actions_list_since_index(last_index):
            CallbackCenter.shared().send_message("GameRoom.modified", self)

        if self.room.get_game().is_game_over():
            CallbackCenter.shared().send_message("GameRoom.gameover", self)

        asyncio.create_task(self.perform_pending_actions())

    def resize_move_list(self, new_size: int):
        current_size = len(self.move_list)
        if current_size < new_size:
            self.move_list.extend([None] * (new_size - current_size))
        elif current_size > new_size:
            self.move_list = self.move_list[:new_size]

    def can_reverse_move(self):
        return False
        # return self.last_move_index >= 0

    def reverse_last_move(self):
        pass
        # if not self.can_reverse_move():
        #     return

        # self.room.reverse_move(self.move_list[self.last_move_index].move_result)
        # self.last_move_index -= 1
        # CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def can_reapply_move(self):
        return False
        # return self.last_move_index + 1 < len(self.move_list)

    def reapply_last_move(self):
        pass
        # if not self.can_reapply_move():
        #     return

        # reapply_index = self.last_move_index + 1
        # self.game.reapply_move(self.move_list[reapply_index].move_result)
        # self.last_move_index += 1
        # CallbackCenter.shared().send_message("GomokuGame.modified", self)
