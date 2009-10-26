/* file: minunit.h (modified from:
 * http://www.jera.com/techinfo/jtns/jtn002.html) */

#define mu_assert(message, test) \
        do { if (!(test)) { \
                sprintf(minunit_msg, \
				"%s:%d ASSERT - %s", __FILE__, __LINE__, message); \
                return minunit_msg; }\
        } while (0)

#define mu_assert_eq(expected, actual) {\
        long a = (long)(expected); \
        long b = (long)(actual); \
        do { if ((a) != (b)) { \
                sprintf(minunit_msg, \
				"%s:%d: ASSERT_EQ - expected %ld, got %ld", \
				__FILE__, __LINE__, a, b);\
                return minunit_msg; }; \
        } while(0); }

#define mu_assert_ge(min, actual) {\
        long a = (min); \
        long b = (actual); \
        do { if ((b) < (a)) { \
                sprintf(minunit_msg, \
				"%s:%d: ASSERT_GE - expected min %ld, got %ld", \
				__FILE__, __LINE__, a, b);\
                return minunit_msg; }; \
        } while(0); }

#define mu_run_test(test) do { char *message = test(); tests_run++; \
        if (message) return message; } while (0)

#define mu_run_suite(suite) do { char* result = suite(); \
        if (result != 0) {printf("FAILURE %s\n", result); return 1; } } while(0)

extern int tests_run;
char minunit_msg[255];

