# The following gets rid of some GNU complaints about filenames:
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

pez_version_major=1
pez_version_minor=6
pez_version_teeny=0
pez_lib_version=$(pez_version_major).$(pez_version_minor)
pez_version=$(pez_version_major).$(pez_version_minor).$(pez_version_teeny)

bin_PROGRAMS = pez$(EXEEXT)

pez_SOURCES = pez.c pez.h pezmain.c pezdef.h st.c st.h
pezdir = $(libdir)/pez/$(pez_lib_version)
pez_DATA = lib/*.pez

AM_CPPFLAGS="-DPEZCONF_LIBDIR=\"${libdir}\"" \
	    "-DPEZCONF_PEZ_LIBDIR=\"${pezdir}\"" \
	    "-DPEZCONF_BINDIR=\"${bindir}\"" \
	    "-DPEZCONF_CFLAGS=\"${CFLAGS}\"" \
	    "-DPEZCONF_LDFLAGS=\"${LDFLAGS}\"" \
	    "-DPEZCONF_SO_FLAGS=\"-shared ${pic_flag}\"" \
	    "-DPEZCONF_LD=\"${LD}\"" \
	    "-DPEZCONF_CC=\"${CC}\""

pez_CFLAGS = $(AM_CFLAGS)
include_HEADERS = pez.h pezdef.h

EXTRA_DIST = README doc/* lightning/* m4 \
	     pez.h pezdef.h regress.pez $(pez_DATA)

lib_LTLIBRARIES = libpez.la

libpez_la_SOURCES = pez.c pez.h st.c st.h
# I swear, the docs say this works:
# libpez_la_LIBTOOLFLAGS = -version-info 1:5:2

repl: pez$(EXEEXT)
	$(RLWRAP) ./pez$(EXEEXT)

regress: pez$(EXEEXT)
	./pez$(EXEEXT) regress.pez
