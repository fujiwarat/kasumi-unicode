bin_PROGRAMS = kasumi

kasumi_SOURCES = main.cxx intl.h \
		KasumiDic.cxx KasumiDic.hxx \
		KasumiWord.cxx KasumiWord.hxx \
		KasumiWordType.cxx KasumiWordType.hxx \
		KasumiString.cxx KasumiString.hxx \
		KasumiException.cxx KasumiException.hxx \
		KasumiMainWindow.cxx KasumiMainWindow.hxx \
		KasumiAddWindow.cxx KasumiAddWindow.hxx \
		KasumiDicEventListener.hxx \
		KasumiWordEventListener.hxx \
		KasumiConfiguration.cxx KasumiConfiguration.hxx \
		cellrendererspin.c cellrendererspin.h \
		kasumi.png \
		kasumi.1

appicondir = $(datadir)/pixmaps
appicon_DATA = kasumi.png

man_MANS = kasumi.1
EXTRA_DIST = $(man_MANS)

AM_CPPFLAGS=-DLOCALEDIR='"$(localedir)"'
SUBDIRS = intl m4 po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
