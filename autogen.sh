#!/bin/sh
test -n "$srcdir" || srcdir=$(dirname "$0")
test -n "$srcdir" || srcdir=.

cd "$srcdir"

CFLAGS=${CFLAGS-"-std=c++14 -Wall -Wformat -Werror=format-security"}
CPPFLAGS=${CPPFLAGS-"-std=c++14 -Wall -Wformat -Werror=format-security"}

autoreconf --verbose --force --install || exit 1
CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" $srcdir/configure "$@" || exit 1
