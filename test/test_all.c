#include "api.h"
#include "minunit.h"

int main(int argc, char **argv) {
	mu_run_suite(test_api);
	// mu_run_suite(test_something);
	// mu_run_suite(test_or_other);

	printf("ALL TESTS PASSED. Tests run: %d\n", tests_run);

	return 0;
}