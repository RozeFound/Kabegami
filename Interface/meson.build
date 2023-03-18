project('kabegami',
          version: '0.1.0',
    meson_version: '>= 0.59.0',
  default_options: [ 'warning_level=2' ],
)

APP_NAME = 'Kabegami'
APP_ID = 'com.github.RozeFound.Kabegami'
APP_VERSION = meson.project_version()

devel = get_option('devel')

if devel == true
	APP_VERSION += '-devel'
	APP_NAME += ' (Development)'
	APP_ID += '.' + 'Devel'
endif

i18n = import('i18n')
gnome = import('gnome')
localedir = get_option('localedir')

subdir('data')
subdir('kabegami')
subdir('po')

gnome.post_install(
     glib_compile_schemas: true,
    gtk_update_icon_cache: true,
  update_desktop_database: true,
)