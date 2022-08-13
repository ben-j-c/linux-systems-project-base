#pragma once
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is an implementation for an auto-resized array.
 */

#include <unistd.h>

#include "../util.h"

struct vec_s;
typedef struct vec_s vec_t;

#define VEC_CLEANUP __attribute__((cleanup(vec_cleanup)))

int vec_alloc(vec_t **vec, size_t elm_size);
void vec_cleanup(vec_t **vec);
int vec_push_back(vec_t *vec, const void *data);
int vec_emplace_back(vec_t *vec, void **data);
int vec_pop_back(vec_t *vec);
void *vec_back(vec_t *vec);
void *vec_front(vec_t *vec);
void *vec_at(vec_t *vec, size_t idx);
size_t vec_size(vec_t *vec);
int vec_foreach(vec_t *vec,
                void *arg_vp,
                int (*each)(const vec_t *vec, size_t idx, void *data, void *arg_vp));
void *vec_take_data(vec_t *vec, size_t *size, size_t *capacity);
