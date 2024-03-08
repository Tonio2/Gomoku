from app.gomoku_app import GomokuApp

from core.callback_center import CallbackCenter

if __name__ == '__main__':
    # CallbackCenter.shared().add_callback("GomokuGame.modified", lambda message, game: game.game.display_struct())
    GomokuApp().run()
