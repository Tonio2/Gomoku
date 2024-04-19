import sys
from constants import *

sys.path.append("../lib")
import pygomoku # type: ignore

def serialize_player(player):
    if player == pygomoku.Player.BLACK:
        return 1
    elif player == pygomoku.Player.WHITE:
        return 2
    else:
        return 0
    
def serialize_move_result(move_result):
    cell_changes = []
    for cell_change in move_result.cell_changes:
        cell_changes.append(
            {
                "row": cell_change.row,
                "col": cell_change.col,
                "old_value": serialize_player(cell_change.old_value),
                "new_value": serialize_player(cell_change.new_value),
            }
        )
    return {
        "cell_changes": cell_changes,
        "white_score_change": move_result.white_score_change,
        "black_score_change": move_result.black_score_change,
    }


def serialize_move_evaluation(moveEvaluation):
    if moveEvaluation.listMoves is None:
        return {
            "move": (moveEvaluation.move[0], moveEvaluation.move[1]),
            "score": moveEvaluation.score,
        }
    listMoves = []
    for move in moveEvaluation.listMoves:
        listMoves.append(serialize_move_evaluation(move))
    return {
        "move": (moveEvaluation.move[0], moveEvaluation.move[1]),
        "score": moveEvaluation.score,
        "list_moves": listMoves,
    }

class GomokuGame:
    def __init__(self, mode, rule_style, ai_is_first, size=19):
        self.game = pygomoku.GomokuGame(size, size)
        self.next_player = 0
        self.next_action = Action.MOVE
        self.mode = mode
        self.rule_style = rule_style
        self.list_moves = []
        self.current_move = 0
        self.players = [
            {
                "color": BLACK,
                "isAI": mode == Mode.PVAI and ai_is_first,
                "score": 0,
                "time": 0,
            },
            {
                "color": WHITE,
                "isAI": mode == Mode.PVAI and not ai_is_first,
                "score": 0,
                "time": 0,
            },
        ]
        self.size = size
        
    def is_three_cell_away_from_center(self, row, col):
        row_dist = abs(row - self.size / 2)
        col_dist = abs(col - self.size / 2)
        return row_dist >= 3 and col_dist >= 3
    
    def is_move_legal(self, row, col):
        if RuleStyle == RuleStyle.PRO:
            if self.current_move == 0:
                return row == self.size / 2 and col == self.size / 2, "Move must be in the center of the board"
            if self.current_move == 1:
                return self.is_three_cell_away_from_center(row, col), "Move must be at least 3 cells away from center"
        return True, ""
            

    def make_move(self, row, col):
        legal, msg = self.is_move_legal(row,col)
        if not legal:
            return False, msg 
        try:
            move_result = self.game.make_move(row, col)
            # Determine next player and next action and current move
            if self.rule_style == RuleStyle.SWAP and self.current_move <= 2:
                if self.current_move == 0 or self.current_move == 1:
                    # player does not change
                    self.next_action = Action.MOVE
                if self.current_move == 2:
                    self.next_action = Action.SWAP
                    self.next_player = 1 - self.next_player
            else:
                self.next_player = 1 - self.next_player
                self.next_action = Action.MOVE
            
            # Update list_moves
            self.list_moves = self.list_moves[:self.current_move] + [{
                "row": row,
                "col": col,
                "move_result": move_result,
            }]
            self.current_move += 1
            return True, ""
        except Exception as e:
            return False, str(e)
        
    def reverse_move(self):
        if self.current_move == 0:
            return False, ""
        if self.rule_style == RuleStyle.SWAP and self.current_move == 3:
            return False, ""
        try:
            move_result = self.list_moves[self.current_move - 1]["move_result"]
            self.game.reverse_move(move_result)
            self.next_player = 1 - self.next_player
            self.current_move -= 1
            return True, ""
        except Exception as e:
            return False, str(e)

    def reapply_move(self):
        if self.current_move == len(self.list_moves):
            return False, ""

        try:
            move_result = self.list_moves[self.current_move]["move_result"]
            self.game.reapply_move(move_result)
            self.next_player = 1 - self.next_player
            self.current_move += 1
            return True, ""
        except Exception as e:
            return False, str(e)

    def get_board(self):
        return self.game.get_board()
    
    def get_size(self):
        return self.game.get_board_width()

    def get_winner(self):
        return serialize_player(self.game.get_winner())

    def is_game_over(self):
        return self.game.is_game_over()

    def get_next_player(self):
        return self.next_player

    def get_game(self):
        return self.game
    
    def get_list_moves(self):
        return [{"row": move["row"], "col": move["col"]} for move in self.list_moves]

    def reset(self):
        self.game = pygomoku.GomokuGame(self.size, self.size)
        self.next_player = 0
        
    def get_state(self):
        return {
            "_board": self.get_board(),
            "_isGameOver": self.is_game_over(),
            "_winner": self.get_winner(),
            "_nextPlayer": self.get_next_player(),
            "_nextAction": self.next_action.value,
            "_listMoves": self.get_list_moves(),
            "_currentMove": self.current_move,
            "_players": self.players,
        }
    
    def get_suggestion(self, depth, length):
        ai = pygomoku.GomokuAI(self.game, pygomoku.Player(self.next_player + 1), depth, length)
        return serialize_move_evaluation(ai.suggest_move())
