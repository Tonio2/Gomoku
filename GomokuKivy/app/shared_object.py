
from core.gomoku_game import GomokuGame

class SharedObject:

    _size: int
    _game: GomokuGame
    _player_time: float

    def __init__(self):
        self._player_time = 300
        self.set_size(19)

    def set_size(self, size):
        self._size = size
        self.reset_game()

    def reset_game(self):
        self._game = GomokuGame(self._size, self._player_time)

    def get_game(self):
        return self._game

    # Singleton Pattern

    _instance = None

    @classmethod
    def get_instance(cls):
        if SharedObject._instance is None:
            SharedObject._instance = SharedObject()
        return SharedObject._instance
