import sys
sys.path.append("./gkserver/lib")
import pygomoku

class GameRoom:

    settings: pygomoku.GameRoomSettings
    room: pygomoku.GameRoom

    def __init__(self):
        self.settings = pygomoku.GameRoomSettings()
        self.room = pygomoku.GameRoom(self.settings)
