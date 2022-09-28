#include "data-structures/linkedlist.h"
#include "errstack.h"
#include "test_utils.h"
#include "util.h"

typedef struct test_struct
{
	llist_st list_handle;
	int data;
} ts_t;

int test_1_init_empty_size(void)
{
	llist_st head;
	ll_init(&head);
	ES_NEW_ASRT(head.next == &head, "Next wrong");
	ES_NEW_ASRT(head.prev == &head, "Next wrong");
	ES_NEW_ASRT(ll_is_empty(&head), "Not empty");
	return 0;
}

int test_2_emplace_foreach(void)
{
	llist_st head;
	llist_st *iter;
	int counter = 0;
	ts_t a      = {.data = 0};
	ts_t b      = {.data = 1};
	ts_t c      = {.data = 2};
	ll_init(&head);

	ll_emplace_back(&head, &a);
	ll_emplace_back(&head, &b);
	ll_emplace_back(&head, &c);

	ES_NEW_ASRT(!ll_is_empty(&head), "Shouldn't be empty");
	LL_FOREACH(&head, iter)
	{
		ES_NEW_ASRT(counter == ((ts_t *) iter)->data, "Failed iterator when counter=%d", counter);
		counter++;
	}
	return 0;
}

int test_3_foreach_remove(void)
{
	llist_st head;
	llist_st *iter;
	ts_t a = {.data = 0};
	ts_t b = {.data = 1};
	ts_t c = {.data = 2};
	ll_init(&head);

	ll_emplace_back(&head, &a);
	ll_emplace_back(&head, &b);
	ll_emplace_back(&head, &c);

	LL_FOREACH(&head, iter)
	{
		ts_t *curr = (void *) iter;
		if (curr->data == 1) {
			ll_remove(iter);
			break;
		}
	}

	ES_NEW_ASRT(((ts_t *) ll_get(&head, 0))->data == 0, "Wrong data for position 1");
	ES_NEW_ASRT(((ts_t *) ll_get(&head, 1))->data == 2, "Wrong data for position 2");
	return 0;
}

static test_function tests[] = {
    test_1_init_empty_size,
    test_2_emplace_foreach,
    test_3_foreach_remove,
};

TESTER_MAIN(tests);