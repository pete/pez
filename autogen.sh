#!/bin/sh

run ()
{
    echo "running: $*"
    eval $*

    if test $? != 0 ; then
	echo "error: while running '$*'"
	exit 1
    fi
}

libtoolize=$(which libtoolize)
if [ -z "$libtoolize" ]; then
	libtoolize=$(which glibtoolize)
fi

if [ ! -z "$libtoolize" ]; then
	run libtoolize -f -c
else
	echo "Can't re-libtoolize.  If the build breaks, try installing libtoolize."
fi

run aclocal -I m4
run automake --add-missing -c
run autoconf
