
from kivy.uix.screenmanager import ScreenManager, Screen

from widgets.menu.menu_root_widget import MenuRootWidget

from app.shared_object import SharedObject

class MenuScreen(Screen):
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.root = MenuRootWidget()

    def switch_to_game(self):
        SharedObject.get_instance().reset_game()
        self.manager.current = 'game'
