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

    def to_pygomoku(self) -> pygomoku.Player:
        match self:
            case GomokuPlayer.EMPTY:
                return pygomoku.Player.EMPTY
            case GomokuPlayer.BLACK:
                return pygomoku.Player.BLACK
            case GomokuPlayer.WHITE:
                return pygomoku.Player.WHITE

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
    total_time: float
    turn_time: float

    def __init__(self):
        self.settings = pygomoku.GameRoomSettings()
        self.total_time = -1
        self.turn_time = -1

    def set_width(self, width: int):
        self.settings.width = width

    def get_width(self) -> int:
        return self.settings.width

    def set_height(self, height: int):
        self.settings.height = height

    def get_height(self) -> int:
        return self.settings.height

    def set_rule_style(self, ruleStyle: GomokuRuleStyle):
        # cast ruleStyle to int
        self.settings.rule_style = pygomoku.GameRoomRuleStyle(int(ruleStyle.value))

    def get_rule_style(self) -> GomokuRuleStyle:
        return GomokuRuleStyle(int(self.settings.rule_style))

    def set_capture(self, capture: bool):
        self.settings.capture = capture

    def get_capture(self) -> bool:
        return self.settings.capture

    def set_is_p1_ai(self, isAi: bool):
        self.settings.p1.is_ai = isAi

    def get_is_p1_ai(self) -> bool:
        return self.settings.p1.is_ai

    def set_is_p2_ai(self, isAi: bool):
        self.settings.p2.is_ai = isAi

    def get_is_p2_ai(self) -> bool:
        return self.settings.p2.is_ai

    @staticmethod
    def get_ai_names_list() -> List[str]:
        return pygomoku.get_ai_names_list()

    def set_p1_ai_name(self, name: str):
        self.settings.p1.ai_name = name

    def get_p1_ai_name(self) -> str:
        return self.settings.p1.ai_name
    
    def set_p2_ai_name(self, name: str):
        self.settings.p2.ai_name = name

    def get_p2_ai_name(self) -> str:
        return self.settings.p2.ai_name

    def set_total_time(self, total_time: float):
        self.total_time = total_time

    def get_total_time(self) -> float:
        return self.total_time

    def set_turn_time(self, turn_time: float):
        self.turn_time = turn_time

    def get_turn_time(self) -> float:
        return self.turn_time


class ExpectedAction:
    player: GomokuPlayer
    player_id: int
    action_type: pygomoku.GameActionType


class GameRoom:

    room: pygomoku.GameRoom

    time_since_start_turn: float
    players_time: Dict[GomokuPlayer, float]

    callback_removed: bool

    def __init__(self, settings: GameRoomSettings):
        self.room = pygomoku.GameRoom(settings.settings)
        self.players_time = {
            GomokuPlayer.WHITE: 0,
            GomokuPlayer.BLACK: 0,
        }
        self.time_since_start_turn = 0
        Clock.schedule_interval(self.update_time, 1 / 20)
        self.pending_action_task = None
        CallbackCenter.shared().add_callback(
            "GomokuGame.modified", self.broadcast_expected_action
        )
        self.callback_removed = False

    def __del__(self):
        self.remove_callbacks()

    def remove_callbacks(self):
        Clock.unschedule(self.update_time)
        CallbackCenter.shared().remove_callback(
            "GomokuGame.modified", self.broadcast_expected_action
        )
        if self.pending_action_task is not None:
            self.pending_action_task.cancel()
        self.callback_removed = True

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
        return self.room.get_color_score(player.to_pygomoku())

    def get_winner(self) -> GomokuPlayer:
        if self.room.get_game().is_game_over():
            return GomokuPlayer(self.room.get_game().get_winner())
        return GomokuPlayer.EMPTY

    def update_time(self, dt: float):
        self.time_since_start_turn += dt
        CallbackCenter.shared().send_message("GomokuGame.time", self)

    def get_player_time(self, player: GomokuPlayer) -> float:
        turn_time = self.time_since_start_turn if player == self.get_current_player() else 0
        return self.players_time[player] + turn_time

    def get_move_list(self) -> List[GomokuMove]:
        move_list = []
        for action in self.room.get_actions_history():
            if action.action_type == pygomoku.GameActionType.MOVE:
                move = GomokuMove()
                move.player = GomokuPlayer(
                    self.room.gomoku_player_from_id(action.player)
                )
                move.row = action.action_value.move.row
                move.column = action.action_value.move.col
                move.timestamp = action.timestamp
                move.move_result = action.action_value.move.result
                move_list.append(move)
        return move_list

    def handle_board_click(self, row: int, col: int):
        if self.room.has_pending_action():
            result = pygomoku.GameActionResult()
            result.success = False
            result.message = "Cannot play"
            CallbackCenter.shared().send_message("GameRoom.action", result)
            return
        playerid = self.room.expected_player()
        result = self.room.perform_action_move(playerid, row, col)
        CallbackCenter.shared().send_message("GameRoom.action", result)
        if result.success:
            CallbackCenter.shared().send_message("GomokuGame.modified", self)
        if self.room.has_pending_action():
            self.perform_pending_actions()
        elif self.room.get_game().is_game_over():
            CallbackCenter.shared().send_message("GomokuGame.gameover", self)

    def can_reverse_move(self):
        return self.room.can_reverse_last_action()

    def reverse_last_move(self):
        self.room.reverse_last_action()
        CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def can_reapply_move(self):
        return self.room.can_reapply_last_action()

    def reapply_last_move(self):
        self.room.reapply_last_action()
        CallbackCenter.shared().send_message("GomokuGame.modified", self)

    def get_move_index(self) -> int:
        return self.room.get_action_index()

    def broadcast_expected_action(self, _, __):
        expected = ExpectedAction()
        expected.player_id = self.room.expected_player()
        expected.player = GomokuPlayer(
            self.room.gomoku_player_from_id(expected.player_id)
        )
        expected.action_type = self.room.expected_action()
        CallbackCenter.shared().send_message("GomokuGame.expected_action", self)
        print("We expect ", expected.player, " to ", expected.action_type)
        self.time_since_start_turn = 0
        black_player_id = self.room.id_from_gomoku_player(GomokuPlayer.BLACK.to_pygomoku())
        white_player_id = self.room.id_from_gomoku_player(GomokuPlayer.WHITE.to_pygomoku())
        self.players_time = {
            GomokuPlayer.BLACK: self.room.get_player_timer(black_player_id),
            GomokuPlayer.WHITE: self.room.get_player_timer(white_player_id),
        }

    def perform_pending_actions(self):

        if self.room.has_pending_action() == False:
            return

        def task_done_callback(_):
            CallbackCenter.shared().send_message("GomokuGame.modified", self)
            if self.room.get_game().is_game_over():
                CallbackCenter.shared().send_message("GomokuGame.gameover", self)
            else:
                if self.callback_removed == False:
                    self.perform_pending_actions()

        self.pending_action_task = asyncio.create_task(
            self.perform_one_pending_action_async()
        )
        self.pending_action_task.add_done_callback(task_done_callback)

    async def perform_one_pending_action_async(self):
        loop = asyncio.get_running_loop()
        await loop.run_in_executor(None, self._perform_pending_action)

    def _perform_pending_action(self):
        result = self.room.perform_pending_action()
        print(result.message)
