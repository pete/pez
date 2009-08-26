/*

   Pez
   Program Linkage Definitions

   This module contains the definitions needed by programs that
   invoke the PEZ system.  It does contain the definitions used
   internally within PEZ (which might create conflicts if
   included in calling programs).

   See doc/CREDITS for information about the authors.  
   This program is in the public domain.
*/

#ifndef __PEZ_H
#define __PEZ_H

#include <config.h>

typedef long pez_int;		      /* Stack integer type */
typedef double pez_real;	      /* Real number type */

/*  External symbols accessible by the calling program.  */

extern pez_int pez_stklen;	      /* Initial/current stack length */
extern pez_int pez_rstklen;	      /* Initial/current return stack length */
extern pez_int pez_heaplen;	      /* Initial/current heap length */
extern pez_int pez_ltempstr;	      /* Temporary string buffer length */
extern pez_int pez_ntempstr;	      /* Number of temporary string buffers */

extern pez_int pez_trace;	      /* Trace mode */
extern pez_int pez_walkback;	      /* Error walkback enabled mode */
extern pez_int pez_comment;	      /* Currently ignoring comment */
extern pez_int pez_redef;	      /* Allow redefinition of words without
                                         issuing the "not unique" warning. */
extern pez_int pez_errline;	      /* Line number where last pez_load()
					 errored or zero if no error. */

extern pez_int pez_argc;
extern char **pez_argv;		// The argv that the pez program sees.

/*  PEZ_EVAL return status codes  */

#define PEZ_SNORM	0	      /* Normal evaluation */
#define PEZ_STACKOVER	-1	      /* Stack overflow */
#define PEZ_STACKUNDER	-2	      /* Stack underflow */
#define PEZ_RSTACKOVER	-3	      /* Return stack overflow */
#define PEZ_RSTACKUNDER -4	      /* Return stack underflow */
#define PEZ_HEAPOVER	-5	      /* Heap overflow */
#define PEZ_BADPOINTER	-6	      /* Pointer outside the heap */
#define PEZ_UNDEFINED	-7	      /* Undefined word */
#define PEZ_FORGETPROT	-8	      /* Attempt to forget protected word */
#define PEZ_NOTINDEF	-9	      /* Compiler word outside definition */
#define PEZ_RUNSTRING	-10	      /* Unterminated string */
#define PEZ_RUNCOMM	-11	      /* Unterminated comment in file */
#define PEZ_BREAK	-12	      /* Asynchronous break signal received */
#define PEZ_DIVZERO	-13	      /* Attempt to divide by zero */
#define PEZ_APPLICATION -14	      /* Application primitive pez_error() */

/*  Entry points  */

extern void pez_init(), pez_mark(), pez_unwind(), pez_break();
extern int pez_eval(char *), pez_load(FILE *);

extern void pez_stack_int(pez_int);
extern void pez_stack_real(pez_real);
extern void pez_stack_string(char*);

extern void pez_heap_real(pez_real);
extern void pez_heap_string(char*);
extern void pez_memstat();

#endif
