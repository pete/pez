/*
   Pez
   Main Interpreter and Compiler

   See doc/CREDITS for information about the authors.  
   This program is in the public domain.
*/

#include <config.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#ifdef ALIGNMENT
#ifdef __TURBOC__
#include <mem.h>
#else
#include <memory.h>
#endif
#endif

#ifdef Macintosh
/* Macintoshes need 32K segments, else barfage ensues */
#pragma segment seg2a
#endif				/*Macintosh */

/*  Subpackage configuration.  If INDIVIDUALLY is defined, the inclusion
	of subpackages is based on whether their compile-time tags are
	defined.  Otherwise, we automatically enable all the subpackages.  */

// TODO:  These are all going away.  Some of them to config.h, and the rest to
// be made into initialization flags for interpreters (which, of course, depends
// on interpreter instances being implemented first).
#ifndef INDIVIDUALLY

#define ARRAY			// Array subscripting words
#define BREAK			// Asynchronous break facility
#define COMPILERW		// Compiler-writing words
#define CONIO			// Interactive console I/O
#define DEFFIELDS		// Definition field access for words
#define DOUBLE			// Double word primitives (2DUP)
#define EVALUATE		// The EVALUATE primitive
#define FILEIO			// File I/O primitives
#define MATH			// Math functions
#define MEMMESSAGE		// Print message for stack/heap errors
#define PROLOGUE		// Prologue processing and auto-init
#define REAL			// Floating point numbers
#define SHORTCUTA		// Shortcut integer arithmetic words
#define SHORTCUTC		// Shortcut integer comparison
#define STRING			// String functions
#define SYSTEM			// System command function
#define FFI			// Foreign function interface
#define PROCESS			// Process-level facilities

#define BOUNDS_CHECK		// For the stack, heap, return stack, and arrays
#define UNRESTRICTED_POINTERS	// Pointers anywhere, not just inside the heap.
#define MATH_CHECK		// x/0 errors.
#define COMPILATION_SAFETY	// The Compiling macro.
#define TRACE			// Execution tracing
#define WALKBACK		// Walkback trace
#define WORDSUSED		// Logging of words used and unused

#endif				// !INDIVIDUALLY

#include "pezdef.h"

#ifdef MATH
#include <math.h>
#endif

/* LINTLIBRARY */

/* Implicit functions (work for all numeric types). */

#ifdef abs
#undef abs
#endif
#define abs(x)	 ((x) < 0	? -(x) : (x))
#define max(a,b) ((a) >  (b) ?	(a) : (b))
#define min(a,b) ((a) <= (b) ?	(a) : (b))

/*  Globals imported  */

/*  Data types	*/

typedef enum { False = 0, True = 1 } Boolean;

#define EOS	 '\0'		/* End of string characters */

#define Truth	-1L		/* Stack value for truth */
#define Falsity 0L		/* Stack value for falsity */

/* 
	Utility definition to get an array's element count (at compile time, and
	provided that you're in the same scope as the declaration).   For example:

		int  arr[] = {1,2,3,4,5};
		...
		printf("%d", ELEMENTS(arr));

	would print a five.  ELEMENTS("abc") can also be used to tell how many bytes
	are in a string constant INCLUDING THE TRAILING NULL. 
*/

#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

/*  Globals visible to calling programs  */

// TODO:  Bascially all of these globals are going into the struct that
// represents an instance of a Pez interpreter.

pez_int pez_stklen = 1000;	/* Evaluation stack length */
pez_int pez_rstklen = 1000;	/* Return stack length */
pez_int pez_heaplen = 20000;	/* Heap length */
pez_int pez_ltempstr = 2560;	/* Temporary string buffer length */
pez_int pez_ntempstr = 4;	/* Number of temporary string buffers */

pez_int pez_trace = Falsity;	/* Tracing if true */
pez_int pez_walkback = Truth;	/* Walkback enabled if true */
pez_int pez_comment = Falsity;	/* Currently ignoring a comment */
pez_int pez_redef = Truth;	/* Allow redefinition without issuing
				   the "not unique" message. */
pez_int pez_errline = 0;	/* Line where last pez_load failed */

pez_int pez_argc = 0;
char **pez_argv = { NULL };

/*  Local variables  */

	/* The evaluation stack */

Exported stackitem *stack = NULL;	/* Evaluation stack */
Exported stackitem *stk;	/* Stack pointer */
Exported stackitem *stackbot;	/* Stack bottom */
Exported stackitem *stacktop;	/* Stack top */

	/* The return stack */

Exported dictword ***rstack = NULL;	/* Return stack */
Exported dictword ***rstk;	/* Return stack pointer */
Exported dictword ***rstackbot;	/* Return stack bottom */
Exported dictword ***rstacktop;	/* Return stack top */

	/* The heap */

Exported stackitem *heap = NULL;	/* Allocation heap */
Exported stackitem *hptr;	/* Heap allocation pointer */
Exported stackitem *heapbot;	/* Bottom of heap (temp string buffer) */
Exported stackitem *heaptop;	/* Top of heap */

	/* The dictionary */

Exported dictword *dict = NULL;	/* Dictionary chain head */
Exported dictword *dictprot = NULL;	/* First protected item in dictionary */

	/* The temporary string buffers */

Exported char **strbuf = NULL;	/* Table of pointers to temp strings */
Exported int cstrbuf = 0;	/* Current temp string */

	/* The walkback trace stack */

#ifdef WALKBACK
static dictword **wback = NULL;	/* Walkback trace buffer */
static dictword **wbptr;	/* Walkback trace pointer */
#endif				/* WALKBACK */

#ifdef MEMSTAT
Exported stackitem *stackmax;	/* Stack maximum excursion */
Exported dictword ***rstackmax;	/* Return stack maximum excursion */
Exported stackitem *heapmax;	/* Heap maximum excursion */
#endif

#ifdef FILEIO
static char *fopenmodes[] = {
#ifdef FBmode
#define FMspecial
	/* Fopen() mode table for systems that require a "b" in the
	   mode string for binary files. */
	"", "r", "", "r+",
	"", "rb", "", "r+b",
	"", "", "w", "w+",
	"", "", "wb", "w+b"
#endif
#ifndef FMspecial
		/* Default fopen() mode table for SVID-compatible systems not
		   overridden by a special table above. */
		"", "r", "", "r+",
	"", "r", "", "r+",
	"", "", "w", "w+",
	"", "", "w", "w+"
#endif
};

#endif				/* FILEIO */

static char tokbuf[128];	/* Token buffer */
static char *instream = NULL;	/* Current input stream line */
static long tokint;		/* Scanned integer */
#ifdef REAL
static pez_real tokreal;	/* Scanned real number */
#ifdef ALIGNMENT
Exported pez_real rbuf0, rbuf1, rbuf2;	/* Real temporary buffers */
#endif
#endif
static long base = 10;		/* Number base */
Exported dictword **ip = NULL;	/* Instruction pointer */
Exported dictword *curword = NULL;	/* Current word being executed */
static int evalstat = PEZ_SNORM;	/* Evaluator status */
static Boolean defpend = False;	/* Token definition pending */
static Boolean forgetpend = False;	/* Forget pending */
static Boolean tickpend = False;	/* Take address of next word */
static Boolean ctickpend = False;	/* Compile-time tick ['] pending */
static Boolean cbrackpend = False;	/* [COMPILE] pending */
static Boolean tail_call_pending = False;
Exported dictword *createword = NULL;	/* Address of word pending creation */
static Boolean stringlit = False;	/* String literal anticipated */
#ifdef BREAK
static Boolean broken = False;	/* Asynchronous break received */
#endif

#ifdef COPYRIGHT
#ifndef HIGHC
#ifndef lint
static
#endif
#endif
char copyright[] = "PEZ: This program is in the public domain.";
#endif

/* The following static cells save the compile addresses of words
   generated by the compiler.  They are looked up immediately after
   the dictionary is created.  This makes the compiler much faster
   since it doesn't have to look up internally-reference words, and,
   far more importantly, keeps it from being spoofed if a user redefines
   one of the words generated by the compiler.	*/

static stackitem s_exit, s_lit, s_flit, s_strlit, s_dotparen,
	s_qbranch, s_branch, s_xdo, s_xqdo, s_xloop, s_pxloop, s_abortq;

/*  Forward functions  */

STATIC void exword(), trouble();
#ifdef MATH_CHECK
STATIC void notcomp(), divzero();
#endif
#ifdef WALKBACK
STATIC void pwalkback();
#endif

/*  ALLOC  --  Allocate memory and error upon exhaustion.  */

static char *alloc(size)
unsigned int size;
{
	char *cp = malloc(size);

	/* printf("\nAlloc %u", size); */
	if(cp == NULL) {
		fprintf(stderr, "\n\nOut of memory!  %u bytes requested.\n",
			size);
		abort();
	}
	return cp;
}

/*  UCASE  --  Force letters in string to upper case.  */

static void ucase(c)
char *c;
{
	char ch;

	while((ch = *c) != EOS) {
		if(islower(ch))
			*c = toupper(ch);
		c++;
	}
}

/*  TOKEN  --  Scan a token and return its type.  */

static int token(cp)
char **cp;
{
	char *sp = *cp;

	while(True) {
		char *tp = tokbuf;
		int tl = 0;
		Boolean istring = False, rstring = False;

		if(pez_comment) {
			while(*sp != ')') {
				if(*sp == EOS) {
					*cp = sp;
					return TokNull;
				}
				sp++;
			}
			sp++;
			pez_comment = Falsity;
		}

		while(isspace(*sp))	/* Skip leading blanks */
			sp++;

		if(*sp == '"') {	/* Is this a string ? */

			/* Assemble string token. */

			sp++;
			while(True) {
				char c = *sp++;

				if(c == '"') {
					sp++;
					*tp++ = EOS;
					break;
				} else if(c == EOS) {
					rstring = True;
					*tp++ = EOS;
					break;
				}
				if(c == '\\') {
					c = *sp++;
					if(c == EOS) {
						rstring = True;
						break;
					}
					// TODO:  lookup table.
					switch (c) {
					case 'b':
						c = '\b';
						break;
					case 'n':
						c = '\n';
						break;
					case 'r':
						c = '\r';
						break;
					case 't':
						c = '\t';
						break;
					default:
						break;
					}
				}
				if(tl < (sizeof tokbuf) - 1) {
					*tp++ = c;
					tl++;
				} else {
					rstring = True;
				}
			}
			istring = True;
		} else {

			/* Scan the next raw token */

			while(True) {
				char c = *sp++;

				if(c == EOS || isspace(c)) {
					*tp++ = EOS;
					break;
				}
				if(tl < (sizeof tokbuf) - 1) {
					*tp++ = c;
					tl++;
				}
			}
		}
		*cp = --sp;	/* Store end of scan pointer */

		if(istring) {
			if(rstring) {
#ifdef MEMMESSAGE
				printf("\nRunaway string: %s\n", tokbuf);
#endif
				evalstat = PEZ_RUNSTRING;
				return TokNull;
			}
			return TokString;
		}

		if(tokbuf[0] == EOS)
			return TokNull;

		/* See if token is a comment to end of line character.  If so,
		 * discard the rest of the line and return null for this token
		 * request.  
		 */

		if(strcmp(tokbuf, "#") == 0 || strcmp(tokbuf, "#!") == 0) {
			while(*sp != EOS)
				sp++;
			*cp = sp;
			return TokNull;
		}

		/* See if this token is a comment open delimiter.  If so, set to
		   ignore all characters until the matching comment close
		   delimiter. */

		if(strcmp(tokbuf, "(") == 0) {
			while(*sp != EOS) {
				if(*sp == ')')
					break;
				sp++;
			}
			if(*sp == ')') {
				sp++;
				continue;
			}
			pez_comment = Truth;
			*cp = sp;
			return TokNull;
		}

		/* See if the token is a number. */

		if(isdigit(tokbuf[0]) || tokbuf[0] == '-') {
			char tc;
			char *tcp;

#ifdef OS2
			/* Compensate for error in OS/2 sscanf() library function */
			if((tokbuf[0] == '-') &&
			   !(isdigit(tokbuf[1]) ||
				 (((tokbuf[1] == 'x') || (tokbuf[1] == 'X')) &&
				  isxdigit(tokbuf[2])))) {
				return TokWord;
			}
#endif				/* OS2 */
#ifdef USE_SSCANF
			if(sscanf(tokbuf, "%li%c", &tokint, &tc) == 1)
				return TokInt;
#else
			tokint = strtoul(tokbuf, &tcp, 0);
			if(*tcp == 0) {
				return TokInt;
			}
#endif
#ifdef REAL
			if(sscanf(tokbuf, "%lf%c", &tokreal, &tc) == 1) {
				return TokReal;
			}
#endif
		}
		return TokWord;
	}
}

/*  LOOKUP  --	Look up token in the dictionary.  */

static dictword *lookup(tkname)
char *tkname;
{
	dictword *dw = dict;

	ucase(tkname);		/* Force name to upper case */
	while(dw != NULL) {
		if(!(dw->wname[0] & WORDHIDDEN) &&
		   (strcmp(dw->wname + 1, tkname) == 0)) {
#ifdef WORDSUSED
			*(dw->wname) |= WORDUSED;	/* Mark this word used */
#endif
			break;
		}
		dw = dw->wnext;
	}
	return dw;
}

/* Gag me with a spoon!  Does no compiler but Turbo support
   #if defined(x) || defined(y) ?? */
#ifdef EXPORT
#define FgetspNeeded
#endif
#ifdef FILEIO
#ifndef FgetspNeeded
#define FgetspNeeded
#endif
#endif

#ifdef FgetspNeeded

/*  PEZ_FGETSP	--  Portable database version of FGETS.  This reads the
			next line into a buffer a la fgets().  A line is
			delimited by either a carriage return or a line
			feed, optionally followed by the other character
			of the pair.  The string is always null
			terminated, and limited to the length specified - 1
			(excess characters on the line are discarded.
			The string is returned, or NULL if end of file is
			encountered and no characters were stored.	No end
			of line character is stored in the string buffer.
			*/

Exported char *pez_fgetsp(s, n, stream)
char *s;
int n;
FILE *stream;
{
	int i = 0, ch;

	while(True) {
		ch = getc(stream);
		if(ch == EOF) {
			if(i == 0)
				return NULL;
			break;
		}
		if(ch == '\r') {
			ch = getc(stream);
			if(ch != '\n')
				ungetc(ch, stream);
			break;
		}
		if(ch == '\n') {
			ch = getc(stream);
			if(ch != '\r')
				ungetc(ch, stream);
			break;
		}
		if(i < (n - 1))
			s[i++] = ch;
	}
	s[i] = EOS;
	return s;
}
#endif				/* FgetspNeeded */

/*  PEZ_MEMSTAT  --  Print memory usage summary.  */

#ifdef MEMSTAT
void pez_memstat()
{
	static char fmt[] = "   %-12s %6ld	%6ld	%6ld	   %3ld\n";

	printf("\n			 Memory Usage Summary\n\n"
		   "				 Current   Maximum	Items	 Percent\n"
		   "  Memory Area	 usage	 used	allocated   in use \n");

	printf(fmt, "Stack",
		   ((long)(stk - stack)),
		   ((long)(stackmax - stack)),
		   pez_stklen, (100L * (stk - stack)) / pez_stklen);
	printf(fmt, "Return stack",
		   ((long)(rstk - rstack)),
		   ((long)(rstackmax - rstack)),
		   pez_rstklen, (100L * (rstk - rstack)) / pez_rstklen);
	printf(fmt, "Heap",
		   ((long)(hptr - heap)),
		   ((long)(heapmax - heap)),
		   pez_heaplen, (100L * (hptr - heap)) / pez_heaplen);
}
#endif				/* MEMSTAT */

/*  Primitive implementing functions.  */

/*  ENTER  --  Enter word in dictionary.  Given token for word's
		   name and initial values for its attributes, returns
		   the newly-allocated dictionary item. */

static void enter(tkname)
char *tkname;
{
	/* Allocate name buffer */
	createword->wname = alloc(((unsigned int)strlen(tkname) + 2));
	createword->wname[0] = 0;	/* Clear flags */
	strcpy(createword->wname + 1, tkname);	/* Copy token to name buffer */
	createword->wnext = dict;	/* Chain rest of dictionary to word */
	dict = createword;	/* Put word at head of dictionary */
}

#ifdef Keyhit

/*  KBQUIT  --	If this system allows detecting key presses, handle
		the pause, resume, and quit protocol for the word
		listing facilities.  */

static Boolean kbquit()
{
	int key;

	if((key = Keyhit()) != 0) {
		printf("\nPress RETURN to stop, any other key to continue: ");
		while((key = Keyhit()) == 0);
		if(key == '\r' || (key == '\n'))
			return True;
	}
	return False;
}
#endif				/* Keyhit */

/*  Primitive word definitions.  */

#ifdef COMPILATION_SAFETY
#define Compiling
#else
#define Compiling if (state == Falsity) {notcomp(); return;}
#endif
#define Compconst(x) Ho(1); Hstore = (stackitem)(x)
#define Skipstring ip += *((char *)ip)

prim P_plus()
{				/* Add two numbers */
	Sl(2);
/* printf("PLUS %lx + %lx = %lx\n", S1, S0, (S1 + S0)); */
	S1 += S0;
	Pop;
}

prim P_minus()
{				/* Subtract two numbers */
	Sl(2);
	S1 -= S0;
	Pop;
}

prim P_times()
{				/* Multiply two numbers */
	Sl(2);
	S1 *= S0;
	Pop;
}

prim P_div()
{				/* Divide two numbers */
	Sl(2);
#ifdef MATH_CHECK
	if(S0 == 0) {
		divzero();
		return;
	}
#endif
	S1 /= S0;
	Pop;
}

prim P_mod()
{				/* Take remainder */
	Sl(2);
#ifdef MATH_CHECK
	if(S0 == 0) {
		divzero();
		return;
	}
#endif
	S1 %= S0;
	Pop;
}

prim P_divmod()
{				/* Compute quotient and remainder */
	stackitem quot;

	Sl(2);
#ifdef MATH_CHECK
	if(S0 == 0) {
		divzero();
		return;
	}
#endif
	quot = S1 / S0;
	S1 %= S0;
	S0 = quot;
}

prim P_min()
{				/* Take minimum of stack top */
	Sl(2);
	S1 = min(S1, S0);
	Pop;
}

prim P_max()
{				/* Take maximum of stack top */
	Sl(2);
	S1 = max(S1, S0);
	Pop;
}

prim P_neg()
{				/* Negate top of stack */
	Sl(1);
	S0 = -S0;
}

prim P_abs()
{				/* Take absolute value of top of stack */
	Sl(1);
	S0 = abs(S0);
}

prim P_equal()
{				/* Test equality */
	Sl(2);
	S1 = (S1 == S0) ? Truth : Falsity;
	Pop;
}

prim P_unequal()
{				/* Test inequality */
	Sl(2);
	S1 = (S1 != S0) ? Truth : Falsity;
	Pop;
}

prim P_gtr()
{				/* Test greater than */
	Sl(2);
	S1 = (S1 > S0) ? Truth : Falsity;
	Pop;
}

prim P_lss()
{				/* Test less than */
	Sl(2);
	S1 = (S1 < S0) ? Truth : Falsity;
	Pop;
}

prim P_geq()
{				/* Test greater than or equal */
	Sl(2);
	S1 = (S1 >= S0) ? Truth : Falsity;
	Pop;
}

prim P_leq()
{				/* Test less than or equal */
	Sl(2);
	S1 = (S1 <= S0) ? Truth : Falsity;
	Pop;
}

prim P_and()
{				/* Logical and */
	Sl(2);
/* printf("AND %lx & %lx = %lx\n", S1, S0, (S1 & S0)); */
	S1 &= S0;
	Pop;
}

prim P_or()
{				/* Logical or */
	Sl(2);
	S1 |= S0;
	Pop;
}

prim P_xor()
{				/* Logical xor */
	Sl(2);
	S1 ^= S0;
	Pop;
}

prim P_not()
{				/* Logical negation */
	Sl(1);
	S0 = ~S0;
}

prim P_shift()
{				/* Shift:  value nbits -- value */
	Sl(1);
	S1 = (S0 < 0) ? (((unsigned long)S1) >> (-S0)) :
		(((unsigned long)S1) << S0);
	Pop;
}

#ifdef SHORTCUTA

prim P_1plus()
{				/* Add one */
	Sl(1);
	S0++;
}

prim P_2plus()
{				/* Add two */
	Sl(1);
	S0 += 2;
}

prim P_1minus()
{				/* Subtract one */
	Sl(1);
	S0--;
}

prim P_2minus()
{				/* Subtract two */
	Sl(1);
	S0 -= 2;
}

prim P_2times()
{				/* Multiply by two */
	Sl(1);
	S0 *= 2;
}

prim P_2div()
{				/* Divide by two */
	Sl(1);
	S0 /= 2;
}

#endif				/* SHORTCUTA */

#ifdef SHORTCUTC

prim P_0equal()
{				/* Equal to zero ? */
	Sl(1);
	S0 = (S0 == 0) ? Truth : Falsity;
}

prim P_0notequal()
{				/* Not equal to zero ? */
	Sl(1);
	S0 = (S0 != 0) ? Truth : Falsity;
}

prim P_0gtr()
{				/* Greater than zero ? */
	Sl(1);
	S0 = (S0 > 0) ? Truth : Falsity;
}

prim P_0lss()
{				/* Less than zero ? */
	Sl(1);
	S0 = (S0 < 0) ? Truth : Falsity;
}

#endif				/* SHORTCUTC */

/*  Storage allocation (heap) primitives  */

prim P_here()
{				/* Push current heap address */
	So(1);
	Push = (stackitem)hptr;
}

prim P_bang()
{				/* Store value into address */
	Sl(2);
	Hpc(S0);
	*((stackitem *)S0) = S1;
	Pop2;
}

prim P_at()
{				/* Fetch value from address */
	Sl(1);
	Hpc(S0);
	S0 = *((stackitem *)S0);
}

prim P_plusbang()
{				/* Add value at specified address */
	Sl(2);
	Hpc(S0);
	*((stackitem *)S0) += S1;
	Pop2;
}

/*
   ( addr -- )
   Increments (by 1) the variable at the specified address.
*/
prim P_1plusbang()
{
	Sl(1);
	Hpc(S0);
	(*((stackitem *)S0))++;
	Pop;
}

prim P_allot()
{				/* Allocate heap bytes */
	stackitem n;

	Sl(1);
	n = (S0 + (sizeof(stackitem) - 1)) / sizeof(stackitem);
	Pop;
	Ho(n);
	hptr += n;
}

prim P_comma()
{				/* Store one item on heap */
	Sl(1);
	Ho(1);
	Hstore = S0;
	Pop;
}

prim P_cbang()
{				/* Store byte value into address */
	Sl(2);
	Hpc(S0);
	*((unsigned char *)S0) = S1;
	Pop2;
}

prim P_cat()
{				/* Fetch byte value from address */
	Sl(1);
	Hpc(S0);
	S0 = *((unsigned char *)S0);
}

prim P_ccomma()
{				/* Store one byte on heap */
	unsigned char *chp;

	Sl(1);
	Ho(1);
	chp = ((unsigned char *)hptr);
	*chp++ = S0;
	hptr = (stackitem *)chp;
	Pop;
}

prim P_cequal()
{				/* Align heap pointer after storing *//* a series of bytes. */
	stackitem n = (((stackitem)hptr) - ((stackitem)heap)) %
		(sizeof(stackitem));

	if(n != 0) {
		char *chp = ((char *)hptr);

		chp += sizeof(stackitem) - n;
		hptr = ((stackitem *)chp);
	}
}

/*  Variable and constant primitives  */

prim P_var()
{				/* Push body address of current word */
	So(1);
	Push = (stackitem)(((stackitem *)curword) + Dictwordl);
}

Exported void P_create()
{				/* Create new word */
	defpend = True;		/* Set definition pending */
	Ho(Dictwordl);
	createword = (dictword *)hptr;	/* Develop address of word */
	createword->wname = NULL;	/* Clear pointer to name string */
	createword->wcode = P_var;	/* Store default code */
	hptr += Dictwordl;	/* Allocate heap space for word */
}

prim P_forget()
{				/* Forget word */
	forgetpend = True;	/* Mark forget pending */
}

prim P_variable()
{				/* Declare variable */
	P_create();		/* Create dictionary item */
	Ho(1);
	Hstore = 0;		/* Initial value = 0 */
}

prim P_con()
{				/* Push value in body */
	So(1);
	Push = *(((stackitem *)curword) + Dictwordl);
}

prim P_constant()
{				/* Declare constant */
	Sl(1);
	P_create();		/* Create dictionary item */
	createword->wcode = P_con;	/* Set code to constant push */
	Ho(1);
	Hstore = S0;		/* Store constant value in body */
	Pop;
}

prim P_cellsize()
{
	So(1);
	Push = sizeof(long);
}

prim P_floatsize()
{
	So(1);
	Push = sizeof(double);
}

/*  Array primitives  */

#ifdef ARRAY
prim P_arraysub()
{				/* Array subscript calculation *//* sub1 sub2 ... subn -- addr */
	int i, offset, esize, nsubs;
	stackitem *array;
	stackitem *isp;

	Sl(1);
	array = (((stackitem *)curword) + Dictwordl);
	Hpc(array);
	nsubs = *array++;	/* Load number of subscripts */
	esize = *array++;	/* Load element size */
#ifndef BOUNDS_CHECK
	isp = &S0;
	for(i = 0; i < nsubs; i++) {
		stackitem subn = *isp--;

		if(subn < 0 || subn >= array[i])
			trouble("Subscript out of range");
	}
#endif
	isp = &S0;
	offset = *isp;		/* Load initial offset */
	for(i = 1; i < nsubs; i++)
		offset = (offset * (*(++array))) + *(--isp);
	Npop(nsubs - 1);
	/* Calculate subscripted address.  We start at the current word,
	   advance to the body, skip two more words for the subscript count
	   and the fundamental element size, then skip the subscript bounds
	   words (as many as there are subscripts).  Then, finally, we
	   can add the calculated offset into the array. */
	S0 = (stackitem)(((char *)(((stackitem *)curword) +
					 Dictwordl + 2 + nsubs)) +
			  (esize * offset));
}

/*
   ( sub1 sub2 ... subn n esize -- array )
   Declares an array and stores its elements.
*/
prim P_array()
{				/* Declare array *//* sub1 sub2 ... subn n esize -- array */
	int i, nsubs, asize = 1;
	stackitem *isp;

	Sl(2);
	if(S0 <= 0)
		trouble("Bad array element size");
	if(S1 <= 0)
		trouble("Bad array subscript count");

	nsubs = S1;		/* Number of subscripts */
	Sl(nsubs + 2);		/* Verify that dimensions are present */

	/* Calculate size of array as the product of the subscripts */

	asize = S0;		/* Fundamental element size */
	isp = &S2;
	for(i = 0; i < nsubs; i++) {
		if(*isp <= 0)
			trouble("Bad array dimension");
		asize *= *isp--;
	}

	asize = (asize + (sizeof(stackitem) - 1)) / sizeof(stackitem);
	Ho(asize + nsubs + 2);	/* Reserve space for array and header */
	P_create();		/* Create variable */
	createword->wcode = P_arraysub;	/* Set method to subscript calculate */
	Hstore = nsubs;		/* Header <- Number of subscripts */
	Hstore = S0;		/* Header <- Fundamental element size */
	isp = &S2;
	for(i = 0; i < nsubs; i++) {	/* Header <- Store subscripts */
		Hstore = *isp--;
	}
	while(asize-- > 0)	/* Clear the array to zero */
		Hstore = 0;
	Npop(nsubs + 2);
}
#endif				/* ARRAY */

/*  String primitives  */

#ifdef STRING

prim P_strlit()
{				/* Push address of string literal */
	So(1);
	Push = (stackitem)(((char *)ip) + 1);

	trace {
		printf("\"%s\" ", (((char *)ip) + 1));
	}

	Skipstring;		/* Advance IP past it */
}

prim P_string()
{				/* Create string buffer */
	Sl(1);
	Ho((S0 + 1 + sizeof(stackitem)) / sizeof(stackitem));
	P_create();		/* Create variable */
	/* Allocate storage for string */
	hptr += (S0 + 1 + sizeof(stackitem)) / sizeof(stackitem);
	Pop;
}

prim P_strcpy()
{				/* Copy string to address on stack */
	Sl(2);
	Hpc(S0);
	Hpc(S1);
	strcpy((char *)S0, (char *)S1);
	Pop2;
}

prim P_strcat()
{				/* Append string to address on stack */
	Sl(2);
	Hpc(S0);
	Hpc(S1);
	strcat((char *)S0, (char *)S1);
	Pop2;
}

prim P_strlen()
{				/* Take length of string on stack top */
	Sl(1);
	Hpc(S0);
	S0 = strlen((char *)S0);
}

prim P_strcmp()
{				/* Compare top two strings on stack */
	int i;

	Sl(2);
	Hpc(S0);
	Hpc(S1);
	i = strcmp((char *)S1, (char *)S0);
	S1 = (i == 0) ? 0L : ((i > 0) ? 1L : -1L);
	Pop;
}

prim P_strchar()
{				/* Find character in string */
	Sl(2);
	Hpc(S0);
	Hpc(S1);
	S1 = (stackitem)strchr((char *)S1, *((char *)S0));
	Pop;
}

prim P_substr()
{				/* Extract and store substring *//* source start length/-1 dest -- */
	long sl, sn;
	char *ss, *sp, *se, *ds;

	Sl(4);
	Hpc(S0);
	Hpc(S3);
	sl = strlen(ss = ((char *)S3));
	se = ss + sl;
	sp = ((char *)S3) + S2;
	if((sn = S1) < 0)
		sn = 999999L;
	ds = (char *)S0;
	while(sn-- && (sp < se))
		*ds++ = *sp++;
	*ds++ = EOS;
	Npop(4);
}

/*
   ( val fmt buf -- )
   Equivalent to sprintf(buf, fmt, val);
*/
prim P_strform()
{
	Sl(2);
	Hpc(S0);
	Hpc(S1);
	sprintf((char *)S0, (char *)S1, S2);
	Npop(3);
}

#ifdef REAL
prim P_fstrform()
{				/* Format real using sprintf() *//* rvalue "%6.2f" str -- */
	Sl(2 + Realsize);
	Hpc(S0);
	Hpc(S1);
	sprintf((char *)S0, (char *)S1, ((pez_real *)(stk - 2))[-1]);
	Npop(2 + Realsize);
}
#endif				/* REAL */

prim P_strint()
{				/* String to integer *//* str -- endptr value */
	stackitem is;
	char *eptr;

	Sl(1);
	So(1);
	Hpc(S0);
	is = strtoul((char *)S0, &eptr, 0);
	S0 = (stackitem)eptr;
	Push = is;
}

#ifdef REAL
prim P_strreal()
{				/* String to real *//* str -- endptr value */
	int i;
	union {
		pez_real fs;
		stackitem fss[Realsize];
	} fsu;
	char *eptr;

	Sl(1);
	So(Realsize);
	Hpc(S0);
	fsu.fs = strtod((char *)S0, &eptr);
	S0 = (stackitem)eptr;
	for(i = 0; i < Realsize; i++) {
		Push = fsu.fss[i];
	}
}
#endif				/* REAL */
#endif				/* STRING */

/*  Floating point primitives  */

#ifdef REAL

prim P_flit()
{				/* Push floating point literal */
	int i;

	So(Realsize);
	trace {
		pez_real tr;

		memcpy((char *)&tr, (char *)ip, sizeof(pez_real));
		printf("%g ", tr);
	}

	for(i = 0; i < Realsize; i++) {
		Push = (stackitem) * ip++;
	}
}

prim P_fplus()
{				/* Add floating point numbers */
	Sl(2 * Realsize);
	SREAL1(REAL1 + REAL0);
	Realpop;
}

prim P_fminus()
{				/* Subtract floating point numbers */
	Sl(2 * Realsize);
	SREAL1(REAL1 - REAL0);
	Realpop;
}

prim P_ftimes()
{				/* Multiply floating point numbers */
	Sl(2 * Realsize);
	SREAL1(REAL1 * REAL0);
	Realpop;
}

prim P_fdiv()
{				/* Divide floating point numbers */
	Sl(2 * Realsize);
#ifdef MATH_CHECK
	if(REAL0 == 0.0) {
		divzero();
		return;
	}
#endif
	SREAL1(REAL1 / REAL0);
	Realpop;
}

prim P_fmin()
{				/* Minimum of top two floats */
	Sl(2 * Realsize);
	SREAL1(min(REAL1, REAL0));
	Realpop;
}

prim P_fmax()
{				/* Maximum of top two floats */
	Sl(2 * Realsize);
	SREAL1(max(REAL1, REAL0));
	Realpop;
}

prim P_fneg()
{				/* Negate top of stack */
	Sl(Realsize);
	SREAL0(-REAL0);
}

prim P_fabs()
{				/* Absolute value of top of stack */
	Sl(Realsize);
	SREAL0(abs(REAL0));
}

prim P_fequal()
{				/* Test equality of top of stack */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 == REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_funequal()
{				/* Test inequality of top of stack */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 != REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_fgtr()
{				/* Test greater than */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 > REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_flss()
{				/* Test less than */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 < REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_fgeq()
{				/* Test greater than or equal */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 >= REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_fleq()
{				/* Test less than or equal */
	stackitem t;

	Sl(2 * Realsize);
	t = (REAL1 <= REAL0) ? Truth : Falsity;
	Realpop2;
	Push = t;
}

prim P_fdot()
{				/* Print floating point top of stack */
	double f;
	memcpy(&f, stk - 1, sizeof(double));
	Sl(Realsize);
	printf("%f ", REAL0);
	Realpop;
}

prim P_float()
{				/* Convert integer to floating */
	pez_real r;

	Sl(1)
		So(Realsize - 1);
	r = S0;
	stk += Realsize - 1;
	SREAL0(r);
}

prim P_fix()
{				/* Convert floating to integer */
	stackitem i;

	Sl(Realsize);
	i = (int)REAL0;
	Realpop;
	Push = i;
}

/*
   ( -- time-as-float )
   Returns the time as number of seconds since the epoch, as a floating point
   value.
*/
prim P_ftime()
{
	struct timeval tv;
	double d;

	So(Realsize);
	gettimeofday(&tv, 0);
	d = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	Realpush(d);
}

#ifdef MATH

#define Mathfunc(x) Sl(Realsize); SREAL0(x(REAL0))

prim P_acos()
{				/* Arc cosine */
	Mathfunc(acos);
}

prim P_asin()
{				/* Arc sine */
	Mathfunc(asin);
}

prim P_atan()
{				/* Arc tangent */
	Mathfunc(atan);
}

prim P_atan2()
{				/* Arc tangent:  y x -- atan */
	Sl(2 * Realsize);
	SREAL1(atan2(REAL1, REAL0));
	Realpop;
}

prim P_cos()
{				/* Cosine */
	Mathfunc(cos);
}

prim P_exp()
{				/* E ^ x */
	Mathfunc(exp);
}

prim P_log()
{				/* Natural log */
	Mathfunc(log);
}

prim P_pow()
{				/* X ^ Y */
	Sl(2 * Realsize);
	SREAL1(pow(REAL1, REAL0));
	Realpop;
}

prim P_sin()
{				/* Sine */
	Mathfunc(sin);
}

prim P_sqrt()
{				/* Square root */
	Mathfunc(sqrt);
}

prim P_tan()
{				/* Tangent */
	Mathfunc(tan);
}

#undef Mathfunc
#endif				/* MATH */
#endif				/* REAL */

/*  Console I/O primitives  */

#ifdef CONIO

prim P_hex()
{
	base = 16;
}

prim P_decimal()
{
	base = 10;
}

prim P_argc()
{
	char **pa;
	if(!pez_argc && pez_argv[0]) {
		pa = pez_argv;
		while(*pa++)
			pez_argc++;
	}

	Push = pez_argc;
}

prim P_argv()
{
	Push = (stackitem)pez_argv;
}

prim P_dot()
{				/* Print top of stack, pop it */
	Sl(1);
	printf(base == 16 ? "%lx " : "%ld ", S0);
	Pop;
}

prim P_question()
{				/* Print value at address */
	Sl(1);
	Hpc(S0);
	printf(base == 16 ? "%lx " : "%ld ", *((stackitem *)S0));
	Pop;
}

prim P_cr()
{				/* Carriage return */
	printf("\n");
	fflush(stdout);
}

prim P_dots()
{				/* Print entire contents of stack */
	stackitem *tsp;

	printf("Stack: ");
	if(stk == stackbot)
		printf("Empty. ");
	else {
		for(tsp = stack; tsp < stk; tsp++) {
			printf(base == 16 ? "%lx " : "%ld ", *tsp);
		}
	}
}

prim P_dotquote()
{				/* Print literal string that follows */
	Compiling;
	stringlit = True;	/* Set string literal expected */
	Compconst(s_dotparen);	/* Compile .( word */
}

/*
   ( -- )
   Prints the following inline string literal.
*/
prim P_dotparen()
{
	if(ip) {
		printf("%s", ((char *)ip) + 1);
		Skipstring;	// So we don't execute the string.
	} else {
		// We have to sort of wing it if the string isn't yet available.
		stringlit = True;
	}
}

prim P_type()
{				/* Print string pointed to by stack */
	Sl(1);
	Hpc(S0);
	printf("%s", (char *)S0);
	Pop;
}

/*
   Print the string at the top of the stack, followed by \n.
   ( string -- )
*/
prim P_puts()
{
	Sl(1);
	// Hpc(S0);
	puts((char *)S0);
	Pop;
}

prim P_words()
{				/* List words */
#ifndef Keyhit
	int key = 0;
#endif
	dictword *dw = dict;

	while(dw != NULL) {

		printf("\n%s", dw->wname + 1);
		dw = dw->wnext;
#ifdef Keyhit
		if(kbquit()) {
			break;
		}
#else
		/* If this system can't trap keystrokes, just stop the WORDS
		   listing after 20 words. */
		if(++key >= 20)
			break;
#endif
	}
	printf("\n");
}
#endif				/* CONIO */

#ifdef FILEIO

prim P_file()
{				/* Declare file */
	Ho(2);
	P_create();		/* Create variable */
	Hstore = FileSent;	/* Store file sentinel */
	Hstore = 0;		/* Mark file not open */
}

prim P_fopen()
{				/* Open file: fname fmodes fd -- flag */
	FILE *fd;
	stackitem stat;

	Sl(3);
	Hpc(S2);
	Hpc(S0);
	Isfile(S0);
	fd = fopen((char *)S2, fopenmodes[S1]);
	if(fd == NULL) {
		stat = Falsity;
	} else {
		*(((stackitem *)S0) + 1) = (stackitem)fd;
		stat = Truth;
	}
	Pop2;
	S0 = stat;
}

prim P_fclose()
{				/* Close file: fd -- */
	Sl(1);
	Hpc(S0);
	Isfile(S0);
	Isopen(S0);
	fclose(FileD(S0));
	*(((stackitem *)S0) + 1) = (stackitem)NULL;
	Pop;
}

prim P_fdelete()
{				/* Delete file: fname -- flag */
	Sl(1);
	Hpc(S0);
	S0 = (unlink((char *)S0) == 0) ? Truth : Falsity;
}

prim P_fgetline()
{				/* Get line: fd string -- flag */
	Sl(2);
	Hpc(S0);
	Isfile(S1);
	Isopen(S1);
	if(pez_fgetsp((char *)S0, 132, FileD(S1)) == NULL) {
		S1 = Falsity;
	} else {
		S1 = Truth;
	}
	Pop;
}

prim P_fputline()
{				/* Put line: string fd -- flag */
	Sl(2);
	Hpc(S1);
	Isfile(S0);
	Isopen(S0);
	if(fputs((char *)S1, FileD(S0)) == EOF) {
		S1 = Falsity;
	} else {
		S1 = putc('\n', FileD(S0)) == EOF ? Falsity : Truth;
	}
	Pop;
}

/*
   ( stream len buf -- bytes-read )
   Reads from an input stream up to len bytes, puts them in buf, and returns the
   the actual number of bytes read.
*/
prim P_fread()
{
	Sl(3);
	Hpc(S0);
	Isfile(S2);
	Isopen(S2);
	S2 = fread((char *)S0, 1, ((int)S1), FileD(S2));
	Pop2;
}

/*
   ( len buf stream -- bytes-written )
   Writes len bytes from buf to stream, returning the actual number of bytes
   written.
*/
prim P_fwrite()
{
	Sl(3);
	Hpc(S1);
	Isfile(S0);
	Isopen(S0);
	S2 = fwrite((char *)S1, 1, ((int)S2), FileD(S0));
	Pop2;
}

prim P_fgetc()
{				/* File get character: fd -- char */
	Sl(1);
	Isfile(S0);
	Isopen(S0);
	S0 = getc(FileD(S0));	/* Returns -1 if EOF hit */
}

prim P_fputc()
{				/* File put character: char fd -- stat */
	Sl(2);
	Isfile(S0);
	Isopen(S0);
	S1 = putc((char)S1, FileD(S0));
	Pop;
}

prim P_ftell()
{				/* Return file position:  fd -- pos */
	Sl(1);
	Isfile(S0);
	Isopen(S0);
	S0 = (stackitem)ftell(FileD(S0));
}

prim P_fseek()
{				/* Seek file:  offset base fd -- */
	Sl(3);
	Isfile(S0);
	Isopen(S0);
	fseek(FileD(S0), (long)S2, (int)S1);
	Npop(3);
}

prim P_fload()
{				/* Load source file:  fd -- evalstat */
	int estat;
	FILE *fd;

	Sl(1);
	Isfile(S0);
	Isopen(S0);
	fd = FileD(S0);
	Pop;
	estat = pez_load(fd);
	So(1);
	Push = estat;
}
#endif				/* FILEIO */

#ifdef EVALUATE

prim P_evaluate()
{				/* string -- status */
	int es = PEZ_SNORM;
	pez_statemark mk;
	pez_int scomm = pez_comment;	/* Stack comment pending state */
	dictword **sip = ip;	/* Stack instruction pointer */
	char *sinstr = instream;	/* Stack input stream */
	char *estring;

	Sl(1);
	Hpc(S0);
	estring = (char *)S0;	/* Get string to evaluate */
	Pop;			/* Pop so it sees arguments below it */
	pez_mark(&mk);		/* Mark in case of error */
	ip = NULL;		/* Fool pez_eval into interp state */
	if((es = pez_eval(estring)) != PEZ_SNORM) {
		pez_unwind(&mk);
	}
	/* If there were no other errors, check for a runaway comment.  If
	   we ended the file in comment-ignore mode, set the runaway comment
	   error status and unwind the file.  */
	if((es == PEZ_SNORM) && (pez_comment != 0)) {
		es = PEZ_RUNCOMM;
		pez_unwind(&mk);
	}
	pez_comment = scomm;	/* Unstack comment pending status */
	ip = sip;		/* Unstack instruction pointer */
	instream = sinstr;	/* Unstack input stream */
	So(1);
	Push = es;		/* Return eval status on top of stack */
}
#endif				/* EVALUATE */

/*  Stack mechanics  */

prim P_depth()
{				/* Push stack depth */
	stackitem s = stk - stack;

	So(1);
	Push = s;
}

prim P_clear()
{				/* Clear stack */
	stk = stack;
}

prim P_dup()
{				/* Duplicate top of stack */
	stackitem s;

	Sl(1);
	So(1);
	s = S0;
	Push = s;
}

prim P_drop()
{				/* Drop top item on stack */
	Sl(1);
	Pop;
}

prim P_swap()
{				/* Exchange two top items on stack */
	stackitem t;

	Sl(2);
	t = S1;
	S1 = S0;
	S0 = t;
}

prim P_over()
{				/* Push copy of next to top of stack */
	stackitem s;

	Sl(2);
	So(1);
	s = S1;
	Push = s;
}

prim P_pick()
{				/* Copy indexed item from stack */
	Sl(2);
	S0 = stk[-(2 + S0)];
}

/*
   ( a b -- b )
*/
prim P_nip()
{
	Sl(2);
	S1 = S0;
	Pop;
}

prim P_rot()
{				/* Rotate 3 top stack items */
	stackitem t;

	Sl(3);
	t = S0;
	S0 = S2;
	S2 = S1;
	S1 = t;
}

prim P_minusrot()
{				/* Reverse rotate 3 top stack items */
	stackitem t;

	Sl(3);
	t = S0;
	S0 = S1;
	S1 = S2;
	S2 = t;
}

prim P_roll()
{				/* Rotate N top stack items */
	stackitem i, j, t;

	Sl(1);
	i = S0;
	Pop;
	Sl(i + 1);
	t = stk[-(i + 1)];
	for(j = -(i + 1); j < -1; j++)
		stk[j] = stk[j + 1];
	S0 = t;
}

prim P_tor()
{				/* Transfer stack top to return stack */
	Rso(1);
	Sl(1);
	Rpush = (rstackitem)S0;
	Pop;
}

prim P_rfrom()
{				/* Transfer return stack top to stack */
	Rsl(1);
	So(1);
	Push = (stackitem)R0;
	Rpop;
}

prim P_rfetch()
{				/* Fetch top item from return stack */
	Rsl(1);
	So(1);
	Push = (stackitem)R0;
}

/*
   ( -- time )
   Returns time as number of seconds since the epoch.
*/
prim P_time()
{
	So(1);
	Push = time(NULL);
}

#ifdef Macintosh
/* This file creates more than 32K of object code on the Mac, which causes
   MPW to barf.  So, we split it up into two code segments of <32K at this
   point. */
#pragma segment TOOLONG
#endif				/* Macintosh */

/*  Double stack manipulation items  */

#ifdef DOUBLE

prim P_2dup()
{				/* Duplicate stack top doubleword */
	stackitem s;

	Sl(2);
	So(2);
	s = S1;
	Push = s;
	s = S1;
	Push = s;
}

prim P_2drop()
{				/* Drop top two items from stack */
	Sl(2);
	stk -= 2;
}

prim P_2swap()
{				/* Swap top two double items on stack */
	stackitem t;

	Sl(4);
	t = S2;
	S2 = S0;
	S0 = t;
	t = S3;
	S3 = S1;
	S1 = t;
}

prim P_2over()
{				/* Extract second pair from stack */
	stackitem s;

	Sl(4);
	So(2);
	s = S3;
	Push = s;
	s = S3;
	Push = s;
}

prim P_2nip()
{
	Sl(4);
	S2 = S0;
	S3 = S1;
	Pop2;
}

prim P_2rot()
{				/* Move third pair to top of stack */
	stackitem t1, t2;

	Sl(6);
	t2 = S5;
	t1 = S4;
	S5 = S3;
	S4 = S2;
	S3 = S1;
	S2 = S0;
	S1 = t2;
	S0 = t1;
}

prim P_2variable()
{				/* Declare double variable */
	P_create();		/* Create dictionary item */
	Ho(2);
	Hstore = 0;		/* Initial value = 0... */
	Hstore = 0;		/* ...in both words */
}

prim P_2con()
{				/* Push double value in body */
	So(2);
	Push = *(((stackitem *)curword) + Dictwordl);
	Push = *(((stackitem *)curword) + Dictwordl + 1);
}

prim P_2constant()
{				/* Declare double word constant */
	Sl(1);
	P_create();		/* Create dictionary item */
	createword->wcode = P_2con;	/* Set code to constant push */
	Ho(2);
	Hstore = S1;		/* Store double word constant value */
	Hstore = S0;		/* in the two words of body */
	Pop2;
}

prim P_2bang()
{				/* Store double value into address */
	stackitem *sp;

	Sl(2);
	Hpc(S0);
	sp = (stackitem *)S0;
	*sp++ = S2;
	*sp = S1;
	Npop(3);
}

prim P_2at()
{				/* Fetch double value from address */
	stackitem *sp;

	Sl(1);
	So(1);
	Hpc(S0);
	sp = (stackitem *)S0;
	S0 = *sp++;
	Push = *sp;
}

prim P_fover()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_over();
		break;
	case 2:
		P_2over();
		break;
	}
}

prim P_fdrop()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_drop();
		break;
	case 2:
		P_2drop();
		break;
	}
}

prim P_fdup()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_dup();
		break;
	case 2:
		P_2dup();
		break;
	}
}

prim P_fswap()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_swap();
		break;
	case 2:
		P_2swap();
		break;
	}
}

prim P_frot()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_rot();
		break;
	case 2:
		P_2rot();
		break;
	}
}

prim P_fvariable()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_variable();
		break;
	case 2:
		P_2variable();
		break;
	}
}

prim P_fconstant()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_constant();
		break;
	case 2:
		P_2constant();
		break;
	}
}

prim P_fbang()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_bang();
		break;
	case 2:
		P_2bang();
		break;
	}
}

prim P_fat()
{
	switch (sizeof(double) / sizeof(long)) {
	case 1:
		P_at();
		break;
	case 2:
		P_2at();
		break;
	}
}
#endif				/* DOUBLE */

/*  Data transfer primitives  */

prim P_dolit()
{				/* Push instruction stream literal */
	So(1);
	trace {
		printf("%ld ", (long)*ip);
	}
	Push = (stackitem) * ip++;	/* Push the next datum from the
					   instruction stream. */
}

/*  Control flow primitives  */

prim P_nest()
{				/* Invoke compiled word */
	if(tail_call_pending) {
		tail_call_pending = False;
	} else {
		Rso(1);
#ifdef WALKBACK
		*wbptr++ = curword;	/* Place word on walkback stack */
#endif
		Rpush = ip;
	}
	ip = (((dictword **)curword) + Dictwordl);
}

prim P_exit()
{				/* Return to top of return stack */
	Rsl(1);
#ifdef WALKBACK
	wbptr = (wbptr > wback) ? wbptr - 1 : wback;
#endif
	ip = R0;		/* Set IP to top of return stack */
	Rpop;
}

prim P_branch()
{				/* Jump to in-line address */
	ip += (stackitem) * ip;	/* Jump addresses are IP-relative */
}

prim P_qbranch()
{				/* Conditional branch to in-line addr */
	Sl(1);
	if(S0 == 0)		/* If flag is false */
		ip += (stackitem) * ip;	/* then branch. */
	else			/* Otherwise */
		ip++;		/* skip the in-line address. */
	Pop;
}

prim P_if()
{				/* Compile IF word */
	Compiling;
	Compconst(s_qbranch);	/* Compile question branch */
	So(1);
	Push = (stackitem)hptr;	/* Save backpatch address on stack */
	Compconst(0);		/* Compile place-holder address cell */
}

prim P_else()
{				/* Compile ELSE word */
	stackitem *bp;

	Compiling;
	Sl(1);
	Compconst(s_branch);	/* Compile branch around other clause */
	Compconst(0);		/* Compile place-holder address cell */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get IF backpatch address */
	*bp = hptr - bp;
	S0 = (stackitem)(hptr - 1);	/* Update backpatch for THEN */
}

prim P_then()
{				/* Compile THEN word */
	stackitem *bp;

	Compiling;
	Sl(1);
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get IF/ELSE backpatch address */
	*bp = hptr - bp;
	Pop;
}

prim P_qdup()
{				/* Duplicate if nonzero */
	Sl(1);
	if(S0 != 0) {
		stackitem s = S0;
		So(1);
		Push = s;
	}
}

prim P_begin()
{				/* Compile BEGIN */
	Compiling;
	So(1);
	Push = (stackitem)hptr;	/* Save jump back address on stack */
}

prim P_until()
{				/* Compile UNTIL */
	stackitem off;
	stackitem *bp;

	Compiling;
	Sl(1);
	Compconst(s_qbranch);	/* Compile question branch */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get BEGIN address */
	off = -(hptr - bp);
	Compconst(off);		/* Compile negative jumpback address */
	Pop;
}

prim P_again()
{				/* Compile AGAIN */
	stackitem off;
	stackitem *bp;

	Compiling;
	Compconst(s_branch);	/* Compile unconditional branch */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get BEGIN address */
	off = -(hptr - bp);
	Compconst(off);		/* Compile negative jumpback address */
	Pop;
}

prim P_while()
{				/* Compile WHILE */
	Compiling;
	So(1);
	Compconst(s_qbranch);	/* Compile question branch */
	Compconst(0);		/* Compile place-holder address cell */
	Push = (stackitem)(hptr - 1);	/* Queue backpatch for REPEAT */
}

prim P_repeat()
{				/* Compile REPEAT */
	stackitem off;
	stackitem *bp1, *bp;

	Compiling;
	Sl(2);
	Hpc(S0);
	bp1 = (stackitem *)S0;	/* Get WHILE backpatch address */
	Pop;
	Compconst(s_branch);	/* Compile unconditional branch */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get BEGIN address */
	off = -(hptr - bp);
	Compconst(off);		/* Compile negative jumpback address */
	*bp1 = hptr - bp1;	/* Backpatch REPEAT's jump out of loop */
	Pop;
}

prim P_do()
{				/* Compile DO */
	Compiling;
	Compconst(s_xdo);	/* Compile runtime DO word */
	So(1);
	Compconst(0);		/* Reserve cell for LEAVE-taking */
	Push = (stackitem)hptr;	/* Save jump back address on stack */
}

prim P_xdo()
{				/* Execute DO */
	Sl(2);
	Rso(3);
	Rpush = ip + ((stackitem) * ip);	// Push exit address from loop
	ip++;			/* Increment past exit address word */
	Rpush = (rstackitem)S1;	/* Push loop limit on return stack */
	Rpush = (rstackitem)S0;	/* Iteration variable initial value to
					   return stack */
	stk -= 2;
}

prim P_qdo()
{				/* Compile ?DO */
	Compiling;
	Compconst(s_xqdo);	/* Compile runtime ?DO word */
	So(1);
	Compconst(0);		/* Reserve cell for LEAVE-taking */
	Push = (stackitem)hptr;	/* Save jump back address on stack */
}

prim P_xqdo()
{				/* Execute ?DO */
	Sl(2);
	if(S0 == S1) {
		ip += (stackitem) * ip;
	} else {
		Rso(3);
		Rpush = ip + ((stackitem) * ip);	/* Push exit address from loop */
		ip++;		/* Increment past exit address word */
		Rpush = (rstackitem)S1;	/* Push loop limit on return stack */
		Rpush = (rstackitem)S0;	/* Iteration variable initial value to
						   return stack */
	}
	stk -= 2;
}

prim P_loop()
{				/* Compile LOOP */
	stackitem off;
	stackitem *bp;

	Compiling;
	Sl(1);
	Compconst(s_xloop);	/* Compile runtime loop */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get DO address */
	off = -(hptr - bp);
	Compconst(off);		/* Compile negative jumpback address */
	*(bp - 1) = (hptr - bp) + 1;	/* Backpatch exit address offset */
	Pop;
}

prim P_ploop()
{				/* Compile +LOOP */
	stackitem off;
	stackitem *bp;

	Compiling;
	Sl(1);
	Compconst(s_pxloop);	/* Compile runtime +loop */
	Hpc(S0);
	bp = (stackitem *)S0;	/* Get DO address */
	off = -(hptr - bp);
	Compconst(off);		/* Compile negative jumpback address */
	*(bp - 1) = (hptr - bp) + 1;	/* Backpatch exit address offset */
	Pop;
}

prim P_xloop()
{				/* Execute LOOP */
	Rsl(3);
	R0 = (rstackitem)(((stackitem) R0) + 1);
	if(((stackitem)R0) == ((stackitem)R1)) {
		rstk -= 3;	/* Pop iteration variable and limit */
		ip++;		/* Skip the jump address */
	} else {
		ip += (stackitem) * ip;
	}
}

prim P_xploop()
{				/* Execute +LOOP */
	stackitem niter;

	Sl(1);
	Rsl(3);

	niter = ((stackitem)R0) + S0;
	if(niter == (stackitem)R1
	   || abs(S0) > abs((stackitem)R0 - (stackitem)R1)) {
		rstk -= 3;	/* Pop iteration variable and limit */
		ip++;		/* Skip the jump address */
	} else {
		ip += (stackitem) * ip;
		R0 = (rstackitem)niter;
	}
	Pop;
}

prim P_leave()
{				/* Compile LEAVE */
	Rsl(3);
	ip = R2;
	rstk -= 3;
}

prim P_i()
{				/* Obtain innermost loop index */
	Rsl(3);
	So(1);
	Push = (stackitem)R0;	/* It's the top item on return stack */
}

prim P_j()
{				/* Obtain next-innermost loop index */
	Rsl(6);
	So(1);
	Push = (stackitem)rstk[-4];	/* It's the 4th item on return stack */
}

prim P_quit()
{				/* Terminate execution */
	rstk = rstack;		/* Clear return stack */
#ifdef WALKBACK
	wbptr = wback;
#endif
	ip = NULL;		/* Stop execution of current word */
}

prim P_abort()
{				/* Abort, clearing data stack */
	P_clear();		/* Clear the data stack */
	P_quit();		/* Shut down execution */
}

prim P_abortq()
{				/* Abort, printing message */
	if(state) {
		stringlit = True;	/* Set string literal expected */
		Compconst(s_abortq);	/* Compile ourselves */
	} else {
		printf("%s", (char *)ip);	/* Otherwise, print string literal
						   in in-line code. */
#ifdef WALKBACK
		pwalkback();
#endif				/* WALKBACK */
		P_abort();	/* Abort */
		pez_comment = state = Falsity;	/* Reset all interpretation state */
		forgetpend = defpend = stringlit = tickpend = ctickpend = False;
	}
}

/*  Compilation primitives  */

prim P_immediate()
{				/* Mark most recent word immediate */
	dict->wname[0] |= IMMEDIATE;
}

prim P_lbrack()
{				/* Set interpret state */
	Compiling;
	state = Falsity;
}

prim P_rbrack()
{				/* Restore compile state */
	state = Truth;
}

Exported void P_dodoes()
{				/* Execute indirect call on method */
	Rso(1);
	So(1);
	Rpush = ip;		/* Push instruction pointer */
#ifdef WALKBACK
	*wbptr++ = curword;	/* Place word on walkback stack */
#endif
	/* The compiler having craftily squirreled away the DOES> clause
	   address before the word definition on the heap, we back up to
	   the heap cell before the current word and load the pointer from
	   there.  This is an ABSOLUTE heap address, not a relative offset. */
	ip = *((dictword ***)(((stackitem *)curword) - 1));

	/* Push the address of this word's body as the argument to the
	   DOES> clause. */
	Push = (stackitem)(((stackitem *)curword) + Dictwordl);
}

prim P_does()
{				/* Specify method for word */

	/* O.K., we were compiling our way through this definition and we've
	   encountered the Dreaded and Dastardly Does.  Here's what we do
	   about it.  The problem is that when we execute the word, we
	   want to push its address on the stack and call the code for the
	   DOES> clause by diverting the IP to that address.  But...how
	   are we to know where the DOES> clause goes without adding a
	   field to every word in the system just to remember it.  Recall
	   that since this system is portable we can't cop-out through
	   machine code.  Further, we can't compile something into the
	   word because the defining code may have already allocated heap
	   for the word's body.  Yukkkk.  Oh well, how about this?  Let's
	   copy any and all heap allocated for the word down one stackitem
	   and then jam the DOES> code address BEFORE the link field in
	   the word we're defining.

	   Then, when (DOES>) (P_dodoes) is called to execute the word, it
	   will fetch that code address by backing up past the start of
	   the word and seting IP to it.  Note that FORGET must recognise
	   such words (by the presence of the pointer to P_dodoes() in
	   their wcode field, in case you're wondering), and make sure to
	   deallocate the heap word containing the link when a
	   DOES>-defined word is deleted.  */

	if(createword != NULL) {
		stackitem *sp = ((stackitem *)createword), *hp;

		Rsl(1);
		Ho(1);

		/* Copy the word definition one word down in the heap to
		   permit us to prefix it with the DOES clause address. */

		for(hp = hptr - 1; hp >= sp; hp--)
			*(hp + 1) = *hp;
		hptr++;		/* Expand allocated length of word */
		*sp++ = (stackitem)ip;	/* Store DOES> clause address before
					   word's definition structure. */
		createword = (dictword *)sp;	/* Move word definition down 1 item */
		createword->wcode = P_dodoes;	/* Set code field to indirect jump */

		/* Now simulate an EXIT to bail out of the definition without
		   executing the DOES> clause at definition time. */

		ip = R0;	/* Set IP to top of return stack */
#ifdef WALKBACK
		wbptr = (wbptr > wback) ? wbptr - 1 : wback;
#endif
		Rpop;		/* Pop the return stack */
	}
}

prim P_colon()
{				/* Begin compilation */
	state = Truth;		/* Set compilation underway */
	P_create();		/* Create conventional word */
}

prim P_semicolon()
{				/* End compilation */
	Compiling;
	Ho(1);
	Hstore = s_exit;
	state = Falsity;	/* No longer compiling */
	/* We wait until now to plug the P_nest code so that it will be
	   present only in completed definitions. */
	if(createword != NULL)
		createword->wcode = P_nest;	/* Use P_nest for code */
	createword = NULL;	/* Flag no word being created */
}

prim P_tick()
{				/* Take address of next word */
	int i;

	/* Try to get next symbol from the input stream.  If
	   we can't, and we're executing a compiled word,
	   report an error.  Since we can't call back to the
	   calling program for more input, we're stuck. */

	i = token(&instream);	/* Scan for next token */
	if(i != TokNull) {
		if(i == TokWord) {
			dictword *di;

			ucase(tokbuf);
			if((di = lookup(tokbuf)) != NULL) {
				So(1);
				Push = (stackitem)di;	/* Push word compile address */
			} else {
				printf(" '%s' undefined ", tokbuf);
			}
		} else {
			printf("\nWord not specified when expected.\n");
			P_abort();
		}
	} else {
		/* O.K., there was nothing in the input stream.  Set the
		   tickpend flag to cause the compilation address of the next
		   token to be pushed when it's supplied on a subsequent input
		   line. */
		if(ip == NULL) {
			tickpend = True;	/* Set tick pending */
		} else {
			printf
				("\nWord requested by ` not on same input line.\n");
			P_abort();
		}
	}
}

prim P_bracktick()
{				/* Compile in-line code address */
	Compiling;
	ctickpend = True;	/* Force literal treatment of next
				   word in compile stream */
}

prim P_execute()
{				/* Execute word pointed to by stack */
	dictword *wp;

	Sl(1);
	wp = (dictword *)S0;	/* Load word address from stack */
	Pop;			/* Pop data stack before execution */
	exword(wp);		/* Recursively call exword() to run
				   the word. */
}

prim P_tail_call()
{
	tail_call_pending = True;
}

prim P_body()
{				/* Get body address for word */
	Sl(1);
	S0 += Dictwordl * sizeof(stackitem);
}

prim P_state()
{				/* Get state of system */
	So(1);
	Push = (stackitem) & state;
}

/*  Definition field access primitives	*/

#ifdef DEFFIELDS

prim P_find()
{				/* Look up word in dictionary */
	dictword *dw;

	Sl(1);
	So(1);
	Hpc(S0);
	strcpy(tokbuf, (char *)S0);	/* Use built-in token buffer... */
	dw = lookup(tokbuf);	/* So ucase() in lookup() doesn't wipe */
	/* the token on the stack */
	if(dw != NULL) {
		S0 = (stackitem)dw;
		/* Push immediate flag */
		Push = (dw->wname[0] & IMMEDIATE) ? 1 : -1;
	} else {
		Push = 0;
	}
}

#define DfOff(fld)  (((char *)&(dict->fld)) - ((char *)dict))

prim P_toname()
{				/* Find name field from compile addr */
	Sl(1);
	S0 += DfOff(wname);
}

/*
   Find link field from compile addr 
 */
prim P_tolink()
{
	if(DfOff(wnext) != 0)
		printf("\n>LINK Foulup--wnext is not at zero!\n");
	// Null operation.  Wnext is first.
	// Sl(1);
	// SO += DfOff(wnext)
}

prim P_frombody()
{				/* Get compile address from body */
	Sl(1);
	S0 -= Dictwordl * sizeof(stackitem);
}

prim P_fromname()
{				/* Get compile address from name */
	Sl(1);
	S0 -= DfOff(wname);
}

prim P_fromlink()
{				/* Get compile address from link */
	if(DfOff(wnext) != 0)
		printf("\nLINK> Foulup--wnext is not at zero!\n");
/*  Sl(1);
					S0 -= DfOff(wnext);  *//* Null operation.  Wnext is first */
}

#undef DfOff

#define DfTran(from,to) (((char *)&(dict->to)) - ((char *)&(dict->from)))

prim P_nametolink()
{				/* Get from name field to link */
	char *from, *to;

	Sl(1);
	/*
	   S0 -= DfTran(wnext, wname);
	 */
	from = (char *)&(dict->wnext);
	to = (char *)&(dict->wname);
	S0 -= (to - from);
}

prim P_linktoname()
{				/* Get from link field to name */
	char *from, *to;

	Sl(1);
	/*
	   S0 += DfTran(wnext, wname);
	 */
	from = (char *)&(dict->wnext);
	to = (char *)&(dict->wname);
	S0 += (to - from);
}

prim P_fetchname()
{				/* Copy word name to string buffer */
	Sl(2);			/* nfa string -- */
	Hpc(S0);
	Hpc(S1);
	/* Since the name buffers aren't in the system heap, but
	   rather are separately allocated with alloc(), we can't
	   check the name pointer references.  But, hey, if the user's
	   futzing with word dictionary items on the heap in the first
	   place, there's a billion other ways to bring us down at
	   his command. */
	strcpy((char *)S0, *((char **)S1) + 1);
	Pop2;
}

prim P_storename()
{				/* Store string buffer in word name */
	char tflags;
	char *cp;

	Sl(2);			/* string nfa -- */
	Hpc(S0);		/* See comments in P_fetchname above */
	Hpc(S1);		/* checking name pointers */
	tflags = **((char **)S0);
	free(*((char **)S0));
	*((char **)S0) = cp = alloc((unsigned int)(strlen((char *)S1) + 2));
	strcpy(cp + 1, (char *)S1);
	*cp = tflags;
	Pop2;
}

#endif				/* DEFFIELDS */

#ifdef SYSTEM
prim P_system()
{				/* string -- status */
	Sl(1);
	Hpc(S0);
	S0 = system((char *)S0);
}
#endif				/* SYSTEM */

#ifdef FFI
#include <dlfcn.h>

/*
   ( libname -- status )
   Loads a .so file that must contain at least one of the following in order
   to be useful:
   	1.  A function declared as prim pez_ffi_init() that performs the
		necessary initializations
	2.  An array named pez_ffi_definitions with type struct primfcn that
		contains the definitions of new words to be added when the library
		is loaded.
   If the status is false, there has been an error, which can be checked with
   dlerror.
*/
prim P_ffi_load()
{
	void *lib;
	void (*init)();
	struct primfcn *defs;

	Sl(1);
	lib = dlopen((char *)S0, RTLD_NOW);
	if(!lib) {
		S0 = Falsity;
		return;
	}

	init = dlsym(lib, "pez_ffi_init");
	defs = dlsym(lib, "pez_ffi_definitions");
	if(!(init || defs)) {
		S0 = Falsity;
		return;
	}
	S0 = Truth;
	dlerror();		// Which clears the last error.
	if(init)
		init();
	if(defs)
		pez_primdef(defs);
}

/* Just a thin wrapper around the system's dlopen. 
   ( libname flags -- libhandle )
*/
prim P_dlopen()
{
	void *lib;
	Sl(2);
	lib = dlopen((char *)S1, S0);
	S1 = (long)lib;
	Pop;
}

/*
   Pushes the RTLD_LAZY flag onto the stack.
   ( -- RTLD_LAZY )
*/
prim P_rtld_lazy()
{
	So(1);
	Push = RTLD_LAZY;
}

/*
   Resolves a symbol.
   ( libhandle symname -- function_address )
*/
prim P_dlsym()
{
	void *f;
	Sl(2);
	f = dlsym((void *)S1, (char *)S0);
	Pop;
	S0 = (long)f;
}

/*
   Returns the last error returned by dlopen/dlsym/dlclose.
   ( -- error_string )
*/
prim P_dlerror()
{
	So(1);
	Push = (long)dlerror();
}

/* 
   Calls a void (*)() from the top of the stack.
*/
prim P_call_void_0()
{
	void (*f)() = (void (*)()) S0;
	So(1);
	Pop;
	f();
}

/*
   Calls a void (*)(word) from the top of the stack.
*/
prim P_call_void_1w()
{
	void (*f)(stackitem) = (void (*)(stackitem))S0;
	So(2);
	f(S1);
	Pop2;
}

/*
   Calls a word (*)() from the top of the stack.
*/
prim P_call_word_0()
{
	stackitem(*f)() = (stackitem(*)())S0;
	So(1);
	S0 = f();
}

/*
   Calls a word (*)(word) from the top of the stack.
*/
prim P_call_word_1w()
{
	stackitem(*f)(stackitem) = (stackitem(*)(stackitem))S0;
	So(2);
	Pop;
	S0 = f(S0);
}

#endif				// FFI

#ifdef PROCESS
#include <sys/wait.h>
#include <signal.h>

extern char **environ;
/*
   ( -- environment )
   Pushes the environ pointer onto the stack.  You probably actually want to
   interact with getenv/setenv unless you're iterating over the environment.
*/
prim P_environ()
{
	So(1);
	Push = (stackitem)environ;
}

/*
   ( varname -- envvar|0 )
   Returns the value for the named variable, or NULL if it isn't set.
*/
prim P_getenv()
{
	Sl(1);
	S0 = (stackitem)getenv((char *)S0);
}

/*
   ( varname value -- success=Truth|error=falsity )
   Sets an environment variable.
*/
prim P_setenv()
{
	Sl(2);
	S1 = -setenv((char *)S1, (char *)S0, 1) - 1;
	Pop;
}

/*
   ( varname -- success=Truth|error=Falsity )
   Removes a variable from the environment.
*/
prim P_unsetenv()
{
	Sl(1);
	S0 = -unsetenv((char *)S0) - 1;
}

/*
   ( message status -- )
   Prints a message to stderr with a \n, and dies with the specified status.
*/
prim P_diebang()
{
	fflush(stdout);
	// No underflow-checking here.  die! shouldn't fail, ever, even if it
	// ends up segfaulting instead.
	fprintf(stderr, "%s\n", (char *)S1);
	exit(S0);
}

/*
   ( -- pid|0 )
   Forks a new process.  The child process will see a zero on the stack, and the
   parent will see the child's pid.
*/
prim P_fork()
{
	So(1);
	Push = fork();
}

/*
   ( path argv -- )
   exec()s another binary.  Does not return unless there's an error.  Path
   should be the path to the other executable, and argv should be an array of
   strings, with a NULL after the last string.
*/
prim P_execv()
{
	Sl(2);
	execv((char *)S1, (char **)S0);
}

/*
   ( -- status pid )
   Waits for the next child process to exit, returning a pid and the process's
   status.
*/
prim P_wait()
{
	So(2);
	Push = 0;
	Push = wait((int *)(stk - 1));
}

/*
   ( -- pid )
   Returns the pid of the current process.
*/
prim P_pid()
{
	So(1);
	Push = getpid();
}

/*
   ( pid signal -- status )
   Sends a signal to a pid, returning the status.
*/
prim P_kill()
{
	Sl(2);
	S1 = kill(S1, S0);
	Pop;
}


#endif				// PROCESS

#ifdef TRACE
prim P_trace()
{				/* Set or clear tracing of execution */
	Sl(1);
	pez_trace = (S0 == 0) ? Falsity : Truth;
	Pop;
}
#endif				/* TRACE */

#ifdef WALKBACK
prim P_walkback()
{				/* Set or clear error walkback */
	Sl(1);
	pez_walkback = (S0 == 0) ? Falsity : Truth;
	Pop;
}
#endif				/* WALKBACK */

#ifdef WORDSUSED

prim P_wordsused()
{				/* List words used by program */
	dictword *dw = dict;

	while(dw != NULL) {
		if(*(dw->wname) & WORDUSED) {
			printf("\n%s", dw->wname + 1);
		}
#ifdef Keyhit
		if(kbquit()) {
			break;
		}
#endif
		dw = dw->wnext;
	}
	printf("\n");
}

prim P_wordsunused()
{				/* List words not used by program */
	dictword *dw = dict;

	while(dw != NULL) {
		if(!(*(dw->wname) & WORDUSED)) {
			printf("\n%s", dw->wname + 1);
		}
#ifdef Keyhit
		if(kbquit()) {
			break;
		}
#endif
		dw = dw->wnext;
	}
	printf("\n");
}
#endif				/* WORDSUSED */

#ifdef COMPILERW

prim P_brackcompile()
{				/* Force compilation of immediate word */
	Compiling;
	cbrackpend = True;	/* Set [COMPILE] pending */
}

prim P_literal()
{				/* Compile top of stack as literal */
	Compiling;
	Sl(1);
	Ho(2);
	Hstore = s_lit;		/* Compile load literal word */
	Hstore = S0;		/* Compile top of stack in line */
	Pop;
}

/*
	Compile address of next inline word
*/
prim P_compile()
{
	Compiling;
	Ho(1);
	Hstore = (stackitem) * ip++;	// Compile next datum from instruction stream.
}

/*
	Mark backward backpatch address
*/
prim P_backmark()
{
	Compiling;
	So(1);
	Push = (stackitem)hptr;	// Push heap address onto stack 
}

/*
	Emit backward jump offset
*/
prim P_backresolve()
{
	stackitem offset;

	Compiling;
	Sl(1);
	Ho(1);
	Hpc(S0);
	offset = -(hptr - (stackitem *)S0);
	Hstore = offset;
	Pop;
}

prim P_fwdmark()
{				/* Mark forward backpatch address */
	Compiling;
	Ho(1);
	Push = (stackitem)hptr;	/* Push heap address onto stack */
	Hstore = 0;
}

/*
   Emit forward jump offset
*/
prim P_fwdresolve()
{
	stackitem offset;

	Compiling;
	Sl(1);
	Hpc(S0);
	offset = (hptr - (stackitem *)S0);
	*((stackitem *)S0) = offset;
	Pop;
}

#endif				/* COMPILERW */

/*  Table of primitive words  */

static struct primfcn primt[] = {
	{"0+", P_plus},
	{"0-", P_minus},
	{"0*", P_times},
	{"0/", P_div},
	{"0MOD", P_mod},
	{"0/MOD", P_divmod},
	{"0MIN", P_min},
	{"0MAX", P_max},
	{"0NEGATE", P_neg},
	{"0ABS", P_abs},
	{"0=", P_equal},
	{"0<>", P_unequal},
	{"0>", P_gtr},
	{"0<", P_lss},
	{"0>=", P_geq},
	{"0<=", P_leq},

	{"0AND", P_and},
	{"0OR", P_or},
	{"0XOR", P_xor},
	{"0NOT", P_not},
	{"0SHIFT", P_shift},

	{"0DEPTH", P_depth},
	{"0CLEAR", P_clear},
	{"0DUP", P_dup},
	{"0DROP", P_drop},
	{"0SWAP", P_swap},
	{"0OVER", P_over},
	{"0PICK", P_pick},
	{"0NIP", P_nip},
	{"0ROT", P_rot},
	{"0-ROT", P_minusrot},
	{"0ROLL", P_roll},
	{"0>R", P_tor},
	{"0R>", P_rfrom},
	{"0R@", P_rfetch},
	{"0TIME", P_time},

#ifdef SHORTCUTA
	{"01+", P_1plus},
	{"02+", P_2plus},
	{"01-", P_1minus},
	{"02-", P_2minus},
	{"02*", P_2times},
	{"02/", P_2div},
#endif				/* SHORTCUTA */

#ifdef SHORTCUTC
	{"00=", P_0equal},
	{"00<>", P_0notequal},
	{"00>", P_0gtr},
	{"00<", P_0lss},
#endif				/* SHORTCUTC */

#ifdef DOUBLE
	{"02DUP", P_2dup},
	{"02DROP", P_2drop},
	{"02SWAP", P_2swap},
	{"02OVER", P_2over},
	{"02NIP", P_2nip},
	{"02ROT", P_2rot},
	{"02VARIABLE", P_2variable},
	{"02CONSTANT", P_2constant},
	{"02!", P_2bang},
	{"02@", P_2at},

	{"0FDUP", P_fdup},
	{"0FDROP", P_fdrop},
	{"0FSWAP", P_fswap},
	{"0FOVER", P_fover},
	{"0FROT", P_frot},
	{"0FVARIABLE", P_fvariable},
	{"0FCONSTANT", P_fconstant},
	{"0F!", P_fbang},
	{"0F@", P_fat},
#endif				/* DOUBLE */

	{"0VARIABLE", P_variable},
	{"0CONSTANT", P_constant},
	{"0!", P_bang},
	{"0@", P_at},
	{"0+!", P_plusbang},
	{"01+!", P_1plusbang},
	{"0ALLOT", P_allot},
	{"0,", P_comma},
	{"0C!", P_cbang},
	{"0C@", P_cat},
	{"0C,", P_ccomma},
	{"0C=", P_cequal},
	{"0HERE", P_here},

	{"0CELLSIZE", P_cellsize},
	{"0FLOATSIZE", P_floatsize},

#ifdef ARRAY
	{"0ARRAY", P_array},
#endif

#ifdef STRING
	{"0(STRLIT)", P_strlit},
	{"0STRING", P_string},
	{"0STRCPY", P_strcpy},
	{"0S!", P_strcpy},
	{"0STRCAT", P_strcat},
	{"0S+", P_strcat},
	{"0STRLEN", P_strlen},
	{"0STRCMP", P_strcmp},
	{"0STRCHAR", P_strchar},
	{"0SUBSTR", P_substr},
	{"0COMPARE", P_strcmp},
	{"0STRFORM", P_strform},
#ifdef REAL
	{"0FSTRFORM", P_fstrform},
#endif
	{"0STRINT", P_strint},
	{"0STRREAL", P_strreal},
#endif				/* STRING */

#ifdef REAL
	{"0(FLIT)", P_flit},
	{"0F+", P_fplus},
	{"0F-", P_fminus},
	{"0F*", P_ftimes},
	{"0F/", P_fdiv},
	{"0FMIN", P_fmin},
	{"0FMAX", P_fmax},
	{"0FNEGATE", P_fneg},
	{"0FABS", P_fabs},
	{"0F=", P_fequal},
	{"0F<>", P_funequal},
	{"0F>", P_fgtr},
	{"0F<", P_flss},
	{"0F>=", P_fgeq},
	{"0F<=", P_fleq},
	{"0F.", P_fdot},
	{"0FLOAT", P_float},
	{"0FIX", P_fix},
	{"0FTIME", P_ftime},

#ifdef MATH
	{"0ACOS", P_acos},
	{"0ASIN", P_asin},
	{"0ATAN", P_atan},
	{"0ATAN2", P_atan2},
	{"0COS", P_cos},
	{"0EXP", P_exp},
	{"0LOG", P_log},
	{"0POW", P_pow},
	{"0SIN", P_sin},
	{"0SQRT", P_sqrt},
	{"0TAN", P_tan},
#endif				/* MATH */
#endif				/* REAL */

	{"0(NEST)", P_nest},
	{"0EXIT", P_exit},
	{"0(LIT)", P_dolit},
	{"0BRANCH", P_branch},
	{"0?BRANCH", P_qbranch},
	{"1IF", P_if},
	{"1ELSE", P_else},
	{"1THEN", P_then},
	{"0?DUP", P_qdup},
	{"1BEGIN", P_begin},
	{"1UNTIL", P_until},
	{"1AGAIN", P_again},
	{"1WHILE", P_while},
	{"1REPEAT", P_repeat},
	{"1DO", P_do},
	{"1?DO", P_qdo},
	{"1LOOP", P_loop},
	{"1+LOOP", P_ploop},
	{"0(XDO)", P_xdo},
	{"0(X?DO)", P_xqdo},
	{"0(XLOOP)", P_xloop},
	{"0(+XLOOP)", P_xploop},
	{"0LEAVE", P_leave},
	{"0I", P_i},
	{"0J", P_j},
	{"0QUIT", P_quit},
	{"0ABORT", P_abort},
	{"1ABORT\"", P_abortq},

#ifdef SYSTEM
	{"0SYSTEM", P_system},
#endif
#ifdef FFI
	{"0FFI-LOAD", P_ffi_load},
	{"0DLOPEN", P_dlopen},
	{"0DLSYM", P_dlsym},
	{"0DLERROR", P_dlerror},

	// dl* flags:
	{"0RTLD_LAZY", P_rtld_lazy},

	// Generic calls:
	{"0CALL-VOID-0", P_call_void_0},
	{"0CALL-VOID-1W", P_call_void_1w},
	{"0CALL-WORD-0", P_call_word_0},
	{"0CALL-WORD-1W", P_call_word_1w},
#endif

#ifdef PROCESS
	{"0ENVIRON", P_environ},
	{"0GETENV", P_getenv},
	{"0SETENV", P_setenv},
	{"0UNSETENV", P_unsetenv},
	{"0DIE!", P_diebang},
	// at-exit is going to have to wait until I figure out a good way to do
	// it.  I am thinking a queue of words to push to 
	// {"0AT-EXIT", P_at_exit},
	{"0FORK", P_fork},
	{"0EXECV", P_execv},
	{"0WAIT", P_wait},
	{"0PID", P_pid},
	{"0KILL", P_kill},
#endif

#ifdef TRACE
	{"0TRACE", P_trace},
#endif
#ifdef WALKBACK
	{"0WALKBACK", P_walkback},
#endif

#ifdef WORDSUSED
	{"0WORDSUSED", P_wordsused},
	{"0WORDSUNUSED", P_wordsunused},
#endif

#ifdef MEMSTAT
	{"0MEMSTAT", pez_memstat},
#endif

	{"0:", P_colon},
	{"1;", P_semicolon},
	{"0IMMEDIATE", P_immediate},
	{"1[", P_lbrack},
	{"0]", P_rbrack},
	{"0CREATE", P_create},
	{"0FORGET", P_forget},
	{"0DOES>", P_does},
	{"0'", P_tick},
	{"1[']", P_bracktick},
	{"0EXECUTE", P_execute},
	{"0TAIL-CALL", P_tail_call},
	{"0>BODY", P_body},
	{"0STATE", P_state},

#ifdef DEFFIELDS
	{"0FIND", P_find},
	{"0>NAME", P_toname},
	{"0>LINK", P_tolink},
	{"0BODY>", P_frombody},
	{"0NAME>", P_fromname},
	{"0LINK>", P_fromlink},
	{"0N>LINK", P_nametolink},
	{"0L>NAME", P_linktoname},
	{"0NAME>S!", P_fetchname},
	{"0S>NAME!", P_storename},
#endif				/* DEFFIELDS */

#ifdef COMPILERW
	{"1[COMPILE]", P_brackcompile},
	{"1LITERAL", P_literal},
	{"0COMPILE", P_compile},
	{"0<MARK", P_backmark},
	{"0<RESOLVE", P_backresolve},
	{"0>MARK", P_fwdmark},
	{"0>RESOLVE", P_fwdresolve},
#endif				/* COMPILERW */

#ifdef CONIO
	{"0HEX", P_hex},
	{"0DECIMAL", P_decimal},
	{"0ARGC", P_argc},
	{"0ARGV", P_argv},
	{"0.", P_dot},
	{"0?", P_question},
	{"0CR", P_cr},
	{"0.S", P_dots},
	{"1.\"", P_dotquote},
	{"1.(", P_dotparen},
	{"0TYPE", P_type},
	{"0PUTS", P_puts},
	{"0WORDS", P_words},
#endif				/* CONIO */

#ifdef FILEIO
	{"0FILE", P_file},
	{"0FOPEN", P_fopen},
	{"0FCLOSE", P_fclose},
	{"0FDELETE", P_fdelete},
	{"0FGETS", P_fgetline},
	{"0FPUTS", P_fputline},
	{"0FREAD", P_fread},
	{"0FWRITE", P_fwrite},
	{"0FGETC", P_fgetc},
	{"0FPUTC", P_fputc},
	{"0FTELL", P_ftell},
	{"0FSEEK", P_fseek},
	{"0FLOAD", P_fload},
#endif				/* FILEIO */

#ifdef EVALUATE
	{"0EVALUATE", P_evaluate},
#endif				/* EVALUATE */

	{NULL, (codeptr)0}
};

/*  PEZ_PRIMDEF  --  Initialise the dictionary with the built-in primitive
			 words.  To save the memory overhead of separately
			 allocated word items, we get one buffer for all
			 the items and link them internally within the buffer. */

void pez_primdef(pt)
struct primfcn *pt;
{
	struct primfcn *pf = pt;
	dictword *nw;
	int i, n = 0;
#ifdef WORDSUSED
#ifdef READONLYSTRINGS
	unsigned int nltotal;
	char *dynames, *cp;
#endif				/* READONLYSTRINGS */
#endif				/* WORDSUSED */

	/* Count the number of definitions in the table. */

	while(pf->pname != NULL) {
		n++;
		pf++;
	}

#ifdef WORDSUSED
#ifdef READONLYSTRINGS
	nltotal = n;
	for(i = 0; i < n; i++) {
		nltotal += strlen(pt[i].pname);
	}
	cp = dynames = alloc(nltotal);
	for(i = 0; i < n; i++) {
		strcpy(cp, pt[i].pname);
		cp += strlen(cp) + 1;
	}
	cp = dynames;
#endif				/* READONLYSTRINGS */
#endif				/* WORDSUSED */

	nw = (dictword *)alloc((unsigned int)(n * sizeof(dictword)));

	nw[n - 1].wnext = dict;
	dict = nw;
	for(i = 0; i < n; i++) {
		nw->wname = pt->pname;
#ifdef WORDSUSED
#ifdef READONLYSTRINGS
		nw->wname = cp;
		cp += strlen(cp) + 1;
#endif				/* READONLYSTRINGS */
#endif				/* WORDSUSED */
		nw->wcode = pt->pcode;
		if(i != (n - 1)) {
			nw->wnext = nw + 1;
		}
		nw++;
		pt++;
	}
}

#ifdef WALKBACK

/*  PWALKBACK  --  Print walkback trace.  */

static void pwalkback()
{
	if(pez_walkback && ((curword != NULL) || (wbptr > wback))) {
		printf("Walkback:\n");
		if(curword != NULL) {
			printf("   %s\n", curword->wname + 1);
		}
		while(wbptr > wback) {
			dictword *wb = *(--wbptr);
			printf("   %s\n", wb->wname + 1);
		}
	}
}
#endif				/* WALKBACK */

/*  TROUBLE  --  Common handler for serious errors.  */

static void trouble(kind)
char *kind;
{
#ifdef MEMMESSAGE
	printf("\n%s.\n", kind);
#endif
#ifdef WALKBACK
	pwalkback();
#endif				/* WALKBACK */
	P_abort();
	pez_comment = state = Falsity;	/* Reset all interpretation state */
	forgetpend = defpend = stringlit = tickpend = ctickpend = False;
}

/*  PEZ_ERROR  --  Handle error detected by user-defined primitive.  */

Exported void pez_error(kind)
char *kind;
{
	trouble(kind);
	evalstat = PEZ_APPLICATION;	/* Signify application-detected error */
}

#ifdef BOUNDS_CHECK

/*  STAKOVER  --  Recover from stack overflow.	*/

Exported void stakover()
{
	trouble("Stack overflow");
	evalstat = PEZ_STACKOVER;
}

/*  STAKUNDER  --  Recover from stack underflow.  */

Exported void stakunder()
{
	trouble("Stack underflow");
	evalstat = PEZ_STACKUNDER;
}

/*  RSTAKOVER  --  Recover from return stack overflow.	*/

Exported void rstakover()
{
	trouble("Return stack overflow");
	evalstat = PEZ_RSTACKOVER;
}

/*  RSTAKUNDER	--  Recover from return stack underflow.  */

Exported void rstakunder()
{
	trouble("Return stack underflow");
	evalstat = PEZ_RSTACKUNDER;
}

/*  HEAPOVER  --  Recover from heap overflow.  Note that a heap
				  overflow does NOT wipe the heap; it's up to
		  the user to do this manually with FORGET or
		  some such. */

Exported void heapover()
{
	trouble("Heap overflow");
	evalstat = PEZ_HEAPOVER;
}

#endif				// BOUNDS_CHECK


#ifdef RESTRICTED_POINTERS

/*  BADPOINTER	--  Abort if pointer reference detected outside the heap.  */
Exported void badpointer()
{
	trouble("Bad pointer");
	evalstat = PEZ_BADPOINTER;
}

#else

Exported void badpointer() { }

#endif				// RESTRICTED_POINTERS

/*  NOTCOMP  --  Compiler word used outside definition.  */

#ifdef COMPILATION_SAFETY

static void notcomp()
{
	trouble("Compiler word outside definition");
	evalstat = PEZ_NOTINDEF;
}

#endif

#ifdef MATH_CHECK

/*  DIVZERO  --  Attempt to divide by zero.  */
static void divzero()
{
	trouble("Divide by zero");
	evalstat = PEZ_DIVZERO;
}

#endif

/*  EXWORD  --	Execute a word (and any sub-words it may invoke). */

static void exword(wp)
dictword *wp;
{
	curword = wp;
	trace {
		printf("\nTrace: %s ", curword->wname + 1);
	}
	(*curword->wcode)();	/* Execute the first word */
	while(ip != NULL) {
#ifdef BREAK
		Keybreak();	/* Poll for asynchronous interrupt */
		if(broken) {	/* Did we receive a break signal */
			trouble("Break signal");
			evalstat = PEZ_BREAK;
			break;
		}
#endif				/* BREAK */
		curword = *ip++;
		trace {
			printf("\nTrace: %s ", curword->wname + 1);
		}
		(*curword->wcode)();	/* Execute the next word */
	}
	curword = NULL;
}

/*  PEZ_INIT  --  Initialise the PEZ system.  The dynamic storage areas
		  are allocated unless the caller has preallocated buffers
		  for them and stored the addresses into the respective
		  pointers.  In either case, the storage management
		  pointers are initialised to the correct addresses.  If
				  the caller preallocates the buffers, it's up to him to
		  ensure that the length allocated agrees with the lengths
		  given by the pez_... cells.  */

void pez_init()
{
	if(dict == NULL) {
		pez_primdef(primt);	/* Define primitive words */
		dictprot = dict;	/* Set protected mark in dictionary */

		/* Look up compiler-referenced words in the new dictionary and
		   save their compile addresses in static variables. */

#define Cconst(cell, name)  cell = (stackitem)lookup(name); if(cell==0)abort()
		Cconst(s_exit, "EXIT");
		Cconst(s_lit, "(LIT)");
		Cconst(s_flit, "(FLIT)");
		Cconst(s_strlit, "(STRLIT)");
		Cconst(s_dotparen, ".(");
		Cconst(s_qbranch, "?BRANCH");
		Cconst(s_branch, "BRANCH");
		Cconst(s_xdo, "(XDO)");
		Cconst(s_xqdo, "(X?DO)");
		Cconst(s_xloop, "(XLOOP)");
		Cconst(s_pxloop, "(+XLOOP)");
		Cconst(s_abortq, "ABORT\"");
#undef Cconst

		if(stack == NULL) {	/* Allocate stack if needed */
			stack = (stackitem *)
				alloc(((unsigned int)pez_stklen) *
				  sizeof(stackitem));
		}
		stk = stackbot = stack;
#ifdef MEMSTAT
		stackmax = stack;
#endif
		stacktop = stack + pez_stklen;
		if(rstack == NULL) {	/* Allocate return stack if needed */
			rstack = (dictword ***)
				alloc(((unsigned int)pez_rstklen) *
				  sizeof(dictword **));
		}
		rstk = rstackbot = rstack;
#ifdef MEMSTAT
		rstackmax = rstack;
#endif
		rstacktop = rstack + pez_rstklen;
#ifdef WALKBACK
		if(wback == NULL) {
			wback =
				(dictword **)alloc(((unsigned int)pez_rstklen) *
						sizeof(dictword *));
		}
		wbptr = wback;
#endif
		if(heap == NULL) {

			/* The temporary string buffers are placed at the start
			   of the heap, which permits us to pointer-check
			   pointers into them as within the heap extents.
			   Hence, the size of the buffer we acquire for the heap
			   is the sum of the heap and temporary string requests.
			 */

			int i;
			char *cp;

			/* Force length of temporary strings to even number of
			   stackitems. */
			pez_ltempstr += sizeof(stackitem) -
				(pez_ltempstr % sizeof(stackitem));
			cp = alloc((((unsigned int)pez_heaplen) *
					sizeof(stackitem)) +
				   ((unsigned int)(pez_ntempstr *
							pez_ltempstr)));
			heapbot = (stackitem *)cp;
			strbuf = (char **)alloc(((unsigned int)pez_ntempstr) *
						 sizeof(char *));
			for(i = 0; i < pez_ntempstr; i++) {
				strbuf[i] = cp;
				cp += ((unsigned int)pez_ltempstr);
			}
			cstrbuf = 0;
			// Available heap memory starts after the temp strings:
			heap = (stackitem *)cp;
		}
		/* The system state word is kept in the first word of the heap
		   so that pointer checking doesn't bounce references to it.
		   When creating the heap, we preallocate this word and
		   initialise the state to the interpretive state. 
		 */
		hptr = heap + 1;
		state = Falsity;
#ifdef MEMSTAT
		heapmax = hptr;
#endif
		heaptop = heap + pez_heaplen;

		/* Now that dynamic memory is up and running, allocate constants
		   and variables built into the system.  */

#ifdef FILEIO
		{
			static struct {
				char *sfn;
				FILE *sfd;
			} stdfiles[] = {
				{
				"STDIN", NULL}, {
				"STDOUT", NULL}, {
				"STDERR", NULL}
			};
			int i;
			dictword *dw;

			/* On some systems stdin, stdout, and stderr aren't
			   constants which can appear in an initialisation.
			   So, we initialise them at runtime here. */

			stdfiles[0].sfd = stdin;
			stdfiles[1].sfd = stdout;
			stdfiles[2].sfd = stderr;

			for(i = 0; i < ELEMENTS(stdfiles); i++) {
				if((dw = pez_vardef(stdfiles[i].sfn,
							2 * sizeof(stackitem))) !=
				   NULL) {
					stackitem *si = pez_body(dw);
					*si++ = FileSent;
					*si = (stackitem)stdfiles[i].sfd;
				}
			}
		}
#endif				/* FILEIO */
		dictprot = dict;	/* Protect all standard words */
	}
}

/*  PEZ_LOOKUP	--  Look up a word in the dictionary.  Returns its
					word item if found or NULL if the word isn't
			in the dictionary. */

dictword *pez_lookup(name)
char *name;
{
	strcpy(tokbuf, name);	/* Use built-in token buffer... */
	ucase(tokbuf);		/* so ucase() doesn't wreck arg string */
	return lookup(tokbuf);	/* Now use normal lookup() on it */
}

/*  PEZ_BODY  --  Returns the address of the body of a word, given
		  its dictionary entry. */

stackitem *pez_body(dw)
dictword *dw;
{
	return ((stackitem *)dw) + Dictwordl;
}

/*  PEZ_EXEC  --  Execute a word, given its dictionary address.  The
				  evaluation status for that word's execution is
		  returned.  The in-progress evaluation status is
		  preserved. */

int pez_exec(dw)
dictword *dw;
{
	int sestat = evalstat, restat;

	evalstat = PEZ_SNORM;
#ifdef BREAK
	broken = False;		/* Reset break received */
#endif
#undef Memerrs
#define Memerrs evalstat
	Rso(1);
	Rpush = ip;		/* Push instruction pointer */
	ip = NULL;		/* Keep exword from running away */
	exword(dw);
	if(evalstat == PEZ_SNORM) {	/* If word ran to completion */
		Rsl(1);
		ip = R0;	/* Pop the return stack */
		Rpop;
	}
#undef Memerrs
#define Memerrs
	restat = evalstat;
	evalstat = sestat;
	return restat;
}

/*  PEZ_VARDEF  --  Define a variable word.  Called with the word's
			name and the number of bytes of storage to allocate
			for its body.  All words defined with pez_vardef()
			have the standard variable action of pushing their
			body address on the stack when invoked.  Returns
			the dictionary item for the new word, or NULL if
			the heap overflows. */

dictword *pez_vardef(name, size)
char *name;
int size;
{
	dictword *di;
	int isize = (size + (sizeof(stackitem) - 1)) / sizeof(stackitem);

#undef Memerrs
#define Memerrs NULL
	evalstat = PEZ_SNORM;
	Ho(Dictwordl + isize);
#undef Memerrs
#define Memerrs
	if(evalstat != PEZ_SNORM)	/* Did the heap overflow */
		return NULL;	/* Yes.  Return NULL */
	createword = (dictword *)hptr;	/* Develop address of word */
	createword->wcode = P_var;	/* Store default code */
	hptr += Dictwordl;	/* Allocate heap space for word */
	while(isize > 0) {
		Hstore = 0;	/* Allocate heap area and clear it */
		isize--;
	}
	strcpy(tokbuf, name);	/* Use built-in token buffer... */
	ucase(tokbuf);		/* so ucase() doesn't wreck arg string */
	enter(tokbuf);		/* Make dictionary entry for it */
	di = createword;	/* Save word address */
	createword = NULL;	/* Mark no word underway */
	return di;		/* Return new word */
}

/*  PEZ_MARK  --  Mark current state of the system.  */

void pez_mark(mp)
pez_statemark *mp;
{
	mp->mstack = stk;	/* Save stack position */
	mp->mheap = hptr;	/* Save heap allocation marker */
	mp->mrstack = rstk;	/* Set return stack pointer */
	mp->mdict = dict;	/* Save last item in dictionary */
}

/*  PEZ_UNWIND	--  Restore system state to previously saved state.  */

void pez_unwind(mp)
pez_statemark *mp;
{

	/* If pez_mark() was called before the system was initialised, and
	   we've initialised since, we cannot unwind.  Just ignore the
	   unwind request.  The user must manually pez_init before an
	   pez_mark() request is made. */

	if(mp->mdict == NULL)	/* Was mark made before pez_init ? */
		return;		/* Yes.  Cannot unwind past init */

	stk = mp->mstack;	/* Roll back stack allocation */
	hptr = mp->mheap;	/* Reset heap state */
	rstk = mp->mrstack;	/* Reset the return stack */

	/* To unwind the dictionary, we can't just reset the pointer,
	   we must walk back through the chain and release all the name
	   buffers attached to the items allocated after the mark was
	   made. */

	while(dict != NULL && dict != dictprot && dict != mp->mdict) {
		free(dict->wname);	/* Release name string for item */
		dict = dict->wnext;	/* Link to previous item */
	}
}

#ifdef BREAK

/*  PEZ_BREAK  --  Asynchronously interrupt execution.	Note that this
		   function only sets a flag, broken, that causes
		   exword() to halt after the current word.  Since
				   this can be called at any time, it daren't touch the
		   system state directly, as it may be in an unstable
		   condition. */

void pez_break()
{
	broken = True;		/* Set break request */
}
#endif				/* BREAK */

/*  PEZ_LOAD  --  Load a file into the system.	*/

int pez_load(fp)
FILE *fp;
{
	int es = PEZ_SNORM;
	char s[134];
	pez_statemark mk;
	pez_int scomm = pez_comment;	/* Stack comment pending state */
	dictword **sip = ip;	/* Stack instruction pointer */
	char *sinstr = instream;	/* Stack input stream */
	int lineno = 0;		/* Current line number */

	pez_errline = 0;	/* Reset line number of error */
	pez_mark(&mk);
	ip = NULL;		/* Fool pez_eval into interp state */
	while(pez_fgetsp(s, 132, fp) != NULL) {
		lineno++;
		if((es = pez_eval(s)) != PEZ_SNORM) {
			pez_errline = lineno;	/* Save line number of error */
			pez_unwind(&mk);
			break;
		}
	}
	/* If there were no other errors, check for a runaway comment.  If
	   we ended the file in comment-ignore mode, set the runaway comment
	   error status and unwind the file.  */
	if((es == PEZ_SNORM) && (pez_comment == Truth)) {
#ifdef MEMMESSAGE
		printf("\nRunaway `(' comment.\n");
#endif
		es = PEZ_RUNCOMM;
		pez_unwind(&mk);
	}
	pez_comment = scomm;	/* Unstack comment pending status */
	ip = sip;		/* Unstack instruction pointer */
	instream = sinstr;	/* Unstack input stream */
	return es;
}

/*  PEZ_PROLOGUE  --  Recognise and process prologue statement.
			  Returns 1 if the statement was part of the
			  prologue and 0 otherwise. */

int pez_prologue(sp)
char *sp;
{
	static struct {
		char *pname;
		pez_int *pparam;
	} proname[] = {
		{
		"STACK ", &pez_stklen}, {
		"RSTACK ", &pez_rstklen}, {
		"HEAP ", &pez_heaplen}, {
		"TEMPSTRL ", &pez_ltempstr}, {
		"TEMPSTRN ", &pez_ntempstr}
	};

	if(strncmp(sp, "# *", 3) == 0) {
		int i;
		char *vp = sp + 3, *ap;

		ucase(vp);
		for(i = 0; i < ELEMENTS(proname); i++) {
			if(strncmp(sp + 3, proname[i].pname,
				   strlen(proname[i].pname)) == 0) {
				if((ap = strchr(sp + 3, ' ')) != NULL) {
					sscanf(ap + 1, "%li",
						   proname[i].pparam);
#ifdef PROLOGUEDEBUG
					printf("Prologue set %sto %ld\n",
						   proname[i].pname,
						   *proname[i].pparam);
#endif
					return 1;
				}
			}
		}
	}
	return 0;
}

/*  PEZ_EVAL  --  Evaluate a string containing PEZ words.  */

int pez_eval(sp)
char *sp;
{
	int i;

#undef Memerrs
#define Memerrs evalstat
	instream = sp;
	evalstat = PEZ_SNORM;	/* Set normal evaluation status */
#ifdef BREAK
	broken = False;		/* Reset asynchronous break */
#endif

/* If automatic prologue processing is configured and we haven't yet
   initialised, check if this is a prologue statement.	If so, execute
   it.	Otherwise automatically initialise with the memory specifications
   currently operative. */

#ifdef PROLOGUE
	if(dict == NULL) {
		if(pez_prologue(sp))
			return evalstat;
		pez_init();
	}
#endif				/* PROLOGUE */

	while((evalstat == PEZ_SNORM) && (i = token(&instream)) != TokNull) {
		dictword *di;

		switch (i) {
		case TokWord:
			if(forgetpend) {
				forgetpend = False;
				ucase(tokbuf);
				if((di = lookup(tokbuf)) != NULL) {
					dictword *dw = dict;

					/* Pass 1.  Rip through the dictionary
					   to make sure this word is not past the
					   marker that guards against forgetting
					   too much.  */

					while(dw != NULL) {
						if(dw == dictprot) {
#ifdef MEMMESSAGE
							printf
								("\nForget protected.\n");
#endif
							evalstat =
								PEZ_FORGETPROT;
							di = NULL;
						}
						if(strcmp(dw->wname + 1, tokbuf)
						   == 0)
							break;
						dw = dw->wnext;
					}

					/* Pass 2.  Walk back through the dictionary
					   items until we encounter the target
					   of the FORGET.  Release each item's
					   name buffer and dechain it from the
					   dictionary list. */

					if(di != NULL) {
						do {
							dw = dict;
							if(dw->wname != NULL)
								free(dw->wname);
							dict = dw->wnext;
						} while(dw != di);
						/* Finally, back the heap
						 * allocation pointer up to the
						 * start of the last item
						 * forgotten. */
						hptr = (stackitem *)di;
						/* Uhhhh, just one more thing.
						 * If this word was defined with
						 * DOES>, there's a link to the
						 * method address hidden before
						 * its wnext field.  See if it's
						 * a DOES> by testing the wcode
						 * field for P_dodoes and, if
						 * so, back up the heap one more
						 * item. */
						if(di->wcode ==
						   (codeptr)P_dodoes) {
#ifdef FORGETDEBUG
							printf
								(" Forgetting DOES> word. ");
#endif
							hptr--;
						}
					}
				} else {
#ifdef MEMMESSAGE
					printf(" '%s' undefined ", tokbuf);
#endif
					evalstat = PEZ_UNDEFINED;
				}
			} else if(tickpend) {
				tickpend = False;
				ucase(tokbuf);
				if((di = lookup(tokbuf)) != NULL) {
					So(1);
					Push = (stackitem)di;	/* Push word compile address */
				} else {
#ifdef MEMMESSAGE
					printf(" '%s' undefined ", tokbuf);
#endif
					evalstat = PEZ_UNDEFINED;
				}
			} else if(defpend) {
				/* If a definition is pending, define the token and
				   leave the address of the new word item created for
				   it on the return stack. */
				defpend = False;
				ucase(tokbuf);
				if(pez_redef && (lookup(tokbuf) != NULL))
					printf("\n%s isn't unique.", tokbuf);
				enter(tokbuf);
			} else {
				di = lookup(tokbuf);
				if(di != NULL) {
					/* Test the state.  If we're
					 * interpreting, execute the word in all
					 * cases.  If we're compiling, compile
					 * the word unless it is a compiler word
					 * flagged for immediate execution by
					 * the presence of a space as the first
					 * character of its name in the
					 * dictionary entry. */
					if(state &&
					   (cbrackpend || ctickpend ||
						!(di->wname[0] & IMMEDIATE))) {
						if(ctickpend) {
							/* If a compile-time
							 * tick preceded this
							 * word, compile a (lit)
							 * word to cause its
							 * address to be pushed
							 * at execution time. */
							Ho(1);
							Hstore = s_lit;
							ctickpend = False;
						}
						cbrackpend = False;
						Ho(1);	/* Reserve stack space */
						Hstore = (stackitem)di;	/* Compile word address */
					} else {
						exword(di);	/* Execute word */
					}
				} else {
#ifdef MEMMESSAGE
					printf(" '%s' undefined ", tokbuf);
#endif
					evalstat = PEZ_UNDEFINED;
					state = Falsity;
				}
			}
			break;

		case TokInt:
			if(state) {
				Ho(2);
				Hstore = s_lit;	/* Push (lit) */
				Hstore = tokint;	/* Compile actual literal */
			} else {
				So(1);
				Push = tokint;
			}
			break;

#ifdef REAL
		case TokReal:
			if(state) {
				int i;
				union {
					pez_real r;
					stackitem s[Realsize];
				} tru;

				Ho(Realsize + 1);
				Hstore = s_flit;	/* Push (flit) */

				tru.r = tokreal;
				fflush(stderr);

				for(i = 0; i < Realsize; i++) {
					Hstore = tru.s[i];
				}
			} else {
				int i;
				union {
					pez_real r;
					stackitem s[Realsize];
				} tru;

				fflush(stderr);

				So(Realsize);
				tru.r = tokreal;
				for(i = 0; i < Realsize; i++) {
					Push = tru.s[i];
				}
			}
			break;
#endif				/* REAL */

		case TokString:
			if(state) {
				if(stringlit) {
					stringlit = False;
				} else {
					Ho(1);
					Hstore = s_strlit; // Address of the P_strlit instruction
				}
				pez_heap_string(tokbuf);
			} else {
				if(stringlit) {
					stringlit = False;
					printf("%s", tokbuf);
				} else {
					So(1);
					pez_stack_string(tokbuf);
				}
			}
			break;
		default:
			printf("\nUnknown token type %d\n", i);
			break;
		}
	}
	return evalstat;
}

int pez_heap_string(char* str) {
	int l = (strlen(str) + 1 + sizeof(stackitem)) / sizeof(stackitem);
	Ho(l);
	*((char *)hptr) = l;	 // Store in-line skip length
	strcpy(((char *)hptr) + 1, str);
	hptr += l;
	return PEZ_SNORM;
}

void pez_stack_string(char* str) {
	strncpy(strbuf[cstrbuf], str, pez_ltempstr - 1);
	Push = (stackitem)strbuf[cstrbuf];
	cstrbuf = (cstrbuf + 1) % ((int)pez_ntempstr);
}
