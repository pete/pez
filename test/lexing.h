#include <stdio.h>
#include <string.h>
#include "pezdef.h"
#include "minunit.h"

char msg[50]; // for when you want to sprintf stuff

// declare, maybe link will work?
int get_quoted_string(char **strbuf, char token_buffer[]);

static char* test_quoted_string() {
	int valid;
	int i;
	char *input[150];
	*input = "\"John Bigboote\"";
	char token_buffer[128];
	valid = get_quoted_string(input, token_buffer);
	mu_assert("String should be valid", valid);
	mu_assert("String different?", strcmp(token_buffer, "John Bigboote") == 0);
	
	*input = "\"\"";
	valid = get_quoted_string(input, token_buffer);
	mu_assert("Empty string should be valid", valid);
	
	*input = "\"";
	valid = get_quoted_string(input, token_buffer);
	mu_assert("Unterminated string should be invalid", !valid);
	
	return 0;
}

char* lexing_suite() {
	pez_init(); // Usually necessary.
	
	mu_run_test(test_quoted_string);

	return 0;
}
