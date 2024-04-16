

from app.shared_object import SharedObject
from kivy.uix.screenmanager import ScreenManager, Screen
from widgets.game.game_root_widget import GameRootWidget

from kivy.uix.modalview import ModalView
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label

from core.callback_center import CallbackCenter
from core.gomoku_room import GameRoom, GomokuPlayer

class GameScreen(Screen):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.root = GameRootWidget()
        CallbackCenter.shared().add_callback("GomokuGame.gameover", self.on_game_over)

    def back_to_menu(self):
        self.manager.current = 'menu'

    def on_enter(self):
        CallbackCenter.shared().send_message("Application.draw", None)

    def on_game_over(self, _, room: GameRoom):
        self.end_game_popup = ModalView(size_hint=(None, None), size=(300, 100))
        text = "Draw"
        winner = room.get_winner()
        if winner != GomokuPlayer.EMPTY:
            text = f'{winner.to_str()} wins !'

        self.end_game_popup.add_widget(Button(text=text, on_release=self.close_popup))
        self.end_game_popup.open()
        # for widget in self.children:
        #     widget.disabled = True

    def close_popup(self, instance):
        self.end_game_popup.dismiss()
        for widget in self.children:
            widget.disabled = False
        self.back_to_menu()
        SharedObject.get_instance().clear_room()
