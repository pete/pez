#include <stdio.h>
#include <string.h>
#include "pezdef.h"
#include "minunit.h"

char msg[50]; // for when you want to sprintf stuff

static char* test_something() {
	
	
	return 0;
}

char* lexing_suite() {
	pez_init(); // Usually necessary.
	
	mu_run_test(test_something);

	return 0;
}
