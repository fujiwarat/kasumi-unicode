bin_PROGRAMS = kasumi

kasumi_SOURCES = main.cxx \
                 KasumiDic.cxx KasumiDic.hxx \
                 KasumiWord.cxx KasumiWord.hxx \
				 KasumiString.cxx KasumiString.hxx \
				 KasumiException.hxx \
				 KasumiMainWindow.cxx KasumiMainWindow.hxx \
				 KasumiDicEventListener.hxx

AM_CPPFLAGS=-DLOCALEDIR='"$(localedir)"'
SUBDIRS = intl m4 po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
