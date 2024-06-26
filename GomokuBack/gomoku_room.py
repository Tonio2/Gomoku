import sys

sys.path.append("../lib")
import pygomoku  # type: ignore

MODE_PVAI = 0
MODE_PVP = 1
MODE_AIVAI = 2

RULE_STANDARD = 0
RULE_PRO = 1
RULE_SWAP = 2

COLOR_EMPTY = 0
COLOR_BLACK = 1
COLOR_WHITE = 2


def pygomoku_color(color):
    return [pygomoku.Player.EMPTY, pygomoku.Player.BLACK, pygomoku.Player.WHITE][color]


PLAYER_1 = 1
PLAYER_2 = 2

ACTION_MOVE = 0
ACTION_SWAP = 1

BOARD_COORDINATES = [
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
]


class RoomError(Exception):
    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg

    def __str__(self):
        return self.msg


def format_players(players):
    p1 = players[PLAYER_1]
    p2 = players[PLAYER_2]

    return [
        {
            "color": p1["color"] - 1,
            "score": p1["score"],
            "time": p1["time"],
            "isAI": p1["is_ai"],
        },
        {
            "color": p2["color"] - 1,
            "score": p2["score"],
            "time": p2["time"],
            "isAI": p2["is_ai"],
        },
    ]


def format_actions_history(actions_history, players):
    actions = []
    for action in actions_history:
        # Player X played COLOR at x,y
        # Player X swapped
        descr = "Player " + str(action.player)
        if action.action_type == pygomoku.GameActionType.MOVE:
            descr += " played "
            color = (
                "BLACK"
                if players[action.player]["color"] == pygomoku.Player.BLACK
                else "WHITE"
            )
            descr += color
            descr += (
                " at "
                + BOARD_COORDINATES[action.action_value.move.row]
                + BOARD_COORDINATES[action.action_value.move.col]
            )
        else:
            if action.action_value.swap.did_swap:
                descr += " swapped color"
            else:
                descr += " didn't swap color"
        actions.append(descr)
    return actions


def room_settings(size, mode, rule_style, ai_player, ai_name, ai_name2):
    player = {
        "is_ai": False,
        "ai_name": "default",
    }
    players = [player, player.copy()]
    if mode == MODE_PVAI:
        players[ai_player]["is_ai"] = True
        players[ai_player]["ai_name"] = ai_name
    if mode == MODE_AIVAI:
        players[0]["is_ai"] = True
        players[0]["ai_name"] = ai_name
        players[1]["is_ai"] = True
        players[1]["ai_name"] = ai_name2
    game_room_settings = pygomoku.GameRoomSettings()
    game_room_settings.width = size
    game_room_settings.height = size
    game_room_settings.rule_style = pygomoku.GameRoomRuleStyle(rule_style)
    game_room_settings.p1 = pygomoku.GameEntitySetting()
    game_room_settings.p1.is_ai = players[0]["is_ai"]
    game_room_settings.p1.ai_name = players[0]["ai_name"]
    game_room_settings.p2 = pygomoku.GameEntitySetting()
    game_room_settings.p2.is_ai = players[1]["is_ai"]
    game_room_settings.p2.ai_name = players[1]["ai_name"]
    return game_room_settings


def format_move_sequence(move_sequence):
    return [[move.row, move.col] for move in move_sequence]


class GomokuRoom:
    def __init__(
        self, size, mode, rule_style, ai_player, ai_name, ai_name2, init_moves=""
    ):
        """Init room
        Parameters:
        size (int): Between 10 and 25
        mode (int): PVP | PVAI
        rule_style (int): Standard | Pro | Swap
        ai_player (int): 0 | 1

        All variable in self are in pygomoku format

        All getter returns in python format

        Get state returns in python format with correct indexation
        """
        if size < 10 or size > 25:
            raise RoomError("Size must be betwwen 10 and 25")
        if not mode in [0, 1, 2]:
            raise RoomError("Mode must be Human vs AI or Human vs Human or AI vs AI")
        if not rule_style in [0, 1, 2, 3, 4]:
            raise RoomError("Rule style must be Standard, Pro, Long Pro, Swap or Swap2")
        if mode == 0 and not ai_player in [0, 1]:
            raise RoomError("Ai player must first or second")
        self.size = size
        self.mode = mode
        self.rule_style = pygomoku.GameRoomRuleStyle(rule_style)
        self.ai_player = ai_player
        self.ai_name = ai_name
        self.ai_name2 = ai_name2

        self.room = pygomoku.GameRoom(
            room_settings(size, mode, rule_style, ai_player, ai_name, ai_name2)
        )

        if init_moves != "":
            moves = init_moves.split(",")
            for move in moves:
                if len(move) != 2:
                    raise RoomError("Invalid move")
                row = BOARD_COORDINATES.index(move[0])
                col = BOARD_COORDINATES.index(move[1])
                action_result = self.room.perform_action_move(
                    self.get_next_player(), row, col
                )
                if not action_result.success:
                    raise RoomError(action_result.message)

    def reset(self):
        self.room = pygomoku.GameRoom(
            room_settings(
                self.size,
                self.mode,
                self.rule_style,
                self.ai_player,
                self.ai_name,
                self.ai_name2,
            )
        )

    def get_players(self):
        players = [{}, {}, {}]

        pygomoku_color = self.room.gomoku_player_from_id(PLAYER_1)
        players[PLAYER_1]["color"] = int(pygomoku_color)
        players[PLAYER_1]["score"] = self.room.get_color_score(pygomoku_color)
        players[PLAYER_1]["time"] = self.room.get_player_timer(PLAYER_1)
        players[PLAYER_1]["is_ai"] = self.room.get_settings().p1.is_ai

        pygomoku_color = self.room.gomoku_player_from_id(PLAYER_2)
        players[PLAYER_2]["color"] = int(pygomoku_color)
        players[PLAYER_2]["score"] = self.room.get_color_score(pygomoku_color)
        players[PLAYER_2]["time"] = self.room.get_player_timer(PLAYER_2)
        players[PLAYER_2]["is_ai"] = self.room.get_settings().p2.is_ai

        return players

    def get_board(self):
        return self.room.get_game().get_board()

    def is_game_over(self):
        is_game_over = self.room.get_game().is_game_over()
        if is_game_over:
            players = self.get_players()
            cur_move = self.get_current_move()
            player1_move_count = cur_move // 2 + cur_move % 2
            player2_move_count = cur_move // 2
            player1_time = players[PLAYER_1]["time"]
            player2_time = players[PLAYER_2]["time"]
            player1_avg_time = player1_time / player1_move_count
            player2_avg_time = player2_time / player2_move_count
            print("Player 1 time average: ", player1_avg_time)
            print("Player 2 time average: ", player2_avg_time)
        return is_game_over

    def get_winner_id(self):
        winner_color = self.room.get_game().get_winner()
        players = self.get_players()
        if winner_color == pygomoku.Player.EMPTY:
            return 0
        elif winner_color == players[PLAYER_1]["color"]:
            return PLAYER_1
        else:
            return PLAYER_2

    def get_next_player(self):
        return self.room.expected_player()

    def get_next_action(self):
        return int(self.room.expected_action())

    def get_actions_history(self):
        return self.room.get_actions_history()

    def get_current_move(self):
        return self.room.get_action_index()

    def has_pending_action(self):
        return self.room.has_pending_action()

    def get_suggestion(self):
        move_sequence = self.room.suggest_move()
        return format_move_sequence(move_sequence)

    def display_move_history(self):
        moves = ""
        actions = self.get_actions_history()
        for action in actions:
            if action.action_type == pygomoku.GameActionType.MOVE:
                moves += (
                    BOARD_COORDINATES[action.action_value.move.row]
                    + BOARD_COORDINATES[action.action_value.move.col]
                    + ","
                )
        print(moves)

    def get_state(self):
        players = self.get_players()
        return {
            "_board": self.get_board(),
            "_isGameOver": self.is_game_over(),
            "_winner": self.get_winner_id() - 1,
            "_nextPlayer": self.get_next_player() - 1,
            "_nextAction": self.get_next_action(),
            "_listMoves": format_actions_history(self.get_actions_history(), players),
            "_currentMove": self.get_current_move() + 1,
            "_players": format_players(players),
            "_hasPendingAction": self.has_pending_action(),
        }

    def make_move(self, row, col):
        if self.has_pending_action():
            raise RoomError("You cannot play AI's move")
        action_result = self.room.perform_action_move(self.get_next_player(), row, col)
        if not action_result.success:
            raise RoomError(action_result.message)
        self.display_move_history()
        print("Move evaluation for next player: ", self.room.get_heuristic_evaluation())
        self.room.get_game().print_patterns()

    def swap(self, swap):
        self.room.perform_action_swap(self.get_next_player(), swap)

    def reverse_last_action(self):
        self.room.reverse_last_action()

    def reapply_last_action(self):
        self.room.reapply_last_action()

    def perform_pending_action(self):
        action_result = self.room.perform_pending_action()
        if not action_result.success:
            raise RoomError(action_result.message)
        self.display_move_history()


class OnlineRoom(GomokuRoom):
    def __init__(self, size, rule_style):
        GomokuRoom.__init__(self, size, 1, rule_style, 0)
        self.ip_addresses = ["", "", ""]

    def is_room_ready(self):
        if "" in self.ip_addresses[1:]:
            return False
        return True

    def make_move(self, ip, row, col):
        if not self.is_room_ready():
            raise RoomError("Not all players have joined")
        if ip != self.ip_addresses[self.get_next_player()]:
            raise RoomError("Not your turn")
        GomokuRoom.make_move(self, row, col)

    def swap(self, ip, swap):
        if not self.is_room_ready():
            raise RoomError("Not all players have joined")
        if ip != self.ip_addresses[self.get_next_player()]:
            raise RoomError("Not your turn")
        GomokuRoom.swap(self, swap)

    def connect(self, ip, player_id):
        if self.ip_addresses[player_id] not in ["", ip]:
            raise RoomError("Someone already took this role")
        if ip in self.ip_addresses:
            raise RoomError("You already took a role")
        self.ip_addresses[player_id] = ip

    def disconnect(self, ip):
        if not ip in self.ip_addresses:
            return 0

        player_id = self.ip_addresses.index(ip)
        self.ip_addresses[player_id] = ""
        return player_id

    def get_available_roles(self):
        return [ip == "" for ip in self.ip_addresses]

    def get_player_id_from_ip(self, ip):
        if ip in self.ip_addresses:
            return self.ip_addresses.index(ip)
        return 0

    def get_state(self):
        state = GomokuRoom.get_state(self)
        state["_isGameReady"] = self.is_room_ready()  # TODO: remove ?
        return state
