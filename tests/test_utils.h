#pragma once

typedef int (*test_function)(void);

#define TESTER_MAIN(tests)                                                                      \
	int main(void)                                                                                 \
	{                                                                                              \
		int ret = 0;                                                                               \
		size_t i;                                                                                  \
		for (i = 0; i < ARRAY_SIZE(tests); i++) {                                                  \
			int test_result;                                                                       \
			printf("Running sub-test %zu: ", i);                                                   \
			if ((test_result = tests[i]()) < 0) {                                                  \
				printf("Failed with [ ");                                                          \
				ES_PRINT();                                                                        \
				printf("\n ]\n");                                                                  \
				ret = -1;                                                                          \
			} else {                                                                               \
				printf("Success\n");                                                               \
			}                                                                                      \
		}                                                                                          \
		return ret;                                                                                \
	}
