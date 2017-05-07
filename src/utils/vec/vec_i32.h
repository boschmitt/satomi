//===--- vec_i32.h ---------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#ifndef SATOMI__UTILS__VEC__VEC_I32_H
#define SATOMI__UTILS__VEC__VEC_I32_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../mem.h"

typedef struct vec_i32_t_ vec_i32_t;
struct vec_i32_t_ {
	uint32_t cap;
	uint32_t size;
	int32_t* data;
};

//===------------------------------------------------------------------------===
// Vector Macros
//===------------------------------------------------------------------------===
#define vec_i32_foreach(vec, entry, i) \
    for (i = 0; (i < vec_i32_size(vec)) && (((entry) = vec_i32_at(vec, i)), 1); i++)

#define vec_i32_foreach_start(vec, entry, i, start) \
    for (i = start; (i < vec_i32_size(vec)) && (((entry) = vec_i32_at(vec, i)), 1); i++)

#define vec_i32_foreach_stop(vec, entry, i, stop) \
    for (i = 0; (i < stop) && (((entry) = vec_i32_at(vec, i)), 1); i++)

//===------------------------------------------------------------------------===
// Vector API
//===------------------------------------------------------------------------===
static inline vec_i32_t *
vec_i32_alloc(uint32_t cap)
{
	vec_i32_t *vec = STM_ALLOC(vec_i32_t, 1);

	if (cap > 0 && cap < 16)
		cap = 16;
	vec->size = 0;
	vec->cap = cap;
	vec->data = vec->cap ? STM_ALLOC(int32_t, vec->cap) : NULL;
	return vec;
}

static inline vec_i32_t *
vec_i32_alloc_exact(uint32_t cap)
{
	vec_i32_t *vec = STM_ALLOC(vec_i32_t, 1);

	cap = 0;
	vec->size = 0;
	vec->cap = cap;
	vec->data = vec->cap ? STM_ALLOC(int32_t, vec->cap ) : NULL;
	return vec;
}

static inline vec_i32_t *
vec_i32_init(uint32_t size, int32_t value)
{
	vec_i32_t *vec = STM_ALLOC(vec_i32_t, 1);

	vec->cap = size;
	vec->size = size;
	vec->data = vec->cap ? STM_ALLOC(int32_t, vec->cap) : NULL;
	memset(vec->data, value, sizeof(int32_t) * vec->size);
	return vec;
}

static inline void
vec_i32_free(vec_i32_t *vec)
{
	if (vec->data != NULL)
		STM_FREE(vec->data);
	STM_FREE(vec);
}

static inline uint32_t
vec_i32_size(vec_i32_t *vec)
{
	return vec->size;
}

static inline void
vec_i32_resize(vec_i32_t *vec, uint32_t new_size)
{
	vec->size = new_size;
	if (vec->cap >= new_size)
		return;
	vec->data = STM_REALLOC(int32_t, vec->data, new_size);
	assert(vec->data != NULL);
	vec->cap = new_size;
}

static inline void
vec_i32_shrink(vec_i32_t *vec, uint32_t new_size)
{
	assert(vec->cap >= new_size);
	vec->size = new_size;
}

static inline void
vec_i32_reserve(vec_i32_t *vec, uint32_t new_cap)
{
	if (vec->cap >= new_cap)
		return;
	vec->data = STM_REALLOC(int32_t, vec->data, new_cap);
	assert(vec->data != NULL);
	vec->cap = new_cap;
}

static inline uint32_t
vec_i32_capacity(vec_i32_t *vec)
{
	return vec->cap;
}

static inline int
vec_i32_empty(vec_i32_t *vec)
{
	return vec->size ? 0 : 1;
}

static inline void
vec_i32_erase(vec_i32_t *vec)
{
	STM_FREE(vec->data);
	vec->size = 0;
	vec->cap = 0;
}

static inline int32_t
vec_i32_at(vec_i32_t *vec, uint32_t idx)
{
	assert(idx >= 0 && idx < vec->size);
	return vec->data[idx];
}

static inline int32_t *
vec_i32_at_ptr(vec_i32_t *vec, uint32_t idx)
{
	assert(idx >= 0 && idx < vec->size);
	return vec->data + idx;
}

static inline int32_t
vec_i32_find(vec_i32_t *vec, int32_t entry)
{
	for (uint32_t i = 0; i < vec->size; i++)
		if (vec->data[i] == entry)
			return 1;
	return 0;
}

static inline int32_t *
vec_i32_data(vec_i32_t *vec)
{
	assert(vec);
	return vec->data;
}

static inline void
vec_i32_duplicate(vec_i32_t *dest, const vec_i32_t *src)
{
	assert(dest != NULL && src != NULL);
	vec_i32_resize(dest, src->cap);
	memcpy(dest->data, src->data, sizeof(int32_t) * src->cap);
	dest->size = src->size;
}

static inline void
vec_i32_copy(vec_i32_t *dest, const vec_i32_t *src)
{
	assert(dest != NULL && src != NULL);
	vec_i32_resize(dest, src->size);
	memcpy(dest->data, src->data, sizeof(int32_t) * src->size);
	dest->size = src->size;
}

static inline void
vec_i32_push_back(vec_i32_t *vec, int32_t value)
{
	if (vec->size == vec->cap) {
		if (vec->cap < 16)
			vec_i32_reserve(vec, 16);
		else
			vec_i32_reserve(vec, 2 * vec->cap);
	}
	vec->data[vec->size] = value;
	vec->size++;
}

static inline int32_t
vec_i32_pop_back(vec_i32_t *vec)
{
	assert(vec && vec->size);
	return vec->data[--vec->size];
}

static inline void
vec_i32_assign(vec_i32_t *vec, uint32_t idx, int32_t value)
{
	assert((idx >= 0) && (idx < vec_i32_size(vec)));
	vec->data[idx] = value;
}

static inline void
vec_i32_insert(vec_i32_t *vec, uint32_t idx, int32_t value)
{
	assert((idx >= 0) && (idx < vec_i32_size(vec)));
	vec_i32_push_back(vec, 0);
	memmove(vec->data + idx + 1, vec->data + idx, (vec->size - idx - 2) * sizeof(int32_t));
	vec->data[idx] = value;
}

static inline void
vec_i32_drop(vec_i32_t *vec, uint32_t idx)
{
	assert((idx >= 0) && (idx < vec_i32_size(vec)));
	memmove(vec->data + idx, vec->data + idx + 1, (vec->size - idx - 1) * sizeof(int32_t));
	vec->size -= 1;
}

static inline void
vec_i32_clear(vec_i32_t *vec)
{
	vec->size = 0;
}

static inline int
vec_i32_asc_compare(const void *p1, const void *p2)
{
	const int32_t *pp1 = (const int32_t *) p1;
	const int32_t *pp2 = (const int32_t *) p2;

	if ( *pp1 < *pp2 )
		return -1;
	if ( *pp1 > *pp2 )
		return 1;
	return 0;
}

static inline int
vec_i32_desc_compare(const void *p1, const void *p2)
{
	const int32_t *pp1 = (const int32_t *) p1;
	const int32_t *pp2 = (const int32_t *) p2;

	if ( *pp1 > *pp2 )
		return -1;
	if ( *pp1 < *pp2 )
		return 1;
	return 0;
}

static inline void
vec_i32_sort(vec_i32_t *vec, int ascending)
{
	if (ascending)
		qsort((void *) vec->data, vec->size, sizeof(int32_t),
		      (int (*)(const void *, const void *)) vec_i32_asc_compare);
	else
		qsort((void*) vec->data, vec->size, sizeof(int32_t),
		      (int (*)(const void *, const void *)) vec_i32_desc_compare);
}

static inline uint32_t
vec_i32_memory(vec_i32_t *vec)
{
	return vec == NULL ? 0 : sizeof(int32_t) * vec->cap + sizeof(vec_i32_t);
}

static inline void
vec_i32_print(vec_i32_t* vec)
{
	assert(vec != NULL);
	fprintf(stdout, "Vector has %u(%u) entries: {", vec->size, vec->cap);
	for (uint32_t i = 0; i < vec->size; i++)
		fprintf(stdout, " %d", vec->data[i]);
	fprintf(stdout, " }\n");
}

#endif /* SATOMI__UTILS__VEC__VEC_I32_H */
