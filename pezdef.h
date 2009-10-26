/*
   Definitions for components of PEZ
   This file contains the definitions for modules within the PEZ
   language system itself. Definitions used by programs that link to
   PEZ are in PEZ.H, which is included by this file
   automatically.

   See doc/CREDITS for information about the authors.
   This program is in the public domain.
*/

#ifndef __PEZDEF_H
#define __PEZDEF_H

#include "pez.h"                   /* Define user linkage structures */

/*  Word flag bits  */

#define IMMEDIATE   1		      /* Word is immediate */
#define WORDUSED    2		      /* Word used by program */
#define WORDHIDDEN  4		      /* Word is hidden from lookup */

/*  Data types	*/

/* Stack items occupied by a dictionary word definition */
#define Dictwordl ((sizeof(pez_dictword) + (sizeof(pez_stackitem)-1)) \
		/ sizeof(pez_stackitem))

/* Token types */

#define TokNull     0		      /* Nothing scanned */
#define TokWord     1		      /* Word stored in token name buffer */
#define TokInt	    2		      /* Integer scanned */
#define TokReal     3		      /* Real number scanned */
#define TokString   4		      /* String scanned */

/*  Primitive definition table entry  */

struct primfcn {
    char *pname;
    pez_wordp pcode;
};

/*  Internal state marker item	*/

#ifdef EXPORT
#define Exported

#ifdef ALIGNMENT
extern pez_real rbuf0, rbuf1, rbuf2;  /* Real temporaries for alignment */
#endif

#define FmodeR	    1		      /* Read mode */
#define FmodeW	    2		      /* Write mode */
#define FmodeB	    4		      /* Binary file mode */
#define FmodeCre    8		      /* Create new file */

extern void P_create(pez_instance *p), P_dodoes(pez_instance *p);
#else  /* EXPORT */
#define Exported static
#endif /* EXPORT */

#ifdef EXPORT
extern
#endif
void stakover(pez_instance *p), rstakover(pez_instance *p),
     heapover(pez_instance *p), badpointer(pez_instance *p),
     stakunder(pez_instance *p), rstakunder(pez_instance *p);

/* Functions called by exported extensions. */
extern void pez_primdef(pez_instance *p, struct primfcn *pt),
	pez_error(pez_instance *p, char *kind);

extern pez_dictword *pez_lookup(pez_instance *p, char *name),
       *pez_vardef(pez_instance *p, char *name, int size);
extern pez_stackitem *pez_body(pez_instance *p, pez_dictword *dw);
extern int pez_exec(pez_instance *p, pez_dictword *dw);
#ifdef EXPORT
extern char *pez_fgetsp(pez_instance *p, char *s, int n, FILE *stream);
#endif

/*  If explicit alignment is not requested, enable it in any case for
    known CPU types that require alignment.  */

#ifndef ALIGNMENT
#ifdef sparc
#define ALIGNMENT
#endif
#endif
#ifdef __TURBOC__
#define  Keyhit()   (kbhit() ? getch() : 0)
/* DOS needs poll to detect break */
#define  Keybreak() { static int n=0; if ((n = (n+1) & 127) == 0) kbhit(); }
#ifdef __MSDOS__
#define MSDOS
#endif
#endif
#ifdef MSDOS
#define FBmode			      /* DOS requires binary file flag */
#endif
#ifdef Macintosh
#define FBmode			      /* Macintosh requires binary file flag */
#endif
#ifdef OS2			      /* OS/2 requires binary file flag */
#define FBmode
#endif

#ifdef HIGHC
#define unlink(x) remove(x)	      /* High C library is brain-dead */
pragma On(PCC_msgs);		      /* High C compiler is brain-dead */
#endif


/*  STATIC is defined as "static" on platforms which require this
    specification on declarations for forward-referenced functions.  */

#define STATIC static

#ifdef ADS                            /* Definitions if we're an ADS app */
#define printf ads_printf	      /* Print through ADS */
#define Keybreak() do {\
	static int n=0;\
	if ((n=(n+1)&127)==0) {UbI(); broken=ads_usrbrk();}\
} while(0)
#endif

#ifndef Keybreak
#define Keybreak()
#endif

/*  Dynamic storage manipulation primitives  */

/*  Stack access definitions  */

#define S0  p->stk[-1]		      /* Top of stack */
#define S1  p->stk[-2]		      /* Next on stack */
#define S2  p->stk[-3]		      /* Third on stack */
#define S3  p->stk[-4]		      /* Fourth on stack */
#define S4  p->stk[-5]		      /* Fifth on stack */
#define S5  p->stk[-6]		      /* Sixth on stack */
#define Pop p->stk--		      /* Pop the top item off the stack */
#define Pop2 p->stk -= 2		      /* Pop two items off the stack */
#define Npop(n) p->stk -= (n)	      /* Pop N items off the stack */
// TODO:  See Realpush().
#define Push *p->stk++		      /* Push item onto stack */

#ifdef MEMSTAT
#define Mss(n) if ((p->stk+(n))>p->stack) p->stackmax = p->stk+(n);
#define Msr(n) if ((p->rstk+(n))>p->rstack) p->rstackmax = p->rstk+(n);
#define Msh(n) if ((p->hptr+(n))>p->heap) p->heapmax = p->hptr+(n);
#else
#define Mss(n)
#define Msr(n)
#define Msh(n)
#endif

#ifdef BOUNDS_CHECK
#define Memerrs
#define Sl(x) if ((p->stk - p->stack)<(x)) {stakunder(p); return Memerrs;}
#define So(n) do {\
	Mss(n); if((p->stk+(n)) > p->stacktop){stakover(p); return Memerrs;} \
} while(0)
#else
#define Sl(x)
#define So(n)
#endif

/*  Return stack access definitions  */

#define R0  p->rstk[-1]		      /* Top of return stack */
#define R1  p->rstk[-2]		      /* Next on return stack */
#define R2  p->rstk[-3]		      /* Third on return stack */
#define Rpop p->rstk--		      /* Pop return stack */
#define Rpush *p->rstk++		      /* Push return stack */
#ifdef BOUNDS_CHECK
#define Rsl(x) if((p->rstk - p->rstack)<(x)) {rstakunder(p); return Memerrs;}
#define Rso(n) Msr(n) \
	if((p->rstk + (n)) > p->rstacktop){rstakover(p); return Memerrs;}
#else
#define Rsl(x)
#define Rso(n)
#endif

/*  Heap access definitions  */

#ifdef BOUNDS_CHECK
#define Ho(n) do { \
	Msh(n); if((p->hptr+(n)) > p->heaptop){heapover(p); return Memerrs;} \
} while(0);
#else
#define Ho(n)
#endif

#ifdef RESTRICTED_POINTERS
#define Hpc(n) do {\
	if((((pez_stackitem *)(n)) < p->heapbot)||\
			(((pez_stackitem *)(n)) >= p->heaptop)){\
		badpointer(p);\
		return Memerrs;\
	}\
} while(0)
#else
#define Hpc(n)
#endif

#define Hstore *p->hptr++		// Store item on heap
// Item on heap with overflow check
#define Hsingle(val) do { Ho(1); *p->hptr++ = val; } while(0)
#define state  (*p->heap)		// Execution state is first heap word

#define prim inline static void		// Attributes of primitive functions

/*  Floating point macros: */
// How many cells a float takes up:
#define Realsize (sizeof(pez_real)/sizeof(pez_stackitem))
// Pop a float:
#define Realpop  p->stk -= Realsize
// Pop two floats:
#define Realpop2 p->stk -= (2 * Realsize)

#define REALSTACK ((pez_real *)p->stk)
#ifdef ALIGNMENT
#define REAL0 *((pez_real *)memcpy((char *)&rbuf0, (char *)(REALSTACK - 1), \
			sizeof(pez_real)))
#define REAL1 *((pez_real *)memcpy((char *)&rbuf1, (char *)(REALSTACK - 2), \
			sizeof(pez_real)))
#define REAL2 *((pez_real *)memcpy((char *)&rbuf2, (char *)(REALSTACK - 3), \
			sizeof(pez_real)))
#define SREAL0(x) rbuf2=(x); (void)memcpy((char *)(REALSTACK - 1), \
		(char *)&rbuf2, sizeof(pez_real))
#define SREAL1(x) rbuf2=(x); (void)memcpy((char *)(REALSTACK - 2), \
		(char *)&rbuf2, sizeof(pez_real))
#else
#define REAL0 REALSTACK[-1]
#define REAL1 REALSTACK[-2]
#define REAL2 REALSTACK[-3]
#define SREAL0(x) REAL0 = (x)
#define SREAL1(x) REAL1 = (x)
#endif
#define Realpush(x) do { \
	So(Realsize); ((pez_real *)p->stk++)[0] = (x); } while(0)

#ifdef TRACE
#define tracing if(p->trace)
#else // TRACE
#define tracing if(0)
#endif

// Miscellaneous conveniences
#define Immediate(word) (di->wname[0] & IMMEDIATE)

#endif
