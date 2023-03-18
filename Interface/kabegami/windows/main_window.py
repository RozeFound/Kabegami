import gi

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw
from kabegami import config

from kabegami.views.not_implemented import NotImplemented

from kabegami.widgets.theme_switcher import ThemeSwitcher

@Gtk.Template(resource_path="/com/github/RozeFound/Kabegami/window.ui")
class MainWindow(Adw.ApplicationWindow):

    __gtype_name__ = 'MainWindow'

    options_menu: Gtk.PopoverMenu = Gtk.Template.Child()
    stack: Adw.ViewStack = Gtk.Template.Child()

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.app = kwargs.get("application")

        if config.BUILD_TYPE == "dev":
            self.add_css_class("devel")

        theme_switcher = ThemeSwitcher()
        self.options_menu.add_child(theme_switcher, "theme")

        self.populate_stack()

    def populate_stack(self) -> None:

        self.page_installed = NotImplemented()
        self.page_browse = NotImplemented()
        self.page_workshop = NotImplemented()

        self.stack.add_titled(self.page_installed, "page_installed", "Installed").set_use_underline(True)
        self.stack.add_titled(self.page_browse, "page_browse", "Browse").set_use_underline(True)
        self.stack.add_titled(self.page_workshop, "page_workshop", "Workshop").set_use_underline(True)
        

