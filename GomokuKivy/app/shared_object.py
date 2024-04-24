from core.gomoku_room import GameRoom, GameRoomSettings


class SharedObject:

    room_settings: GameRoomSettings
    room: GameRoom

    def __init__(self):
        self.room = None
        self.room_settings = GameRoomSettings()

    def create_room(self):
        self.room = GameRoom(self.room_settings)

    def clear_room(self):
        if self.room is not None:
            self.room.remove_callbacks()
        self.room = None

    def get_room(self):
        return self.room

    def get_game(self):
        return self.room.get_game()

    # Singleton Pattern

    _instance = None

    @classmethod
    def get_instance(cls):
        if SharedObject._instance is None:
            SharedObject._instance = SharedObject()
        return SharedObject._instance
