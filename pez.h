/*

			      A T L A S T

	  Autodesk Threaded Language Application System Toolkit

		     Program Linkage Definitions

     Designed and implemented in January of 1990 by John Walker.

     This  module  contains  the  definitions  needed by programs that
     invoke the ATLAST system.	It does contain the  definitions  used
     internally   within  ATLAST  (which  might  create  conflicts  if
     included in calling programs).

		This program is in the public domain.

*/

#ifndef __ATLAST_H
#define __ATLAST_H

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

/*  ATL_EVAL return status codes  */

#define ATL_SNORM	0	      /* Normal evaluation */
#define ATL_STACKOVER	-1	      /* Stack overflow */
#define ATL_STACKUNDER	-2	      /* Stack underflow */
#define ATL_RSTACKOVER	-3	      /* Return stack overflow */
#define ATL_RSTACKUNDER -4	      /* Return stack underflow */
#define ATL_HEAPOVER	-5	      /* Heap overflow */
#define ATL_BADPOINTER	-6	      /* Pointer outside the heap */
#define ATL_UNDEFINED	-7	      /* Undefined word */
#define ATL_FORGETPROT	-8	      /* Attempt to forget protected word */
#define ATL_NOTINDEF	-9	      /* Compiler word outside definition */
#define ATL_RUNSTRING	-10	      /* Unterminated string */
#define ATL_RUNCOMM	-11	      /* Unterminated comment in file */
#define ATL_BREAK	-12	      /* Asynchronous break signal received */
#define ATL_DIVZERO	-13	      /* Attempt to divide by zero */
#define ATL_APPLICATION -14	      /* Application primitive pez_error() */

/*  Entry points  */

extern void pez_init(), pez_mark(), pez_unwind(), pez_break();
extern int pez_eval(), pez_load();
extern void pez_memstat();

#endif
