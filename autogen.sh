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

run libtoolize -f -c
run aclocal -I m4
run automake --add-missing -c
run autoconf
