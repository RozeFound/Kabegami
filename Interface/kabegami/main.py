import gi
from typing import Callable

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, Gio
from kabegami.windows.main_window import MainWindow
from kabegami import config

class Kabegami(Adw.Application):

    def __init__(self) -> None:
        super().__init__(application_id=config.APP_ID)

        self.create_action("preferences", self.show_preferences)
        self.create_action("about", self.show_about)
        self.create_action("exit", lambda *_: quit(), ["<primary>q"])

    def create_action(self, name: str, callback: Callable, shortcuts: list[str] = None) -> None:

        action = Gio.SimpleAction.new(name, None)
        action.connect("activate", callback)
        self.add_action(action)

        if shortcuts: self.set_accels_for_action(f"app.{name}", shortcuts)

    def show_preferences(self, *args) -> None:
        print("app.preferences callback activated")

    def show_about(self, *args) -> None:
        
        builder = Gtk.Builder.new_from_resource("/com/github/RozeFound/Kabegami/about.ui")

        about = builder.get_object("about_window")
        about.set_application_name(config.APP_NAME)
        about.set_application_icon(config.APP_ICON)
        about.set_version(config.APP_VERSION)

        about.set_transient_for(self.window)
        about.present()

    def do_activate(self) -> None:

        Adw.Application.do_activate(self)
        window = self.props.active_window
        if not window: window = MainWindow(application=self)
        self.window = window
        window.present()

def main(version: str, argv: list[str]) -> int:
    app = Kabegami()
    return app.run(argv)

