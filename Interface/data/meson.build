conf = configuration_data()

conf.set('APP_NAME', APP_NAME)
conf.set('APP_ID', APP_ID)

APP_ICON = ('@0@.svg').format(APP_ID)
APP_SYMBOLIC_ICON = ('@0@-symbolic.svg').format(APP_ID)

desktop_file = configure_file(
          input: 'com.github.RozeFound.Kabegami.desktop.in.in',
         output: 'com.github.RozeFound.Kabegami.desktop.in',
  configuration: conf
)

desktop = i18n.merge_file(
        input: desktop_file,
       output: 'com.github.RozeFound.Kabegami.desktop',
         type: 'desktop',
       po_dir: '../po',
      install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils, args: [desktop_file])
endif

appstream_file = i18n.merge_file(
        input: 'com.github.RozeFound.Kabegami.appdata.xml.in',
       output: 'com.github.RozeFound.Kabegami.appdata.xml',
       po_dir: '../po',
      install: true,
  install_dir: join_paths(get_option('datadir'), 'appdata')
)

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util, args: ['validate', appstream_file])
endif

install_data('com.github.RozeFound.Kabegami.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0/schemas')
)

compile_schemas = find_program('glib-compile-schemas', required: false)
if compile_schemas.found()
  test('Validate schema file',
       compile_schemas,
       args: ['--strict', '--dry-run', meson.current_source_dir()])
endif

scalable_dir = join_paths('icons', 'hicolor', 'scalable', 'apps')
install_data ( join_paths(scalable_dir, APP_ICON),
  install_dir: join_paths(get_option('prefix'), get_option('datadir'), scalable_dir)
)

symbolic_dir = join_paths('icons', 'hicolor', 'symbolic', 'apps')
install_data ( join_paths(symbolic_dir, APP_SYMBOLIC_ICON),
  install_dir: join_paths(get_option('prefix'), get_option('datadir'), symbolic_dir)
)