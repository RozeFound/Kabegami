pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), meson.project_name())
windowsdir = join_paths(pkgdatadir, 'kabegami/windows')

kabegami_sources = [
  '__init__.py',
  'main_window.py',
]

install_data(kabegami_sources, install_dir: windowsdir)