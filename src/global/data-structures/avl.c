/**
 * @file avl.c
 * @author Benjamin Correia (ben-j-c)
 * @brief An avl tree implementation featuring various allocation schemes
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022 Benjamin Correia. All rights reserved.
 * @license This file is MIT licensed
 *
 * I'm afraid to test this file, I wrote it all in one shot without incrementally testing
 */
#include "avl.h"

#include <stdlib.h>

#include "linkedlist.h"
#include "util.h"

struct avl_s
{
	avl_node_st *root;
	avl_cmp_ft cmp;
};

enum direction
{
	LEFT,
	RIGHT,
};

#define OPPOSITE(dir)   (1 - dir)
#define AS_BALANCE(dir) (2 * dir - 1)

static void _rotate(avl_node_st **root, enum direction dir)
{
	avl_node_st *new_root;
	avl_node_st *new_child;
	new_root                            = (*root)->child[1 - dir];
	new_child                           = (*root)->child[1 - dir]->child[dir];
	(*root)->child[1 - dir]->child[dir] = *root;
	(*root)->child[1 - dir]             = new_child;
	new_root->balance += dir * 2 - 1;
	new_child->balance -= dir * 2 - 1;
}

static void _rebalance(avl_node_st **a, enum direction dir)
{
	if ((*a)->balance == 2 || (*a)->balance == -2) {
		if (((*a)->balance * (*a)->child[dir]->balance) < 0) {
			_rotate(&(*a)->child[dir], dir);
			_rotate(a, OPPOSITE(dir));
		} else {
			_rotate(a, OPPOSITE(dir));
		}
	}
}

int _insert(avl_st *tree,
            avl_node_st **curr,
            avl_node_st *to_add,
            enum direction dir,
            int parent_balance)
{
	int prev_balance;
	enum direction child_insert_side;
	if (*curr == NULL) {
		*curr = to_add;
		return parent_balance + 2 * dir - 1;
	}
	prev_balance      = (*curr)->balance;
	child_insert_side = tree->cmp(*curr, to_add) < 0 ? LEFT : RIGHT;
	(*curr)->balance  = _insert(tree,
                               &((*curr)->child[child_insert_side]),
                               to_add,
                               child_insert_side,
                               (*curr)->balance);
	_rebalance(curr, child_insert_side);
	if ((*curr)->balance != 0 && prev_balance != (*curr)->balance) {
		return parent_balance + 2 * dir - 1;
	}
	return parent_balance;
}

int avl_alloc(avl_st **dst, avl_cmp_ft cmp)
{
	void *tmp = malloc(sizeof(avl_st));
	if (tmp == NULL) {
		return -1;
	}
	*dst         = tmp;
	(*dst)->cmp  = cmp;
	(*dst)->root = NULL;
	return 1;
}

void avl_add(avl_st *dst, avl_node_st *node)
{
	_insert(dst, &dst->root, node, RIGHT, 0);
}

void _swap(avl_node_st **a, avl_node_st **b)
{
	SWAP(*a, *b);
	SWAP((*a)->balance, (*b)->balance);
}

int _del_with_swap(avl_node_st **a, avl_node_st **to_del, enum direction dir, int parent_balance)
{
	int prev_balance;
	if (!(*a)->child[dir]) {
		_swap(a, to_del);
		*a = (*a)->child[1 - dir];
		return parent_balance - AS_BALANCE(dir);
	}
	prev_balance  = (*a)->balance;
	(*a)->balance = _del_with_swap(&((*a)->child[dir]), to_del, dir, (*a)->balance);
	_rebalance(a, OPPOSITE(dir));
	if ((*a)->balance == 0 && prev_balance != (*a)->balance) {
		return parent_balance + 2 * (1 - dir) - 1;
	}
	return parent_balance;
}

int _delete(avl_st *tree,
            avl_node_st **dst,
            avl_node_st **curr,
            avl_node_st *to_del,
            enum direction dir,
            int parent_balance)
{
	int prev_balance;
	int cmp_val;
	enum direction del_direction;
	if (*curr == NULL) {
		return parent_balance;
	}
	cmp_val      = tree->cmp(*curr, to_del);
	prev_balance = (*curr)->balance;
	if (cmp_val == 0) {
		int balance_change;
		/* Swap this node with the most inward child node. Update this balance if child tree changes
		 * height*/
		*dst = *curr;
		if ((*curr)->child[LEFT]) {
			balance_change = _del_with_swap(&(*curr)->child[LEFT], curr, RIGHT, 0);
			del_direction  = LEFT;

		} else if ((*curr)->child[RIGHT]) {
			balance_change = _del_with_swap(&(*curr)->child[RIGHT], curr, LEFT, 0);
			del_direction  = RIGHT;
		} else {
			*curr = NULL;
			return parent_balance - dir * 2 + 1;
		}
		if (balance_change) {
			(*curr)->balance -= AS_BALANCE(del_direction);
		}
	} else {
		del_direction    = cmp_val < 0 ? LEFT : RIGHT;
		(*curr)->balance = _delete(tree,
		                           dst,
		                           &((*curr)->child[del_direction]),
		                           to_del,
		                           del_direction,
		                           (*curr)->balance);
	}
	/* On way back up */
	_rebalance(curr, OPPOSITE(del_direction));
	if ((*curr)->balance == 0 && prev_balance != (*curr)->balance) {
		return parent_balance + 2 * dir - 1;
	}
	return parent_balance;
}

avl_node_st *avl_del(avl_st *tree, avl_node_st *to_remove)
{
	avl_node_st *ret = NULL;
	_delete(tree, &ret, &tree->root, to_remove, RIGHT, 0);
	return ret;
}

avl_node_st *_find_eq(const avl_st *tree, avl_node_st *curr, const avl_node_st *node)
{
	if (curr) {
		int cmp = tree->cmp(curr, node);
		if (cmp == 0) {
			return curr;
		} else if (cmp < 0) {
			return _find_eq(tree, curr->child[LEFT], node);
		} else {
			return _find_eq(tree, curr->child[RIGHT], node);
		}
	}
	return NULL;
}

avl_node_st *avl_find_eq(const avl_st *tree, const avl_node_st *node)
{
	return _find_eq(tree, tree->root, node);
}

avl_node_st *avl_min(const avl_st *tree)
{
	avl_node_st *curr = tree->root;
	while (curr && curr->child[LEFT]) {
		curr = curr->child[LEFT];
	}
	return curr;
}

avl_node_st *avl_max(const avl_st *tree)
{
	avl_node_st *curr = tree->root;
	while (curr && curr->child[RIGHT]) {
		curr = curr->child[RIGHT];
	}
	return curr;
}

typedef int(
    _traversal_step_f)(size_t *, avl_node_st *, void *, avl_iter_ft, enum avl_traversal_order_e);

static _traversal_step_f _left;
static _traversal_step_f _right;
static _traversal_step_f _body;

typedef int (
    *_traversal_step_ft)(size_t *, avl_node_st *, void *, avl_iter_ft, enum avl_traversal_order_e);

static const _traversal_step_ft _steps[][3] = {
    [AVL_IN_ORDER]           = {_left, _body, _right},
    [AVL_PRE_ORDER]          = {_body, _left, _right},
    [AVL_POST_ORDER]         = {_left, _right, _body},
    [AVL_IN_ORDER_REVERSE]   = {_right, _body, _left},
    [AVL_PRE_ORDER_REVERSE]  = {_right, _left, _body},
    [AVL_POST_ORDER_REVERSE] = {_body, _right, _left},
};

static int _foreach_order(size_t *idx,
                          avl_node_st *curr,
                          void *data,
                          avl_iter_ft body,
                          enum avl_traversal_order_e order)
{
	int i;
	for (i = 0; i < 3; i++) {
		int res = _steps[order][i](idx, curr, data, body, order);
		if (res <= 0) {
			return res;
		}
	}
	return 1;
}

static int _left(size_t *idx,
                 avl_node_st *curr,
                 void *data,
                 avl_iter_ft body,
                 enum avl_traversal_order_e order)
{
	int res = _foreach_order(idx, curr->child[LEFT], data, body, order);
	if (res <= 0) {
		return res;
	}
	return 1;
}

static int _right(size_t *idx,
                  avl_node_st *curr,
                  void *data,
                  avl_iter_ft body,
                  enum avl_traversal_order_e order)
{
	int res = _foreach_order(idx, curr->child[RIGHT], data, body, order);
	if (res <= 0) {
		return res;
	}
	return 1;
}

static int _body(size_t *idx,
                 avl_node_st *curr,
                 void *data,
                 avl_iter_ft body,
                 UNUSED enum avl_traversal_order_e order)
{
	int res = body(curr, *idx, data);
	(*idx)++;
	if (res <= 0) {
		return res;
	}
	return 1;
}

struct _breadth_element
{
	llist_st list_data;
	avl_node_st *to_process;
};

static void _queue_cleanup(llist_st *head)
{
	while (!ll_is_empty(head)) {
		llist_st *to_remove = ll_get(head, 0);
		ll_remove(to_remove);
		free(to_remove);
	}
}

static int _foreach_breadth(size_t *idx, avl_node_st *start, void *data, avl_iter_ft body)
{
	int res;
	CLEANUP(_queue_cleanup) llist_st head;
	ll_init(&head);
	struct _breadth_element *curr = NULL;
	if (start) {
		curr = calloc(1, sizeof(*curr));
		if (!curr) {
			return -1;
		}
		curr->to_process = start;
		ll_emplace_back(&head, curr);
	}
	while (curr) {
		if (curr->to_process->child[LEFT]) {
			struct _breadth_element *to_queue = calloc(1, sizeof(*to_queue));
			if (!to_queue) {
				return -1;
			}
			to_queue->to_process = curr->to_process->child[LEFT];
			ll_emplace_back(&head, to_queue);
		}
		if (curr->to_process->child[RIGHT]) {
			struct _breadth_element *to_queue = calloc(1, sizeof(*to_queue));
			if (!to_queue) {
				return -1;
			}
			to_queue->to_process = curr->to_process->child[RIGHT];
			ll_emplace_back(&head, to_queue);
		}

		res = body(curr->to_process, *idx, data);
		(*idx)++;
		if (res <= 0) {
			return res;
		}
		ll_remove((void *) curr);
		free(curr);
		curr = (void *) ll_get(&head, 0);
	}
	return 1;
}

ssize_t avl_foreach(avl_st *tree, void *data, avl_iter_ft body, enum avl_traversal_order_e order)
{
	int res;
	size_t idx = 0;
	if (order < AVL_BREADTH_FIRST && order >= 0) {
		res = _foreach_order(&idx, tree->root, data, body, order);
	} else if (order == AVL_BREADTH_FIRST) {
		res = _foreach_breadth(&idx, tree->root, data, body);
	} else {
		return -1;
	}

	if (res >= 0) {
		return idx;
	}
	return res;
}