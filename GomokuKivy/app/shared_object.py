
from core.gomoku_game import GomokuGame

class SharedObject:

    _width: int
    _height: int
    _mode: int
    _game: GomokuGame
    _player_time: float

    def __init__(self):
        self._player_time = 300
        self._mode = 0
        self._width = 19
        self._height = 19
        self.reset_game()

    def set_size(self, width, height):
        self._width = width
        self._height = height
        self.reset_game()
    
    def set_mode(self, mode):
        self._mode = mode
        self.reset_game()

    def reset_game(self):
        self._game = GomokuGame(self._width, self._height, self._player_time, self._mode)

    def get_game(self):
        return self._game

    # Singleton Pattern

    _instance = None

    @classmethod
    def get_instance(cls):
        if SharedObject._instance is None:
            SharedObject._instance = SharedObject()
        return SharedObject._instance
