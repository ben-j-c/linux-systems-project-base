#include "data-structures/avl.h"
#include "errstack.h"
#include "test_utils.h"
#include "util.h"

typedef struct test_s
{
	avl_node_t avl_node;
	int a;
} ts_t;

int _cmp(const avl_node_st *a, const avl_node_st *b)
{
	return ((ts_t *) b)->a - ((ts_t *) a)->a;
}

int test_1_basic(void)
{
	avl_t *tree;
	ES_FWD_INT_NM(avl_alloc(&tree, _cmp));

	return 1;
}

static test_function tests[] = {
    test_1_basic,
};

TESTER_MAIN(tests);