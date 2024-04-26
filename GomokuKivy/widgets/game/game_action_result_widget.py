from kivy.uix.widget import Widget
from kivy.uix.label import Label
from kivy.graphics import Color, Rectangle, Ellipse
from kivy.clock import Clock
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty

from app.shared_object import SharedObject
from core.gomoku_room import GameRoom, GomokuPlayer

from core.callback_center import CallbackCenter


class GameActionResultWidget(Widget):

    action_label: Label

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        CallbackCenter.shared().add_callback("GameRoom.action", self.on_action_result)
        self.action_label = Label()
        self.action_label.multiline = True
        self.action_label.color = (0.9, 0.1, 0.1)
        self.add_widget(self.action_label)
        self.hide(None)
        self.clock = None

    def __del__(self):
        CallbackCenter.shared().remove_callback(
            "GameRoom.action", self.on_action_result
        )

    def on_action_result(self, _, action):
        if action.success:
            self.hide(None)
            return

        self.opacity = 1
        self.action_label.text = action.message
        self.action_label.texture_update()
        self.action_label.size = self.action_label.texture_size
        self.size = self.action_label.size
        self.action_label.pos = self.pos

        if self.clock is not None:
            self.clock.cancel()
        self.clock = Clock.schedule_once(self.hide, 1.5)

    def hide(self, _):
        self.opacity = 0
