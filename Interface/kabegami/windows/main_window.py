import gi

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, Gio
from gettext import gettext as _

from kabegami import config
from kabegami.views.not_implemented import NotImplemented
from kabegami.widgets.theme_switcher import ThemeSwitcher

@Gtk.Template(resource_path=f"{config.RESOURCES}/window.ui")
class MainWindow(Adw.ApplicationWindow):

    __gtype_name__ = 'MainWindow'

    options_menu: Gtk.PopoverMenu = Gtk.Template.Child()
    stack: Adw.ViewStack = Gtk.Template.Child()

    settings = Gio.Settings.new(config.APP_ID)

    def __init__(self, **kwargs):
        super().__init__(**kwargs,
        default_width=self.settings.get_int("width"),
        default_height=self.settings.get_int("height"))

        self.app = kwargs.get("application")

        if config.BUILD_TYPE == "dev":
            self.add_css_class("devel")

        self.theme_switcher = ThemeSwitcher()
        self.options_menu.add_child(self.theme_switcher, "theme")

        self.populate_stack()

    @Gtk.Template.Callback()
    def on_close_request(self, *args) -> None:

        self.settings.set_int("width", self.get_width())
        self.settings.set_int("height", self.get_height())

        self.settings.set_int("theme", self.theme_switcher.theme)

    def populate_stack(self) -> None:

        self.page_installed = NotImplemented()
        self.page_browse = NotImplemented()
        self.page_workshop = NotImplemented()

        self.stack.add_titled_with_icon(self.page_installed, 
        "page_installed", _("Installed"), "folder-symbolic")
        self.stack.add_titled_with_icon(self.page_browse, 
            "page_browse", _("Browse"), "web-browser-symbolic")
        self.stack.add_titled_with_icon(self.page_workshop, 
            "page_workshop", _("Workshop"), "com.valvesoftware.Steam")
        

