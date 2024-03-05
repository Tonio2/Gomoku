

from app.shared_object import SharedObject
from kivy.uix.screenmanager import ScreenManager, Screen
from widgets.game.game_root_widget import GameRootWidget
from core.callback_center import CallbackCenter

class GameScreen(Screen):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.root = GameRootWidget()

    def back_to_menu(self):
        self.manager.current = 'menu'

    def on_enter(self):
        CallbackCenter.shared().send_message("Application.draw", None)
