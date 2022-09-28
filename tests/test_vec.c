#include "data-structures/vec.h"
#include "errstack.h"
#include "stdlib.h"
#include "test_utils.h"
#include "util.h"

int test_1_create(void)
{
	VEC_CLEANUP vec_t *v;
	vec_alloc(&v, sizeof(int));
	ES_FWD_INT(vec_alloc(&v, sizeof(int)), "Failed to alloc");
	ES_NEW_ASRT(vec_size(v) == 0, "Size wasn't zero");
	return 0;
}

#define N 100000
int test_2_push_back_pop_back(void)
{
	int i;
	VEC_CLEANUP vec_t *v;
	ES_FWD_INT(vec_alloc(&v, sizeof(int)), "Failed to alloc");
	for (i = 0; i < N; i++) {
		ES_FWD_INT(vec_push_back(v, &i), "Push back failed for %d", i);
	}
	ES_NEW_ASRT(vec_size(v) == N, "Sizes don't match, got %zu expected %d", vec_size(v), N);
	for (i = 0; i < N; i++) {
		int *actual = vec_at(v, N - i - 1);
		ES_NEW_ASRT((int) vec_size(v) == N - i, "Size mismatch at %d", i);
		ES_NEW_ASRT(*actual == N - i - 1,
		            "Wrong value at position %d, expected %d got %d",
		            N - i,
		            *actual);
		ES_FWD_INT(vec_pop_back(v), "Failed to pop back at %d", i);
	}
	ES_NEW_ASRT(vec_size(v) == 0, "Expected 0 size, got %zu", vec_size(v));
	return 0;
}

int test_3_take_data(void)
{
	int i, *data;
	size_t size, capacity;
	VEC_CLEANUP vec_t *v;
	ES_FWD_INT(vec_alloc(&v, sizeof(int)), "Failed to alloc");
	for (i = 0; i < N; i++) {
		ES_FWD_INT(vec_push_back(v, &i), "Push back failed for %d", i);
	}
	data = vec_take_data(&v, &size, &capacity);
	ES_NEW_ASRT(data, "failed to take data");
	ES_NEW_ASRT(size <= capacity,
	            "Capacity doesn't make sense (size=%zu, cap=%zu)",
	            size,
	            capacity);
	ES_NEW_ASRT(v == NULL, "Vector wasn't cleared");
	free(data);
	return 0;
}

int _test_4_1_foreach(UNUSED const vec_t *vec, size_t idx, UNUSED void *data, void *arg_vp)
{
	if (idx == N / 2) {
		return 0;
	}
	(*(int *) arg_vp)++;
	return 1;
}

int _test_4_2_foreach(UNUSED const vec_t *vec, UNUSED size_t idx, UNUSED void *data, void *arg_vp)
{
	(*(int *) arg_vp)++;
	return 1;
}

int _test_4_3_foreach(UNUSED const vec_t *vec, UNUSED size_t idx, UNUSED void *data, void *arg_vp)
{
	(*(int *) arg_vp)++;
	return -1;
}

int test_4_foreach(void)
{
	int i, res, counter = 0;
	VEC_CLEANUP vec_t *v;
	ES_FWD_INT(vec_alloc(&v, sizeof(int)), "Failed to alloc");
	for (i = 0; i < N; i++) {
		ES_FWD_INT(vec_push_back(v, &i), "Push back failed for %d", i);
	}
	res = vec_foreach(v, &counter, _test_4_1_foreach);
	ES_NEW_ASRT(res == 0 && counter == N / 2, "Wrong result, got %d %d", res, counter);
	counter = 0;
	res     = vec_foreach(v, &counter, _test_4_2_foreach);
	ES_NEW_ASRT(res == 1 && counter == N, "Wrong result, got %d %d", res, counter);
	counter = 0;
	res     = vec_foreach(v, &counter, _test_4_3_foreach);
	ES_NEW_ASRT(res == -1 && counter == 1, "Wrong result, got %d %d", res, counter);
	return 1;
}

static test_function tests[] = {
    test_1_create,
    test_2_push_back_pop_back,
    test_3_take_data,
    test_4_foreach,
};

TESTER_MAIN(tests);
