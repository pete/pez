#!/bin/sh

make distclean >/dev/null 2>/dev/null
automake
autoconf
./configure $@
make
