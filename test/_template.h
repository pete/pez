#include <stdio.h>
#include <string.h>
#include "pezdef.h"
#include "minunit.h"

char msg[50]; // for when you want to sprintf stuff

static char* test_something() {
	int a = 1;
	
	sprintf(msg, "%d should equal %d", a, a);
	mu_assert(msg, a == a);
	
	return 0;
}

char* test_suite_name() {
	pez_init(); // Usually necessary.
	
	mu_run_test(test_something);

	return 0;
}
