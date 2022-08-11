#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "../util.h"

struct ht_s;
typedef struct ht_s ht_st;

typedef size_t (*ht_hash_func_t)(void *key);
typedef int64_t (*ht_cmp_func_t)(void *key1, void *key2);
typedef int (*ht_alloc_func_t)(void **dst, void *data);
typedef void (*ht_free_func_t)(void *kv);
typedef int (*ht_foreach_func_t)(const ht_st *ht, void *key, void *value, void *data);

/**
 * Allocate a new table.
 *
 * @param dst Where to store pointer to allocated table
 * @param hash How to calculate the hash of a key
 * @param cmp How to check equality of keys
 * @param key_size Size of a key to be allocated (0 means no allocation, ptr value is copied)
 * @param key_copy How to copy data when setting a new k/v pair (if exists, key_size is ignored)
 * @param key_free How to free fresh copies of the key (always called if exists)
 * @param value_size Size of value to be allocated (0 means no allocation is made, ptr value is
 * copied)
 * @param value_copy How to copy data when setting a new k/v pair (if exists, value_size is ignored)
 * @param value_free How to free fresh copies of the value (always called if exists)
 *
 * @returns negative on failure, 0 or positive on success
 */
int ht_alloc(ht_st **dst,
             ht_hash_func_t hash,
             ht_cmp_func_t cmp,
             size_t key_size,
             ht_alloc_func_t key_copy,
             ht_free_func_t key_free,
             size_t value_size,
             ht_alloc_func_t value_copy,
             ht_free_func_t value_free);
void ht_free(ht_st **to_free);
void ht_purge(ht_st *ht);

int ht_set(ht_st *ht, void *key, void *value);
void *ht_get(ht_st *ht, void *key);
WARN_UNUSED void *ht_take(ht_st *ht, void *key);
void ht_delete(ht_st *ht, void *key);
int ht_foreach(ht_st *ht, ht_foreach_func_t body, void *data);

size_t ht_buckets(ht_st *ht);
size_t ht_size(ht_st *ht);
double ht_density(ht_st *ht);

size_t ht_int_hash(void *key);
int64_t ht_int_cmp(void *key1, void *key2);

size_t ht_str_hash(const char *key);
int64_t ht_str_cmp(const char *key1, const char *key2);
int ht_str_copy(void **dst, void *key);
void ht_str_free(void *key);

size_t ht_arb_hash_util(void *key, size_t size);
int64_t ht_arb_cmp_util(void *key1, void *key2, size_t size);
/* Allocate an int -> user defined data hash table */
#define ht_int_alloc(dst, value_size, value_copy, value_free)                                      \
	ht_alloc(dst, ht_int_hash, ht_int_cmp, 0, NULL, NULL, value_size, value_copy, value_free)
#define ht_int_set(ht, key, value) ht_set(ht, (void *) (uint64_t) key, value)
#define ht_int_get(ht, key)        ht_get(ht, (void *) (uint64_t) key)
#define ht_int_delete(ht, key)     ht_delete(ht, (void *) (uint64_t) key)
/* Allocate a string -> user defined data hash table. Copies via strdup */
#define ht_str_alloc(dst, value_size, value_copy, value_free)                                      \
	ht_alloc(dst,                                                                                  \
	         ht_str_hash,                                                                          \
	         ht_str_cmp,                                                                           \
	         0,                                                                                    \
	         ht_str_copy,                                                                          \
	         ht_str_free,                                                                          \
	         value_size,                                                                           \
	         value_copy,                                                                           \
	         value_free)
#define ht_str_set(ht, key, value) ht_set(ht, (void *) key, value)
#define ht_str_get(ht, key)        ht_get(ht, (void *) key)

#define HT_CLEANUP CLEANUP(ht_free)
