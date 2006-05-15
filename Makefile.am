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
                 kasumi.png

pkgdata_DATA = kasumi.png

AM_CPPFLAGS=-DLOCALEDIR='"$(localedir)"'
SUBDIRS = intl m4 po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
