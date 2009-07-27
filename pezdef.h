/*

   Pez
   Definitions for components of PEZ
    This  file	contains the definitions for modules within the PEZ
    language system itself.  Definitions used by programs that link to
    PEZ   are   in	PEZ.H,  which  is  included	by  this  file
    automatically.

   See doc/CREDITS for information about the authors.  
   This program is in the public domain.
*/

#ifndef __PEZDEF_H
#define __PEZDEF_H

#include "pez.h"                   /* Define user linkage structures */

typedef void (*codeptr)();	      /* Machine code pointer */

/*  Dictionary word entry  */

typedef struct dw {
    struct dw *wnext;		      /* Next word in dictionary */
    char *wname;		      /* Word name.  The first character is
					 actually the word flags, including
					 the (IMMEDIATE) bit. */
    codeptr wcode;		      /* Machine code implementation */
} dictword;

/*  Word flag bits  */

#define IMMEDIATE   1		      /* Word is immediate */
#define WORDUSED    2		      /* Word used by program */
#define WORDHIDDEN  4		      /* Word is hidden from lookup */

/*  Data types	*/

typedef long stackitem;
typedef dictword **rstackitem;

/* Stack items occupied by a dictionary word definition */
#define Dictwordl ((sizeof(dictword)+(sizeof(stackitem)-1))/sizeof(stackitem))

/* Token types */

#define TokNull     0		      /* Nothing scanned */
#define TokWord     1		      /* Word stored in token name buffer */
#define TokInt	    2		      /* Integer scanned */
#define TokReal     3		      /* Real number scanned */
#define TokString   4		      /* String scanned */

/*  Primitive definition table entry  */

struct primfcn {
    char *pname;
    codeptr pcode;
};

/*  Internal state marker item	*/

typedef struct {
    stackitem *mstack;		      /* Stack position marker */
    stackitem *mheap;		      /* Heap allocation marker */
    dictword ***mrstack;	      /* Return stack position marker */
    dictword *mdict;		      /* Dictionary marker */
} pez_statemark;

#ifdef EXPORT
#define Exported
#ifndef NOMANGLE
#define stk	    pez__sp
#define stack	    pez__sk
#define stackbot    pez__sb
#define stacktop    pez__st
#define rstk	    pez__rp
#define rstack	    pez__rk
#define rstackbot   pez__rb
#define rstacktop   pez__rt
#define heap	    pez__hb
#define hptr	    pez__hp
#define heapbot     pez__hs
#define heaptop     pez__ht
#define dict	    pez__dh
#define dictprot    pez__dp
#define strbuf	    pez__ts
#define cstrbuf     pez__tn
#define ip	    pez__ip
#define curword     pez__cw
#define createword  pez__wd
#endif /* NOMANGLE */

#ifdef MEMSTAT
#ifndef NOMANGLE
#define stackmax    pez__sx
#define rstackmax   pez__rx
#define heapmax     pez__hx
#endif /* NOMANGLE */
extern stackitem *stackmax, *heapmax;
extern dictword ***rstackmax;
#endif

#ifdef ALIGNMENT
#ifndef NOMANGLE
#define rbuf0	    pez__r0
#define rbuf1	    pez__r1
#define rbuf2	    pez__r2
#endif /* NOMANGLE */
extern pez_real rbuf0, rbuf1, rbuf2;  /* Real temporaries for alignment */
#endif

#define FmodeR	    1		      /* Read mode */
#define FmodeW	    2		      /* Write mode */
#define FmodeB	    4		      /* Binary file mode */
#define FmodeCre    8		      /* Create new file */

extern stackitem *stack, *stk, *stackbot, *stacktop, *heap, *hptr,
		 *heapbot, *heaptop;
extern dictword ***rstack, ***rstk, ***rstackbot, ***rstacktop;
extern dictword *dict, *dictprot, *curword, *createword;
extern dictword **ip;
extern char **strbuf;
extern int cstrbuf;

#ifndef NOMANGLE
#define P_create    pez__Pcr
#define P_dodoes    pez__Pds
#endif /* NOMANGLE */
extern void P_create(), P_dodoes();
#else  /* EXPORT */
#define Exported static
#endif /* EXPORT */

#ifdef EXPORT
#ifndef NOMANGLE
#define stakover    pez__Eso
#define rstakover   pez__Erso
#define heapover    pez__Eho
#define badpointer  pez__Ebp
#define stakunder   pez__Esu
#define rstakunder  pez__Ersu
#endif /* NOMANGLE */
extern
#endif
void stakover(), rstakover(), heapover(), badpointer(),
     stakunder(), rstakunder();

/* Functions called by exported extensions. */
extern void pez_primdef(), pez_error();
extern dictword *pez_lookup(), *pez_vardef();
extern stackitem *pez_body();
extern int pez_exec();
#ifdef EXPORT
extern char *pez_fgetsp();
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
#define Keybreak() {static int n=0; if ((n=(n+1)&127)==0) {UbI(); broken=ads_usrbrk();}}
#endif

#ifndef Keybreak
#define Keybreak()
#endif

/*  Dynamic storage manipulation primitives  */

/*  Stack access definitions  */

#define S0  stk[-1]		      /* Top of stack */
#define S1  stk[-2]		      /* Next on stack */
#define S2  stk[-3]		      /* Third on stack */
#define S3  stk[-4]		      /* Fourth on stack */
#define S4  stk[-5]		      /* Fifth on stack */
#define S5  stk[-6]		      /* Sixth on stack */
#define Pop stk--		      /* Pop the top item off the stack */
#define Pop2 stk -= 2		      /* Pop two items off the stack */
#define Npop(n) stk -= (n)	      /* Pop N items off the stack */
// TODO:  See Realpush().
#define Push *stk++		      /* Push item onto stack */

#ifdef MEMSTAT
#define Mss(n) if ((stk+(n))>stackmax) stackmax = stk+(n);
#define Msr(n) if ((rstk+(n))>rstackmax) rstackmax = rstk+(n);
#define Msh(n) if ((hptr+(n))>heapmax) heapmax = hptr+(n);
#else
#define Mss(n)
#define Msr(n)
#define Msh(n)
#endif

#ifdef BOUNDS_CHECK
#define Memerrs
#define Sl(x) if ((stk-stack)<(x)) {stakunder(); return Memerrs;}
#define So(n) Mss(n) if ((stk+(n))>stacktop) {stakover(); return Memerrs;}
#else
#define Sl(x)
#define So(n)
#endif

/*  Return stack access definitions  */

#define R0  rstk[-1]		      /* Top of return stack */
#define R1  rstk[-2]		      /* Next on return stack */
#define R2  rstk[-3]		      /* Third on return stack */
#define Rpop rstk--		      /* Pop return stack */
#define Rpush *rstk++		      /* Push return stack */
#ifdef BOUNDS_CHECK
#define Rsl(x) if ((rstk-rstack)<(x)) {rstakunder(); return Memerrs;}
#define Rso(n) Msr(n) if ((rstk+(n))>rstacktop){rstakover(); return Memerrs;}
#else
#define Rsl(x)
#define Rso(n)
#endif

/*  Heap access definitions  */

#ifdef BOUNDS_CHECK
#define Ho(n)  Msh(n) if ((hptr+(n))>heaptop){heapover(); return Memerrs;}
#define Hpc(n) if ((((stackitem *)(n))<heapbot)||(((stackitem *)(n))>=heaptop)){badpointer(); return Memerrs;}
#else
#define Ho(n)
#define Hpc(n)
#endif

#define Hstore *hptr++		      /* Store item on heap */
#define state  (*heap)		      /* Execution state is first heap word */

#define prim static void	      /* Attributes of primitive functions */

/*  Real number definitions (used only if REAL is configured).	*/

#define Realsize (sizeof(pez_real)/sizeof(stackitem)) /* Stack cells / real */
#define Realpop  stk -= Realsize      /* Pop real from stack */
#define Realpop2 stk -= (2 * Realsize) /* Pop two reals from stack */

#define REALSTACK ((pez_real *)stk)
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
#define Realpush(x) do { So(Realsize); ((pez_real *)stk++)[0] = (x); } while(0)

#ifdef TRACE
#define trace if(pez_trace)
#else // TRACE
#define trace if(0)
#endif


/*  File I/O definitions (used only if FILEIO is configured).  */

#define FileSent    0x831FDF9DL       /* Courtesy Marinchip Radioactive
					 random number generator */
#define Isfile(x) Hpc(x); if (*((stackitem *)(x))!=FileSent) {printf("\nNot a file\n");return;}
#define FileD(x)  ((FILE *)*(((stackitem *)(x)) + 1))
#define Isopen(x) if (FileD(x) == NULL) {printf("\nFile not open\n");return;}

#endif
