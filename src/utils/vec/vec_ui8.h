//===--- vec_ui8.h ----------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#ifndef SATOMI__UTILS__VEC__VEC_UI8_H
#define SATOMI__UTILS__VEC__VEC_UI8_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../mem.h"

typedef struct vec_ui8_t_ vec_ui8_t;
struct vec_ui8_t_ {
	uint32_t cap;
	uint32_t size;
	uint8_t* data;
};

//===------------------------------------------------------------------------===
// Vector Macros
//===------------------------------------------------------------------------===
#define vec_ui8_foreach(vec, entry, i) \
    for (i = 0; (i < vec_ui8_size(vec)) && (((entry) = vec_ui8_at(vec, i)), 1); i++)

#define vec_ui8_foreach_start(vec, entry, i, start) \
    for (i = start; (i < vec_ui8_size(vec)) && (((entry) = vec_ui8_at(vec, i)), 1); i++)

#define vec_ui8_foreach_stop(vec, entry, i, stop) \
    for (i = 0; (i < stop) && (((entry) = vec_ui8_at(vec, i)), 1); i++)

//===------------------------------------------------------------------------===
// Vector API
//===------------------------------------------------------------------------===
static inline vec_ui8_t *
vec_ui8_alloc(uint32_t cap)
{
	vec_ui8_t *vec = STM_ALLOC(vec_ui8_t, 1);

	if (cap > 0 && cap < 16)
		cap = 16;
	vec->size = 0;
	vec->cap = cap;
	vec->data = vec->cap ? STM_ALLOC(uint8_t, vec->cap) : NULL;
	return vec;
}

static inline vec_ui8_t *
vec_ui8_alloc_exact(uint32_t cap)
{
	vec_ui8_t *vec = STM_ALLOC(vec_ui8_t, 1);

	cap = 0;
	vec->size = 0;
	vec->cap = cap;
	vec->data = vec->cap ? STM_ALLOC(uint8_t, vec->cap ) : NULL;
	return vec;
}

static inline vec_ui8_t *
vec_ui8_init(uint32_t size, uint8_t value)
{
	vec_ui8_t *vec = STM_ALLOC(vec_ui8_t, 1);

	vec->cap = size;
	vec->size = size;
	vec->data = vec->cap ? STM_ALLOC(uint8_t, vec->cap) : NULL;
	memset(vec->data, value, sizeof(uint8_t) * vec->size);
	return vec;
}

static inline void
vec_ui8_free(vec_ui8_t *vec)
{
	if (vec->data != NULL)
		STM_FREE(vec->data);
	STM_FREE(vec);
}

static inline uint32_t
vec_ui8_size(vec_ui8_t *vec) { return vec->size; }

static inline void
vec_ui8_resize(vec_ui8_t *vec, uint32_t new_size)
{
	vec->size = new_size;
	if (vec->cap >= new_size)
		return;
	vec->data = STM_REALLOC(uint8_t, vec->data, new_size);
	assert(vec->data != NULL);
	vec->cap = new_size;
}

static inline void
vec_ui8_shrink(vec_ui8_t *vec, uint32_t new_size)
{
	assert(vec->cap >= new_size);
	vec->size = new_size;
}

static inline void
vec_ui8_reserve(vec_ui8_t *vec, uint32_t new_cap)
{
	if (vec->cap >= new_cap)
		return;
	vec->data = STM_REALLOC(uint8_t, vec->data, new_cap);
	assert(vec->data != NULL);
	vec->cap = new_cap;
}

static inline uint32_t
vec_ui8_capacity(vec_ui8_t *vec) { return vec->cap; }

static inline int
vec_ui8_empty(vec_ui8_t *vec) { return vec->size ? 0 : 1; }

static inline void
vec_ui8_erase(vec_ui8_t *vec)
{
	STM_FREE(vec->data);
	vec->size = 0;
	vec->cap = 0;
}

static inline uint8_t
vec_ui8_at(vec_ui8_t *vec, uint32_t idx)
{
	assert(idx >= 0 && idx < vec->size);
	return vec->data[idx];
}

static inline uint8_t *
vec_ui8_at_ptr(vec_ui8_t *vec, uint32_t idx)
{
	assert(idx >= 0 && idx < vec->size);
	return vec->data + idx;
}

static inline uint8_t
vec_ui8_find(vec_ui8_t *vec, uint8_t entry)
{
	for (uint32_t i = 0; i < vec->size; i++)
		if (vec->data[i] == entry)
			return 1;
	return 0;
}

static inline uint8_t *
vec_ui8_data(vec_ui8_t *vec)
{
	assert(vec);
	return vec->data;
}

static inline void
vec_ui8_duplicate(vec_ui8_t *dest, const vec_ui8_t *src)
{
	assert(dest != NULL && src != NULL);
	vec_ui8_resize(dest, src->cap);
	memcpy(dest->data, src->data, sizeof(uint8_t) * src->cap);
	dest->size = src->size;
}

static inline void
vec_ui8_copy(vec_ui8_t *dest, const vec_ui8_t *src)
{
	assert(dest != NULL && src != NULL);
	vec_ui8_resize(dest, src->size);
	memcpy(dest->data, src->data, sizeof(uint8_t) * src->size);
	dest->size = src->size;
}

static inline void
vec_ui8_push_back(vec_ui8_t *vec, uint8_t value)
{
	if (vec->size == vec->cap) {
		if (vec->cap < 16)
			vec_ui8_reserve(vec, 16);
		else
			vec_ui8_reserve(vec, 2 * vec->cap);
	}
	vec->data[vec->size] = value;
	vec->size++;
}

static inline uint8_t
vec_ui8_pop_back(vec_ui8_t *vec)
{
	assert(vec && vec->size);
	return vec->data[--vec->size];
}

static inline void
vec_ui8_assign(vec_ui8_t *vec, uint32_t idx, uint8_t value)
{
	assert((idx >= 0) && (idx < vec_ui8_size(vec)));
	vec->data[idx] = value;
}

static inline void
vec_ui8_insert(vec_ui8_t *vec, uint32_t idx, uint8_t value)
{
	assert((idx >= 0) && (idx < vec_ui8_size(vec)));
	vec_ui8_push_back(vec, 0);
	memmove(vec->data + idx + 1, vec->data + idx, (vec->size - idx - 2) * sizeof(uint8_t));
	vec->data[idx] = value;
}

static inline void
vec_ui8_drop(vec_ui8_t *vec, uint32_t idx)
{
	assert((idx >= 0) && (idx < vec_ui8_size(vec)));
	memmove(vec->data + idx, vec->data + idx + 1, (vec->size - idx - 1) * sizeof(uint8_t));
	vec->size -= 1;
}

static inline void
vec_ui8_clear(vec_ui8_t *vec)
{
	vec->size = 0;
}

static inline int
vec_ui8_asc_compare(const void *p1, const void *p2)
{
	const uint8_t *pp1 = (const uint8_t *) p1;
	const uint8_t *pp2 = (const uint8_t *) p2;

	if ( *pp1 < *pp2 )
		return -1;
	if ( *pp1 > *pp2 )
		return 1;
	return 0;
}

static inline int
vec_ui8_desc_compare(const void *p1, const void *p2)
{
	const uint8_t *pp1 = (const uint8_t *) p1;
	const uint8_t *pp2 = (const uint8_t *) p2;

	if ( *pp1 > *pp2 )
		return -1;
	if ( *pp1 < *pp2 )
		return 1;
	return 0;
}

static inline void
vec_ui8_sort(vec_ui8_t *vec, int ascending)
{
	if (ascending)
		qsort((void *) vec->data, vec->size, sizeof(uint8_t),
		      (int (*)(const void *, const void *)) vec_ui8_asc_compare);
	else
		qsort((void*) vec->data, vec->size, sizeof(uint8_t),
		      (int (*)(const void *, const void *)) vec_ui8_desc_compare);
}

static inline uint32_t
vec_ui8_memory(vec_ui8_t *vec)
{
	return vec == NULL ? 0 : sizeof(uint8_t) * vec->cap + sizeof(vec_ui8_t);
}

static inline void
vec_ui8_print(vec_ui8_t* vec)
{
	assert(vec != NULL);
	fprintf(stdout, "Vector has %u(%u) entries: {", vec->size, vec->cap);
	for (uint32_t i = 0; i < vec->size; i++)
		fprintf(stdout, " %u", vec->data[i]);
	fprintf(stdout, " }\n");
}

#endif /* SATOMI__UTILS__VEC__VEC_UI8_H */
