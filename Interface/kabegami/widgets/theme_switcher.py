import gi

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, GObject

@Gtk.Template(resource_path="/com/github/RozeFound/Kabegami/theme_switcher.ui")
class ThemeSwitcher (Adw.Bin):

    __gtype_name__ = "ThemeSwitcherWidget"

    system = Gtk.Template.Child()
    light = Gtk.Template.Child()
    dark = Gtk.Template.Child()

    show_system = GObject.property(type=bool, default=True)

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)

        self.style_manager = Adw.StyleManager.get_default()

        self.style_manager.bind_property(
            'system-supports-color-schemes',
            self, 'show_system'
        )

    def change_color_scheme(self, color_scheme: str) -> None:

        new_color_scheme = Adw.ColorScheme.DEFAULT

        match color_scheme:
            case "light": new_color_scheme = Adw.ColorScheme.FORCE_LIGHT
            case "dark": new_color_scheme = Adw.ColorScheme.FORCE_DARK

        self.style_manager.props.color_scheme = new_color_scheme

    @Gtk.Template.Callback()
    def on_color_scheme_changed(self, *args) -> None:

        if self.system.props.active:
            self.change_color_scheme("auto")
        if self.light.props.active:
            self.change_color_scheme("light")
        if self.dark.props.active:
            self.change_color_scheme("dark")