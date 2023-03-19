import gi

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, Gio, GObject
from kabegami import config

@Gtk.Template(resource_path=f"{config.RESOURCES}/theme_switcher.ui")
class ThemeSwitcher (Adw.Bin):

    __gtype_name__ = "ThemeSwitcherWidget"

    system: Gtk.CheckButton = Gtk.Template.Child()
    light: Gtk.CheckButton = Gtk.Template.Child()
    dark: Gtk.CheckButton = Gtk.Template.Child()

    settings = Gio.Settings.new(config.APP_ID)

    show_system = GObject.property(type=bool, default=True)

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)

        self.style_manager = Adw.StyleManager.get_default()

        self.style_manager.bind_property(
            "system-supports-color-schemes",
            self, "show-system"
        )

        self.theme = self.settings.get_int("theme")
        self.change_color_scheme()

    def change_color_scheme(self) -> None:
        
        color_scheme = Adw.ColorScheme.DEFAULT

        match self.theme:
            case 0: 
                if not self.system.get_active(): 
                    self.system.activate()
            case 1: 
                if not self.light.get_active(): self.light.activate()
                color_scheme = Adw.ColorScheme.FORCE_LIGHT
            case 2: 
                if not self.dark.get_active(): self.dark.activate()
                color_scheme = Adw.ColorScheme.FORCE_DARK

        self.style_manager.props.color_scheme = color_scheme

    @Gtk.Template.Callback()
    def on_color_scheme_changed(self, *args) -> None:

        if self.system.get_active(): self.theme = 0
        elif self.light.get_active(): self.theme = 1
        elif self.dark.get_active(): self.theme = 2
        else: return

        self.change_color_scheme()