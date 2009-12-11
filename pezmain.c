/*
   pezmain.c provides a read-eval-print loop executable for Pez.

   See doc/CREDITS for information about the authors.
   This program is in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <gc/gc.h>

#include <signal.h>
#include "pezdef.h"

#define FALSE	0
#define TRUE	1

pez_instance *p;

/*  Globals imported  */

#ifndef HIGHC

/*  CTRLC  --  Catch a user console break signal.  If your C library
	       does not provide this Unix-compatibile facility
	       (registered with the call on signal() in main()),
	       just turn this code off or, better still, replace it
	       with the equivalent on your system.  */

static void ctrlc(int sig)
{
	if(sig == SIGINT)
		pez_break(p);
}
#endif				/* HIGHC */

int print_usage(FILE * s, char *pname)
{
	return fprintf(s,
		       "Usage:  %s [options] [input file] [pez args]\n"
		       "        Options:\n"
		       "           -D     Treat file as definitions\n"
		       "           -Hn    Heap length n\n"
		       "           -Ifile Include named definition file\n"
		       "           -Rn    Return stack length n\n"
		       "           -Sn    Stack length n\n"
		       "           -T     Set TRACE mode\n"
		       "           -U     Print this message\n", pname);
}

static void init_pez_argv(int argc)
{
	int size = sizeof(char *) *argc;
	p->argv = GC_MALLOC(size);
	if(!p->argv) {
		fprintf(stderr, "Couldn't allocate enough memory to duplicate "
			"argv (%d bytes).\n"
			"Something's real bad wrong.\n", size);
		exit(2);
	}
	memset(p->argv, 0, size);
}

void pezmain_load(char *fname, FILE *fp)
{
	int stat;
	if(fp == NULL) {
		fprintf(stderr, "Unable to open include file %s\n", fname);
		exit(1);
	}
	stat = pez_load(p, fp);
	fclose(fp);
	if(stat != PEZ_SNORM) {
		printf("\nError %d in include file %s\n", stat, fname);
	}
}

void include_file(char *fname)
{
	int stat;
	FILE *fp;

	fp = fopen(fname,
#ifdef FBmode
		   "rb"
#else
		   "r"
#endif
	    );
	pezmain_load(fname, fp);
}

void load_rc()
{
	char path[256];
	FILE *fd;
	sprintf (path, "%s/.pezrc", getenv ("HOME"));
	
	fd = fopen(path,
#ifdef FBmode
		   "rb"
#else
		   "r"
#endif
	    );
	if(fd != NULL) {
		pezmain_load(path, fd);
	}
	
}


/*  MAIN  --  Main program.  */

int main(int argc, char *argv[])
{
	int i;
	int fname = FALSE, defmode = FALSE;
	int optdone = 0;
	FILE *ifp;
	char *include[20];
	char *cp, opt;
	int in = 0, status = PEZ_SNORM, interactive;
	char **pez_argv_current;

	p = pez_init();
	if(!p) {
		fprintf(stderr, "Couldn't initialize Pez!\n");
		exit(1);
	}

	init_pez_argv(argc);
	pez_argv_current = p->argv;

	ifp = stdin;
	load_rc();
	
	for(i = 1; i < argc; i++) {
		cp = argv[i];
		if(*cp == '-') {
			opt = *(++cp);
			if(islower(opt))
				opt = toupper(opt);
			// TODO:  No more case-insensitive command-line opts,
			// for one, and try to make these a little closer to
			// what users would expect, like -h printing help rather
			// than -u or -?.
			// Aside from that, I suppose we should have a real
			// parser here.
			switch (opt) {

			case 'D':
				defmode = TRUE;
				break;

			case 'I':
				include[in++] = cp + 1;
				break;

			case 'T':
				p->trace = TRUE;
				break;

			case '-':
				optdone = 1;
				break;

			case '?':
			case 'U':
				print_usage(stdout, argv[0]);
				return 0;
			default:
				optdone = 1;
				break;
			}
		} else {
			break;
		}
	}

	// Keeping i from before:
	while(i < argc) {
		cp = argv[i];
		char fn[132];

		if(fname) {
			(*(pez_argv_current++)) = cp;
		} else {
			fname = TRUE;
			strncpy(fn, cp, sizeof(fn));
			ifp = fopen(fn, "r");
			if(ifp == NULL) {
				fprintf(stderr, "Unable to open file %s\n", fn);
				return 1;
			}
		}
		i++;
	}

	/* If any include files were named, load each in turn before
	   we execute the program. */

	for(i = 0; i < in; i++) {
		int stat;
		char fn[132];
		FILE *fp;

		strncpy(fn, include[i], sizeof(fn));
		if(strchr(fn, '.') == NULL)
			strcat(fn, ".pez");
		include_file(fn);
	}

	/* Now that all the preliminaries are out of the way, fall into
	   the main PEZ execution loop. */

	interactive = !fname && isatty(0);

#ifndef HIGHC
	signal(SIGINT, ctrlc);
#endif				/* HIGHC */
	while(status == PEZ_SNORM || interactive) {
		char t[TOK_BUF_SZ];	

		if(interactive) {
			pez_eval(p, "10 n.s");
			if(p->comment)
				printf("(  ");
			else if(p->heap != NULL && state)
				printf(":> ");
			else
				printf("-> ");
			fflush(stdout);
		}
		if(fgets(t, TOK_BUF_SZ, ifp) == NULL) {
			if(fname && defmode) {
				fname = defmode = FALSE;
				interactive = TRUE;
				ifp = stdin;
				continue;
			}
			break;
		}
		status = pez_eval(p, t);
	}
	if(interactive)
		printf("\n");
	return 0;
}
