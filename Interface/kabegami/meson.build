pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), meson.project_name())
installdir = join_paths(pkgdatadir, 'kabegami')

python = import('python')

conf = configuration_data()
conf.set('PYTHON', python.find_installation('python3').full_path())
conf.set('APP_ID', APP_ID)
conf.set('APP_NAME', APP_NAME)
conf.set('APP_VERSION', APP_VERSION)
conf.set('APP_ICON', APP_ICON)
conf.set('localedir', join_paths(get_option('prefix'), get_option('localedir')))
conf.set('pkgdatadir', pkgdatadir)

BUILD_TYPE='prod'
if get_option('devel')
  BUILD_TYPE='dev'
endif

conf.set('BUILD_TYPE', BUILD_TYPE)

subdir('widgets')
subdir('views')
subdir('windows')
subdir('ui')

configure_file(
  input: 'kabegami.py',
  output: 'kabegami',
  configuration: conf,
  install: true,
  install_dir: get_option('bindir'),
  install_mode: ['rwxr-xr-x']
)

config_file = configure_file(
    input: 'config.py',
    output: 'config.py',
    configuration: conf
)

kabegami_sources = [
  '__init__.py',
  'main.py',
  config_file
]

install_data(kabegami_sources, install_dir: installdir)