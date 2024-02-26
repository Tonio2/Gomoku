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

    def __init__(self):
        game = pygomoku.GomokuGame()

    def get_board_size(self) -> Tuple[int, int]:
        return (19, 19)

    def get_board_value_at(self, row: int, col: int) -> BoardValue:
        return BoardValue.EMPTY
