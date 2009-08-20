#include <stdio.h>
#include <string.h>
#include "pezdef.h"
#include "minunit.h"

char msg[50];

static char* test_eval() {
	int val;
	
	pez_eval("3 4 5");
	
	val = (int)S0;
	sprintf(msg, "ToS should be %d, is %d", 5, val);
	mu_assert(msg, 5 == val);
	
	val = (int)S1;
	sprintf(msg, "NoS should be %d, is %d", 4, val);
	mu_assert(msg, 4 == val);
	
    return 0;
}

static char* test_mark() {
	pez_statemark mk;
	pez_mark(&mk);
	mu_assert("mk.mstack == stack addr", stk == mk.mstack);
	mu_assert("mk.mrstack == return stack addr", rstk == mk.mrstack);

	return 0;
}

static char* test_lookup() {
	dictword *dw;
	char* wordname;
	
	dw = pez_lookup("cr");
	wordname = dw->wname + 1;
	mu_assert("Expected word name to be 'CR'", strcmp(wordname, "CR") == 0);
	
	dw = pez_lookup("john_big_boote");
	mu_assert("Expected NULL for bogus word lookup", dw == NULL);
	return 0;
}

static char* test_vardef() {
	dictword *pi;
	pi = pez_vardef("Pi", sizeof(pez_real));
	
	*((pez_real *) pez_body(pi)) = 3.14159;
	pez_eval("pi 2@");
	
	sprintf(msg, "ToS real should be %f, is %f", 3.14159, REAL0);
	mu_assert(msg, 3.14159 == REAL0);
	
	return 0;
}

static char* test_exec() {
	dictword *dw;
	
	dw = pez_lookup("swap");
	pez_eval("7 13");
	pez_exec(dw);
	
	sprintf(msg, "ToS should be %d, is %d", 7, (int)S0);
	mu_assert(msg, 7 == (int)S0);
	
	sprintf(msg, "NoS should be %d, is %d", 13, (int)S1);
	mu_assert(msg, 13 == (int)S1);
	
	return 0;
}

char* api_suite() {
	pez_init();	
	mu_run_test(test_eval);
	mu_run_test(test_mark);
	mu_run_test(test_lookup);
	mu_run_test(test_vardef);
	mu_run_test(test_exec);
	return 0;
}