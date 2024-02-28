from typing import Tuple
from enum import Enum
import sys

sys.path.append("../lib")
import pygomoku

class BoardValue(Enum):
    EMPTY = 0
    WHITE = 1
    BLACK = 2

class GomokuGame:

    def __init__(self, size: int):
        self.game = pygomoku.GomokuGame(size)

    def get_board_size(self) -> Tuple[int, int]:
        size = self.game.get_board_size()
        return (size, size)

    def get_board_value_at(self, row: int, col: int) -> BoardValue:
        value = self.game.get_board_value(row, col)
        return BoardValue(value)
