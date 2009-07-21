# The following gets rid of some GNU complaints about filenames:
AUTOMAKE_OPTIONS = foreign
# SUBDIRS = lib

# TODO:  This sort of breaks unless I make a Makefile for doc/.  Ignoring for
# now...
# docdir = $(prefix)/share/doc/$(PACKAGE)
# TODO how to include examples?
# doc_DATA = doc/COPYING doc/CREDITS doc/ChangeLog doc/TODO doc/ctags examples

bin_PROGRAMS = pez

pez_SOURCES = pez.c pezmain.c
pez_CFLAGS = $(AM_CFLAGS)

# TODO how to add get versioning? (e.g., libpez.so.1.3.0)
# More pressing TODO :  how to get this to build?
# libpez_so_SOURCES = pez.c pez.h pezdef.h

EXTRA_DIST = doc/COPYING doc/CREDITS doc/TODO doc/ctags

lib_LTLIBRARIES = libpez.la

libpez_la_SOURCES = pez.c