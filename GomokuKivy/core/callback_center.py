from typing import Any, Callable, Dict, List

class CallbackCenter:

    _callbacks: Dict[str, List[Callable[[str, Any], None]]]

    def __init__(self):
        self._callbacks = {}

    def add_callback(self, message: str, callback: Callable[[str, Any], None]) -> None:
        if message not in self._callbacks:
            self._callbacks[message] = []
        self._callbacks[message].append(callback)

    def remove_callback(self, message: str, callback: Callable[[str, Any], None]) -> None:
        if message in self._callbacks and callback in self._callbacks[message]:
            self._callbacks[message].remove(callback)

    def remove_all_callbacks(self, message: str) -> None:
        if message in self._callbacks:
            self._callbacks[message] = []

    def send_message(self, message: str, payload: Any) -> None:
        if message in self._callbacks:
            for callback in self._callbacks[message]:
                callback(message, payload)

    # singleton

    _shared = None

    @classmethod
    def shared(cls) -> "CallbackCenter":
        if cls._shared is None:
            cls._shared = cls()
        return cls._shared