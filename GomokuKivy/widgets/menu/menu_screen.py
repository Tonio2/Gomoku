
from kivy.uix.screenmanager import ScreenManager, Screen

from widgets.menu.menu_root_widget import MenuRootWidget

class MenuScreen(Screen):
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.root = MenuRootWidget()

    def switch_to_game(self):
        self.manager.current = 'game'
