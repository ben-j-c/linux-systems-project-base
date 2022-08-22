/**
 * @file linkedlist.c
 * @author your name (you@domain.com)
 * @brief The implementation for linkedlist.h
 * @version 0.1
 * @date 2022-08-21
 *
 * @copyright Copyright (c) 2022 Benjamin Correia. All rights reserved.
 * @license This file is MIT licensed
 *
 */
#include "linkedlist.h"

void ll_init(llist_st *dst)
{
	dst->next = dst;
	dst->prev = dst;
}

void ll_push_back(llist_st *dst, llist_st *src)
{
	llist_st *new_dst_prev = src->prev;
	dst->prev->next        = src;
	src->prev->next        = dst;
	src->prev              = dst->prev;
	dst->prev              = new_dst_prev;
}

void ll_push_front(llist_st **dst, llist_st *src)
{
	llist_st *new_dst_prev = src->prev;
	src->prev->next        = *dst;
	(*dst)->prev->next     = src;
	src->prev              = (*dst)->prev;
	(*dst)->prev           = new_dst_prev;

	*dst = src;
}

bool ll_is_empty(llist_st *src)
{
	return src->next == src && src->prev == src;
}

int ll_foreach(llist_st *list,
               void *state,
               int (*body)(llist_st *list, llist_st *node, size_t idx, void *state))
{
	llist_st *cur = list->next;
	size_t idx    = 0;
	while (cur != list) {
		int ret = body(list, cur, idx, state);
		if (ret < 0) {
			return ret;
		}
		if (ret == 0) {
			return idx + 1;
		}
		idx++;
		cur = cur->next;
	}
	return idx;
}

llist_st *ll_get(llist_st *list, size_t idx)
{
	llist_st *cur = list->next;
	while (idx) {
		idx--;
		cur = cur->next;
	}
	return cur;
}

llist_st *ll_get_reverse(llist_st *list, size_t idx)
{
	llist_st *cur = list->prev;
	while (idx) {
		idx--;
		cur = cur->prev;
	}
	return cur;
}

llist_st *ll_find(llist_st *list,
                  void *state,
                  bool (*pred)(llist_st *node, size_t idx, void *state))
{
	llist_st *cur = list->next;
	size_t idx    = 0;
	while (cur != list) {
		if (pred(cur, idx, state)) {
			return cur;
		}
	}
	return NULL;
}

void ll_remove(llist_st *to_remove)
{
	if (to_remove->next == to_remove) {
		return;
	}
	to_remove->prev->next = to_remove->next;
	to_remove->next->prev = to_remove->prev;
}
