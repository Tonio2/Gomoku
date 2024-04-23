import sys

sys.path.append("../lib")
import pygomoku # type: ignore

MODE_PVAI = 0
MODE_PVP = 1

RULE_STANDARD = 0
RULE_PRO = 1
RULE_SWAP = 2

class GomokuRoom:
    def __init__(self, size, mode, rule_style, ai_player):
        """Init room
            Parameters:
            size (int): Between 10 and 25
            mode (int): PVP | PVAI
            rule_style (int): Standard | Pro | Swap
            ai_player (int): 0 | 1
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
        self.rule_Style = rule_style
        self.ai_player = ai_player
        self.players = [{
            "is_ai": false,
            "depth": 0,
        },
        {
            "is_ai": false,
            "depth": 0,
        }]
        if mode == MODE_PVAI:
            self.players[ai_player]["is_ai"] = True
            self.players[ai_player]["depth"] = 3
        game_room_settings = pygomoku.GameRoomSettings(size, size, pygomoku.GameRuleStyle(rule_style), p1, p2)
        self.room = pygomoku.GameRoom(game_room_settings)

        

        

