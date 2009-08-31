# The following gets rid of some GNU complaints about filenames:
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
# SUBDIRS = lib

# TODO:  This sort of breaks unless I make a Makefile for doc/.  Ignoring for
# now...
# docdir = $(prefix)/share/doc/$(PACKAGE)
# TODO how to include examples?
# doc_DATA = doc/COPYING doc/CREDITS doc/ChangeLog doc/TODO doc/ctags examples

AM_CPPFLAGS="-DCONF_LIBDIR=\"${libdir}\"" "-DCONF_CFLAGS=\"${CFLAGS}\""

bin_PROGRAMS = pez$(EXEEXT)

pez_SOURCES = pez.c pezmain.c pez.h pezdef.h
pez_CFLAGS = $(AM_CFLAGS)
include_HEADERS = pez.h pezdef.h

EXTRA_DIST = doc/COPYING doc/CREDITS doc/TODO doc/ctags pez.h pezdef.h regress.pez

lib_LTLIBRARIES = libpez.la

libpez_la_SOURCES = pez.c
# I swear, the docs say this works:
# libpez_la_LIBTOOLFLAGS = -version-info 1:4:3

repl: pez$(EXEEXT)
	$(RLWRAP) ./pez$(EXEEXT)

regress: pez$(EXEEXT)
	./pez$(EXEEXT) regress.pez
