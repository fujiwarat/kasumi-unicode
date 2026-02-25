project('kasumi',
  ['c', 'cpp'],
  version: '2.6',
  default_options: [ 'cpp_std=c++14' ],
  meson_version: '>= 1.5.0',
  license: 'GPL-2.0-or-later'
)

# Modules
#-------------------------------------------------
i18n = import('i18n')

# Versioning
#-------------------------------------------------
_version_split = meson.project_version().split('.')
kasumi_major_version = _version_split[0].to_int()
kasumi_minor_version = _version_split[1].to_int()
kasumi_prev_minor_version= kasumi_minor_version - 1

# Dependencies
#-------------------------------------------------
gtk_dep = dependency('gtk+-3.0', required: false)
if not gtk_dep.found()
  gtk_dep = dependency('gtk+-2.0')
endif

anthy_dep = dependency('anthy-unicode', required: false)
if not anthy_dep.found()
  anthy_dep = dependency('anthy')
endif

kasumi_desktop = i18n.merge_file(
  input: 'kasumi.desktop.in',
  output: '@BASENAME@',
  type: 'desktop',
  po_dir: meson.current_source_dir() / 'po',
  install: true,
  install_dir: get_option('datadir') / 'applications'
)

install_data(
  'kasumi.1',
  install_dir: get_option('datadir') / 'man' / 'man1',
)

install_data(
  'kasumi.png',
  install_dir: get_option('datadir') / 'pixmaps',
)

cdata = configuration_data()
cdata.set('PACKAGE', '"@0@"'.format(meson.project_name()))
cdata.set('VERSION', '"@0@"'.format(meson.project_version()))
cdata.set_quoted('LOCALEDIR', get_option('prefix') / get_option('localedir'))
cdata.set('ICONV_CONST', '')
cdata.set('HAVE_CATGETS', 1)
cdata.set('HAVE_GETTEXT', 1)
cdata.set('HAVE_LC_MESSAGES', 1)
cdata.set('ENABLE_NLS', 1)

configure_file(
  input: 'config.h.meson',
  output: '@BASENAME@',
  configuration: cdata
)

add_project_arguments('-DHAVE_CONFIG_H', language: [ 'c', 'cpp' ])

kasumi_bin_sources = files(
  'KasumiAddWindow.cxx',
  'KasumiConfiguration.cxx',
  'KasumiDic.cxx',
  'KasumiException.cxx',
  'KasumiMainWindow.cxx',
  'KasumiString.cxx',
  'KasumiWord.cxx',
  'KasumiWordType.cxx',
  'cellrendererspin.c',
  'main.cxx',
)

kasumi_bin = executable('kasumi',
  kasumi_bin_sources,
  dependencies: [ anthy_dep, gtk_dep ],
  install: true,
)

subdir('po')
