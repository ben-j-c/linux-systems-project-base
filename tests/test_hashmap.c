#include "data-structures/hashtable.h"
#include "errstack.h"
#include "test_utils.h"
#include "util.h"

int test_1_basic(void)
{
	HT_CLEANUP ht_st *t;
	ES_FWD_INT(ht_str_alloc(&t, 0, NULL, NULL), "Failed to alloc");
	ES_FWD_INT_NM(ht_str_set(t, "key1", 11L));
	ES_FWD_INT_NM(ht_str_set(t, "key2", 12L));
	ES_FWD_INT_NM(ht_str_set(t, "key3", 13L));
	ES_FWD_INT_NM(ht_str_set(t, "key4", 14L));
	ES_FWD_INT_NM(ht_str_set(t, "key5", 15L));
	ES_FWD_INT_NM(ht_str_set(t, "key6", 16L));
	ES_FWD_INT_NM(ht_str_set(t, "key7", 17L));
	ES_FWD_INT_NM(ht_str_set(t, "key8", 18L));
	ES_FWD_INT_NM(ht_str_set(t, "key9", 19L));

	ES_NEW_ASRT_NM((long) ht_str_get(t, "key1") == 11);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key2") == 12);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key3") == 13);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key4") == 14);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key5") == 15);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key6") == 16);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key7") == 17);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key8") == 18);
	ES_NEW_ASRT_NM((long) ht_str_get(t, "key9") == 19);

	return 1;
}

#define N 100000
int test_2_large_insert(void)
{
	long i;
	HT_CLEANUP ht_st *t;
	ES_FWD_INT(ht_int_alloc(&t, 0, NULL, NULL), "Failed to alloc");

	for (i = 0; i < N; i++) {
		ES_FWD_INT_NM(ht_int_set(t, i, i * i));
	}
	ES_NEW_ASRT_NM(ht_size(t) == N);

	for (i = 0; i < N; i++) {
		ES_NEW_ASRT_NM((long) ht_str_get(t, i) == i * i);
		ht_delete(t, (void *) i);
		ES_NEW_ASRT_NM(!ht_has(t, (void *) i));
	}

	ES_NEW_ASRT_NM(ht_size(t) == 0);
	return 1;
}

static test_function tests[] = {
    test_1_basic,
    test_2_large_insert,
};

TESTER_MAIN(tests);