import gi

gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw

@Gtk.Template(resource_path="/com/github/RozeFound/Kabegami/placeholder.ui")
class NotImplemented(Adw.Bin):

    __gtype_name__ = "NotImplementedView"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)


