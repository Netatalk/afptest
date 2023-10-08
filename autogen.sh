#!/bin/sh

LIBTOOLIZE=libtoolize
SYSNAME=`uname`
if [ "x$SYSNAME" = "xDarwin" ] ; then
  LIBTOOLIZE=glibtoolize
fi

# build it all.
$LIBTOOLIZE --copy --force && \
	aclocal -I macros $ACLOCAL_FLAGS && \
	autoheader && \
	automake --include-deps --add-missing --foreign && \
	autoconf

# just in case automake generated errors...
autoconf

LDFLAGS=-rdynamic ./configure --enable-maintainer-mode -C "$@"
