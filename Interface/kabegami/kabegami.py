#!@PYTHON@
import sys, signal, gettext

APP_VERSION = "@APP_VERSION@"
pkgdatadir = "@pkgdatadir@"
localedir = "@localedir@"

signal.signal(signal.SIGINT, signal.SIG_DFL)
gettext.install('kabegami', localedir)

sys.path.insert(1, pkgdatadir)

if __name__ == "__main__":

    gresource = f"{pkgdatadir}/kabegami.gresource"

    from gi.repository import Gio
    resource = Gio.Resource.load(gresource)
    resource._register()

    from kabegami.main import main
    sys.exit(main(APP_VERSION, sys.argv))