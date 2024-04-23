import sys

sys.path.append("../lib")
import pygomoku # type: ignore

MODE_PVAI = 0
MODE_PVP = 1

RULE_STANDARD = 0
RULE_PRO = 1
RULE_SWAP = 2

COLOR_EMPTY = 0
COLOR_BLACK = 1
COLOR_WHITE = 2
def server_color(color):
    color_table = {
            pygomoku.Player.EMPTY: COLOR_EMPTY,
            pygomoku.Player.BLACK: COLOR_BLACK,
            pygomoku.Player.WHITE: COLOR_WHITE
            }
    return color_table[color]

def pygomoku_color(color):
    return [pygomoku.Player.EMPTY, pygomoku.Player.BLACK, pygomoku.Player.WHITE][color]

PLAYER_1 = 1
PLAYER_2 = 2

ACTION_MOVE = 0
ACTION_SWAP = 1

class GomokuRoom:
    def __init__(self, size, mode, rule_style, ai_player):
        """Init room
            Parameters:
            size (int): Between 10 and 25
            mode (int): PVP | PVAI
            rule_style (int): Standard | Pro | Swap
            ai_player (int): 0 | 1

            All variable in self are in pygomoku format

            All getter returns in python format
        """
        if size < 10 or size > 25:
            raise Exception("Size must be betwwen 10 and 25")
        if not mode in [0,1]:
            raise Exception("Mode must be Human vs AI or Human vs Human")
        if not rule_style in [0, 1, 2]:
            raise Exception("Rule style must be Standard, Pro or Swap")
        if mode == 0 and not ai_player in [0, 1]:
            raise Exception("Ai player must first or second")
        self.size = size
        self.mode = mode
        self.rule_style = pygomoku.GameRoomStyle(rule_style)
        self.ai_player = ai_player
        players = [{
            "is_ai": False,
            "depth": 0,
        },
        {
            "is_ai": False,
            "depth": 0,
        }]
        if mode == MODE_PVAI:
            players[ai_player]["is_ai"] = True
            players[ai_player]["depth"] = 3
        game_room_settings = pygomoku.GameRoomSettings(size, size, self.rule_style, players[0], players[1])
        self.room = pygomoku.GameRoom(game_room_settings)
        self.players = [{}, players[0], players[1]]
        self.update_state()

    def update_state(self):
        self.action_index = self.room.get_action_index()
        self.list_moves = self.room.get_actions_history()
        
        pygomoku_color = self.room.gomoku_player_from_id(PLAYER_1)
        self.players[PLAYER_1]["color"] = pygomoku_color
        self.players[PLAYER_1]["score"] = self.room.get_color_score(pygomoku_color)
        
        pygomoku_color = self.room.gomoku_player_from_id(PLAYER_2)
        self.players[PLAYER_2]["color"] = pygomoku_color
        self.players[PLAYER_2]["score"] = self.room.get_color_score(pygomoku_color)

        self.next_player = self.room.expected_player()
        self.next_action = self.expected_action()

    def get_board(self):
        return self.room.get_game().get_board()

    def is_game_over(self):
        return self.room.get_game().is_game_over()

    def get_winner(self):
        winner_color = self.room.get_game.get_winner()
        if winner_color == pygomoku.Player.EMPTY:
            return 0
        elif winner_color == self.players[PLAYER_1]["color"]:
            return PLAYER_1
        else:
            return PLAYER_2

    def get_next_player(self):
        return self.next_player
    
    def get_next_action(self):
        table = {
            pygomoku.GameActionType.MOVE: ACTION_MOVE,
            pygomoku.GameActionType.SWAP: ACTION_SWAP
        }
        return table[self.next_action]

    def get_list_moves(self):
        moves = []
        for move in self.list_moves:
            # Player X played COLOR at x,y
            # Player X swapped
            action = "Player " + str(move.player)
            if move.action_type == pygomoku.GameActionType.MOVE:
                action += " played "
                color = "BLACK" if self.players[move.player]["color"] == pygomoku.Player.BLACK else "WHITE"
                action += color
                action += " at " + str(move.action_value.move.row) + str(move.action_value.move.col)
            else:
                if move.action_value.swap.did_swap:
                    action += " swapped color"
                else:
                    action += " didn't swap color"
            moves.append(action)
        return moves

    def get_current_move(self):
        return self.action_index

    def get_players(self):
        players = self.players
        players[PLAYER_1]["color"] = server_color(players[PLAYER_1]["color"])
        players[PLAYER_2]["color"] = server_color(players[PLAYER_2]["color"])
        return players

    def get_state(self):
        return {
            "_board": self.get_board(),
            "_isGameOver": self.is_game_over(),
            "_winner": self.get_winner(),
            "_nextPlayer": self.get_next_player(),
            "_nextAction": self.get_next_action(),
            "_listMoves": self.get_list_moves(),
            "_currentMove": self.get_current_move(),
            "_players": self.get_players(),
        }


    def make_move(self, row, col):
        self.room.perform_action_move(self.next_player, row, col)

    def reverse_last_action(self):
        self.room.reverse_last_action()

    def reapply_last_action(self):
        self.room.reapply_last_action()
