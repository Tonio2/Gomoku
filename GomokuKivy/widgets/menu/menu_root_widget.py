from app.shared_object import SharedObject

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty
from kivy.core.window import Window
from core.gomoku_room import GomokuRuleStyle


class MenuRootWidget(Widget):

    def __init__(self, **kwargs):
        super(MenuRootWidget, self).__init__(**kwargs)

    def set_board_width(self, value):
        shared_object = SharedObject.get_instance()
        shared_object.room_settings.set_width(int(value))
        print("width: ", shared_object.room_settings.get_width())

    def set_board_height(self, value):
        shared_object = SharedObject.get_instance()
        shared_object.room_settings.set_height(int(value))
        print("height: ", shared_object.room_settings.get_height())

    def set_ai_mode(self, value):
        shared_object = SharedObject.get_instance()
        if value == "Human VS Human":
            shared_object.room_settings.set_is_p1_ai(False)
            shared_object.room_settings.set_is_p2_ai(False)
        elif value == "Human VS AI":
            shared_object.room_settings.set_is_p1_ai(False)
            shared_object.room_settings.set_is_p2_ai(True)
        elif value == "AI VS Human":
            shared_object.room_settings.set_is_p1_ai(True)
            shared_object.room_settings.set_is_p2_ai(False)
        elif value == "AI VS AI":
            shared_object.room_settings.set_is_p1_ai(True)
            shared_object.room_settings.set_is_p2_ai(True)
        print("is_p1_ai: ", shared_object.room_settings.get_is_p1_ai())
        print("is_p2_ai: ", shared_object.room_settings.get_is_p2_ai())

    def set_rule_style(self, value):
        shared_object = SharedObject.get_instance()
        rule_style = GomokuRuleStyle.STANDARD
        if value == "Standard":
            rule_style = GomokuRuleStyle.STANDARD
        elif value == "Pro":
            rule_style = GomokuRuleStyle.PRO
        elif value == "LongPro":
            rule_style = GomokuRuleStyle.LONG_PRO
        elif value == "Swap":
            rule_style = GomokuRuleStyle.SWAP
        elif value == "Swap2":
            rule_style = GomokuRuleStyle.SWAP2
        shared_object.room_settings.set_rule_style(rule_style)
        print("rule_style: ", shared_object.room_settings.get_rule_style())

    def toggle_capture(self, value):
        shared_object = SharedObject.get_instance()
        shared_object.room_settings.set_capture(value)
        print("capture: ", shared_object.room_settings.get_capture())

    def set_total_time(self, value):
        shared_object = SharedObject.get_instance()
        if value == "No limit":
            shared_object.room_settings.set_total_time(-1)
        else:
            shared_object.room_settings.set_total_time(float(value))
        print("total_time: ", shared_object.room_settings.get_total_time())

    def set_turn_time(self, value):
        shared_object = SharedObject.get_instance()
        if value == "No limit":
            shared_object.room_settings.set_turn_time(-1)
        else:
            shared_object.room_settings.set_turn_time(float(value))
        print("turn_time: ", shared_object.room_settings.get_turn_time())
