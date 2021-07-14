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
		kasumi.1 \
		kasumi.desktop.in

appicondir = $(datadir)/pixmaps
appicon_DATA = kasumi.png

kasumi_LDADD = $(INTLLIBS) $(LTLIBICONV)

man_MANS = kasumi.1
EXTRA_DIST = $(man_MANS)

desktopdir = $(datadir)/applications
Desktop_in_files = kasumi.desktop.in
desktop_DATA = $(Desktop_in_files:.desktop.in=.desktop)

localedir = $(datadir)/locale
AM_CPPFLAGS=-DLOCALEDIR=\"$(localedir)\"
SUBDIRS = intl m4 po

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = \
CFLAGS="$(CFLAGS)" \
CPPFLAGS="$(CPPFLAGS)" \
$(NULL)

CLEANFILES = $(desktop_DATA)
#EXTRA_DIST = config.rpath

dist-hook:
	if test -d .git ; then \
	    git log --name-status --date=iso > $(distdir)/ChangeLog ; \
	    KASUMI_PREV_MINOR_VERSION=`expr $(KASUMI_MINOR_VERSION) - 1`; \
	    KASUMI_PREV_VERSION=\
"$(KASUMI_MAJOR_VERSION).$$KASUMI_PREV_MINOR_VERSION"; \
	    echo "Changes in kasumi-unicode $(KASUMI_VERSION)" \
	        > $(distdir)/NEWS; \
	    echo "" >> $(distdir)/NEWS; \
	    git shortlog $$KASUMI_PREV_VERSION...$(KASUMI_VERSION) \
	        >> $(distdir)/NEWS; \
	    echo "" >> $(distdir)/NEWS; \
	    git log $$KASUMI_PREV_VERSION...$(KASUMI_VERSION) --reverse \
	        --pretty=format:'%s (%an) %h' >> $(distdir)/NEWS; \
	fi

