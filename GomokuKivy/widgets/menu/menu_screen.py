from kivy.uix.screenmanager import ScreenManager, Screen

from widgets.menu.menu_root_widget import MenuRootWidget

from app.shared_object import SharedObject
from core.callback_center import CallbackCenter


class MenuScreen(Screen):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def switch_to_game(self):
        SharedObject.get_instance().create_room()
        self.manager.current = "game"
        CallbackCenter.shared().send_message("GomokuGame.start", SharedObject.get_instance().get_room())
