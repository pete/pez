#include <stdio.h>
#include <string.h>
#include "pezdef.h"
#include "minunit.h"

char msg[50];

static char* test_stack_int() {
	pez_stack_int(42);
	
	sprintf(msg, "NoS should be %d, is %d", 42, (int)S0);
	mu_assert(msg, 42 == (int)S0);
	
	return 0;
}

static char* test_stack_real() {
	pez_stack_real(3.14159);
	
	sprintf(msg, "ToS as real should be %f, is %f", 3.14159, REAL0);
	mu_assert(msg, 3.14159 == REAL0);
	
	return 0;
}

char* eval_functions_suite() {
	pez_init();
	mu_run_test(test_stack_int);
	mu_run_test(test_stack_real);
	return 0;
}
