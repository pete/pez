
#	Unix makefile for PEZ

VERSION = 1.2

COPTIONS = -g -Wall -fPIC -DPIC -DREENTRANT
CC = gcc

LIBRARIES = -lm -ldl

INCLUDE =

PEZCONFIG = -DMEMSTAT -DALIGNMENT -DEXPORT -DREADONLYSTRINGS # -DNOMEMCHECK

CFLAGS += $(COPTIONS) $(INCLUDE) $(PEZCONFIG)

PEZOBJ = pez.o pezmain.o

APPS = pez primdeftest

all:	$(APPS) libpez.so

pez: $(PEZOBJ)
	$(CC) $(CFLAGS) $(PEZOBJ) -o pez $(LIBRARIES)

libpez.so: $(PEZOBJ)
	$(CC) -shared $(CFLAGS) $(PEZOBJ) -o libpez.so

pez.o:   pez.c pezdef.h
pezdef.h:   pez.h
pezmain.o:  pezmain.c pez.h

primdeftest:
	$(CC) $(CFLAGS) primdeftest.c pez.c -o primdeftest $(LIBRARIES)

#	Run the Atlast regression test
regression:
	echo testcons >/tmp/regin.tmp
	echo Test 1234 >>/tmp/regin.tmp
	echo ls pez.c >>/tmp/regin.tmp
	echo  >>/tmp/regin.tmp
	echo  >>/tmp/regin.tmp
	./pez -iregress </tmp/regin.tmp >/tmp/regout.tmp
	diff regout.txt /tmp/regout.tmp

clean:
	rm -f $(APPS)
	rm -f *.bak *.o *.so *.dvi *.aux *.log
	rm -f core core.* cscope.out *.tar.gz
	rm -rf dist
	( cd tex ; make clean )
	
dist:
	rm -rf dist
	mkdir dist
	mkdir dist/pez-$(VERSION)
	cp -p pez.c pez.h tex/pez.pdf pezdef.h \
		pez.html \
		pezmain.c COPYING log.txt Makefile MANIFEST \
		primdeftest.c \
		regout.txt regress.pez dist/pez-$(VERSION)
	mkdir dist/pez-$(VERSION)/tex
	cp -p tex/*.tex tex/*.sty tex/Makefile dist/pez-$(VERSION)/tex
	find dist -type f -exec chmod 644 {} \;
	( cd dist ; tar cfvz ../pez-$(VERSION).tar.gz pez-$(VERSION) )
	rm -rf dist

lint:	lintpez

lintpez:
	lint pez.c pezmain.c $(LIBRARIES) $(PEZCONFIG)

repl: pez
	rlwrap ./pez
