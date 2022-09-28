#pragma once
/**
 * @file avl.h
 * @author Benjamin Correia (ben-j-c@github)
 * @brief An avl tree implementation featuring various allocation schemes
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) 2022 Benjamin Correia. All rights reserved.
 * @license This file is MIT licensed
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

struct avl_node_s;
/**
 * @brief Place this struct in your struct.
 *
 */
typedef struct avl_node_s avl_node_st;
typedef struct avl_node_s avl_node_t;
struct avl_node_s
{
	avl_node_st *child[2];
	int32_t balance;
};

struct avl_s;
typedef struct avl_s avl_st;
typedef struct avl_s avl_t;

typedef int (*avl_cmp_ft)(const avl_node_st *a, const avl_node_st *b);
typedef int (*avl_iter_ft)(const avl_node_st *cur, size_t idx, void *data);

int avl_alloc(avl_st **dst, avl_cmp_ft cmp);
void avl_add(avl_st *tree, avl_node_st *node);
avl_node_st *avl_del(avl_st *tree, avl_node_st *to_remove);
avl_node_st *avl_find_eq(const avl_st *tree, const avl_node_st *node);
avl_node_st *avl_min(const avl_st *tree);
avl_node_st *avl_max(const avl_st *tree);

enum avl_traversal_order_e
{
	AVL_IN_ORDER,
	AVL_PRE_ORDER,
	AVL_POST_ORDER,
	AVL_IN_ORDER_REVERSE,
	AVL_PRE_ORDER_REVERSE,
	AVL_POST_ORDER_REVERSE,
	AVL_BREADTH_FIRST,
};
ssize_t avl_foreach(avl_st *tree, void *data, avl_iter_ft body, enum avl_traversal_order_e order);
