pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), meson.project_name())

gnome = import('gnome')

blueprints = custom_target('blueprints',
  input: files(
    'about.blp',
    'window.blp',
    'placeholder.blp',
    'theme_switcher.blp'
    ),
  output: '.',
  command: [find_program('blueprint-compiler'), 'batch-compile', '@OUTPUT@', '@CURRENT_SOURCE_DIR@', '@INPUT@'])

gnome.compile_resources('kabegami',
  'kabegami.gresource.xml',
  gresource_bundle: true,
  dependencies: blueprints,
  install: true,
  install_dir: pkgdatadir,
)