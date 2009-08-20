#include "api.h"
#include "eval_functions.h"
#include "lexing.h"
#include "minunit.h"

int tests_run = 0;


int main(int argc, char **argv) {
	mu_run_suite(api_suite);
	mu_run_suite(eval_functions_suite);
	mu_run_suite(lexing_suite);

	printf("ALL TESTS PASSED. %d tests run.\n", tests_run);

	return 0;
}