dnl Process this file with autoconf to produce a configure script.
m4_define([kasumi_released], [1])
m4_define([kasumi_major_version], [2])
m4_define([kasumi_minor_version], [6])
m4_define(kasumi_maybe_datestamp,
    m4_esyscmd([if test x]kasumi_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([kasumi_version],
    kasumi_major_version.kasumi_minor_version[]kasumi_maybe_datestamp)

AC_INIT([kasumi-unicode], [kasumi_version],
        [https://github.com/fujiwarat/kasumi-unicode/issues],
        [kasumi-unicode])
AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR(m4)

dnl Define sustituted variables:
KASUMI_MAJOR_VERSION=kasumi_major_version
KASUMI_MINOR_VERSION=kasumi_minor_version
KASUMI_VERSION=kasumi_version
AC_SUBST(KASUMI_MAJOR_VERSION)
AC_SUBST(KASUMI_MINOR_VERSION)
AC_SUBST(KASUMI_VERSION)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX

dnl Checks for gettext
ALL_LINGUAS="ja it"
AM_GNU_GETTEXT

dnl LDFLAGS="$LDFLAGS $INTLLIBS"
dnl localedir='${prefix}/share/locale'
dnl AC_SUBST(localedir)
dnl AC_DEFINE(HAVE_CATGETS, 1,Define as 1 if you have catgets and don't want to use GNU gettext.)
dnl AC_DEFINE(HAVE_GETTEXT, 1, Define as 1 if you have GNU gettext.)
dnl AC_DEFINE(HAVE_LC_MESSAGES, 1, Define as 1 if you have LC_MESSAGES.)
dnl AC_DEFINE(HAVE_STPCPY, 1, Define as 1 if you have stpcpy() function.)
dnl AC_DEFINE(ENABLE_NLS, 1, Define as 1 if you want to use NLS.)

AM_ICONV

if test "x$am_cv_func_iconv" != "xyes"; then
	AC_MSG_ERROR([Unable to find iconv(). iconv() is needed for Kasumi])
fi

dnl Checks for libraries.
PKG_CHECK_MODULES([GTK], [gtk+-3.0], [GTK="gtk3"],
                  [PKG_CHECK_MODULES([GTK], [gtk+-2.0], [GTK="gtk2"])])
CFLAGS="$CFLAGS $GTK_CFLAGS"
CPPFLAGS="$CPPFLAGS $GTK_CFLAGS"
LIBS="$LIBS $GTK_LIBS"
AC_CHECK_LIB(iconv, iconv)

PKG_CHECK_MODULES([ANTHY], [anthy-unicode], [ANTHY="anthy-unicode"],
                  [PKG_CHECK_MODULES([ANTHY], [anthy], [ANTHY="anthy"])])
if test "x$ANTHY" = "xanthy-unicode"; then
    AC_CHECK_LIB(anthydic-unicode, anthy_dic_util_init)
    AC_CHECK_LIB(anthy-unicode, anthy_get_version_string)
elif test "x$ANTHY" = "xanthy"; then
    AC_CHECK_LIB(anthydic, anthy_dic_util_init)
    AC_CHECK_LIB(anthy, anthy_get_version_string)
else
    as_fn_error $? "anthy-unicode or anthy is not installed" "$LINENO" 5; 
fi
CFLAGS="$CFLAGS $ANTHY_CFLAGS"
CPPFLAGS="$CPPFLAGS $ANTHY_CFLAGS"
LIBS="$LIBS $ANTHY_LIBS"

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CONFIG_FILES([
kasumi.desktop
])

AC_OUTPUT(Makefile intl/Makefile po/Makefile.in m4/Makefile )

AC_OUTPUT
echo "
                kasumi-unicode version kasumi_version

ANTHY                       $ANTHY
GTK                         $GTK
"
