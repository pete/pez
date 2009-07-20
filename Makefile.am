# The following gets rid of some GNU complaints about filenames:
AUTOMAKE_OPTIONS = foreign
# SUBDIRS = lib

# TODO:  This sort of breaks unless I make a Makefile for doc/.  Ignoring for
# now...
# docdir = $(prefix)/share/doc/$(PACKAGE)
# TODO how to include examples?
# doc_DATA = doc/COPYING doc/CREDITS doc/ChangeLog doc/TODO doc/ctags examples

bin_PROGRAMS = pez

pez_SOURCES = pez.c pezmain.c pez.h pezdef.h
libpez_so_SOURCES = pez.c pez.h pezdef.h

# TODO how to add get versioning? (e.g., libpez.so.1.3.0)

