#include "vec.h"

#include <stdlib.h>

#include "../errstack.h"

struct vec_s
{
	size_t capacity;
	size_t size;
	size_t elm_size;
	void *data;
};

int vec_alloc(vec_t **vec, size_t elm_size)
{
	vec_t *tmp;
	*vec = NULL;
	ES_NEW_ASRT_NM(tmp = calloc(1, sizeof(vec_t)));
	ES_NEW_ASRT_NM(tmp->data = calloc(1, elm_size));
	tmp->elm_size = elm_size;
	tmp->capacity = 1;
	tmp->size     = 0;
	*vec          = tmp;
	return 1;
}

void vec_cleanup(vec_t **vec)
{
	vec_t *tmp = *vec;
	if (tmp) {
		if (tmp->data)
			free(tmp->data);
		free(tmp);
	}
	*vec = NULL;
}

#define _upsize_check()                                                                            \
	({                                                                                             \
		if (vec->capacity == vec->size) {                                                          \
			size_t new_cap = vec->capacity * 2;                                                    \
			void *tmp      = realloc(vec->data, new_cap * vec->elm_size);                          \
			ES_NEW_ASRT_NM(tmp);                                                                   \
			vec->data     = tmp;                                                                   \
			vec->capacity = new_cap;                                                               \
		}                                                                                          \
	})

int vec_FWD_back(vec_t *vec, const void *data)
{
	_upsize_check();
	memcpy(vec->data + vec->elm_size * vec->size, data, vec->elm_size);
	vec->size++;
	return 1;
}

int vec_emplace_back(vec_t *vec, void **data)
{
	*data = NULL;
	_upsize_check();
	*data = vec->data + vec->elm_size * vec->size;
	vec->size++;
	return 1;
}

int vec_pop_back(vec_t *vec)
{
	if (vec->capacity / 4 > vec->size) {
		size_t new_cap = vec->capacity / 2;
		void *tmp      = realloc(vec->data, new_cap * vec->elm_size);
		ES_NEW_ASRT_NM(tmp);
		vec->data     = tmp;
		vec->capacity = new_cap;
	}
	vec->size--;
	return 1;
}

void *vec_back(vec_t *vec)
{
	if (vec->size == 0)
		return NULL;
	return vec->data + vec->elm_size * (vec->size - 1);
}

void *vec_front(vec_t *vec)
{
	if (vec->size == 0)
		return NULL;
	return vec->data;
}

void *vec_at(vec_t *vec, size_t idx)
{
	if (vec->size == 0 || idx >= vec->size)
		return NULL;
	return vec->data + vec->elm_size * (idx);
}

size_t vec_size(vec_t *vec)
{
	return vec->size;
}

int vec_foreach(vec_t *vec,
                void *arg_vp,
                int (*each)(const vec_t *vec, size_t idx, void *data, void *arg_vp))
{
	size_t i;

	for (i = 0; i < vec_size(vec); i++) {
		int ret = 0;
		ES_FWD_INT_NM(ret = each(vec, i, vec_at(vec, i), arg_vp));
		if (ret == 0) {
			return 0;
		}
	}
	return 1;
}