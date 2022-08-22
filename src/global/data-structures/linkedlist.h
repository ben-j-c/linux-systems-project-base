#pragma once
/**
 * @file linkedlist.h
 * @author Benjamin Correia (ben-j-c@github)
 * @brief A simple doubly linked list
 * @version 0.1
 * @date 2022-08-21
 *
 * @copyright Copyright (c) 2022 Benjamin Correia. All rights reserved.
 * @license This file is MIT licensed
 */
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief This struct should be embedded into your struct.
 */
typedef struct llist_s
{
	llist_st *next;
	llist_st *prev;
} llist_st;

/**
 * @brief Initialize a linked list header node. A header node is optional, though foreach and
 * foreach_reverse are implemented assuming that a header node exists. If you dont want a header
 * node, then still use this function, just when you create the first node.
 *
 * @param dst The node to initalize
 */
void ll_init(llist_st *dst);

/**
 * @brief For the given node, push src behind this node. In the case of the dst == header node, this
 * results in src being put at the end.
 *
 * @param dst The destination list
 * @param src The nodes to be moved
 */
void ll_push_back(llist_st *dst, llist_st *src);
/**
 * @brief For the given node, push src in front of this node.
 *
 * @param dst The destination list
 * @param src The nodes to be moved
 */
void ll_push_front(llist_st **dst, llist_st *src);
/**
 * @brief Check if this header node has any data nodes.
 *
 * @param src The list to check
 * @return true if there are no nodes
 * @return false if there are nodes
 */
bool ll_is_empty(llist_st *src);

/**
 * @brief Iterate over the nodes in this list
 *
 * @param list The list to iterate
 * @param state A pointer to be passed through to the body
 * @param body A callback function executed for every node. A return >1 success, =0 success and
 * break, <0 failure and break
 * @returns The number of nodes processed through the callback function, or a negative value if a
 * callback returned that
 */
int ll_foreach(llist_st *list,
               void *state,
               int (*body)(llist_st *list, llist_st *node, size_t idx, void *state));

/**
 * @brief Iterate over the nodes in this list in reverse
 *
 * @param list The list to iterate
 * @param state A pointer to be passed through to the body
 * @param body A callback function executed for every node. A return >1 success, =0 success and
 * break, <0 failure and break
 * @returns The number of nodes processed through the callback function, or a negative value if a
 * callback returned that
 */
int ll_foreach_reverse(llist_st *list,
                       void *state,
                       int (*body)(llist_st *list, llist_st *node, size_t idx, void *state));

/**
 * @brief Find the first node that satisfies a given predicate
 *
 * @param list The list to search
 * @param state A user data pointer
 * @param pred The predicate to test
 * @return The first node that satisfies the predicate, or NULL if none
 */
llist_st *ll_find(llist_st *list,
                  void *state,
                  bool (*pred)(llist_st *node, size_t idx, void *state));

/**
 * @brief Get a list node given an index from the start
 *
 * @param list The list of concern
 * @param idx The index desired
 * @returns The node at the index % (size + 1). If index == size, then the header node is returned
 */
llist_st *ll_get(llist_st *list, size_t idx);

/**
 * @brief Get a list node given an index from the end
 *
 * @param list The list of concern
 * @param idx The index desired
 * @return The node at the index % (size + 1). If index == size, then the header node is returned
 */
llist_st *ll_get_reverse(llist_st *list, size_t idx);

/**
 * @brief Remove a node from the list it's present in. If the node is the header node, then nothing
 * happens
 *
 * @param to_remove The node to be removed
 */
void ll_remove(llist_st *to_remove);
