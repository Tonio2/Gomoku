
from app.shared_object import SharedObject

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.core.window import Window

class MenuRootWidget(Widget):

    def __init__(self, **kwargs):
        super(MenuRootWidget, self).__init__(**kwargs)

    def set_board_size(self, value):
        shared_object = SharedObject.get_instance()
        shared_object.set_size(int(value))
