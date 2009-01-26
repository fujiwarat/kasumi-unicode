#!/bin/sh
autopoint
aclocal -I m4
autoheader
automake --add-missing --copy --include-deps
autoconf
