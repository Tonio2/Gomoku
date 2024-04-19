from enum import Enum

BLACK = 0
WHITE = 1

class Action(Enum):
    MOVE = 0
    SWAP = 1
    
class RuleStyle(Enum):
    STANDARD = 0
    PRO = 1
    SWAP = 2

class Mode(Enum):
    PVAI = 0
    PVP = 1
