#include "api.h"
#include "eval_functions.h"
#include "minunit.h"

int tests_run = 0;


int main(int argc, char **argv) {
	mu_run_suite(test_api);
	mu_run_suite(test_eval_functions);

	printf("ALL TESTS PASSED. %d tests run.\n", tests_run);

	return 0;
}