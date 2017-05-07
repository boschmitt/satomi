//===--- wec_ui32.h ---------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#ifndef SATOMI__UTILS__WEC__WEC_UI32_H
#define SATOMI__UTILS__WEC__WEC_UI32_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../mem.h"
#include "../vec/vec.h"


typedef struct wec_ui32_t_ wec_ui32_t;
struct wec_ui32_t_ {
	uint32_t cap;
	uint32_t size;
	vec_ui32_t* data;
};
//===------------------------------------------------------------------------===
// Vector Macros
//===------------------------------------------------------------------------===
#define wec_ui32_foreach(vec, entry, i) \
    for (i = 0; (i < wec_ui32_size(vec)) && (((entry) = wec_ui32_at(vec, i)), 1); i++)
//===----------------------------------------------------------------------===//
// Vector API
//===----------------------------------------------------------------------===//
static inline wec_ui32_t *
wec_ui32_alloc(uint32_t cap)
{
	wec_ui32_t *wec = STM_ALLOC(wec_ui32_t, 1);

	if (cap > 0 && cap < 16)
		cap = 16;
	wec->size = 0;
	wec->cap = cap;
	wec->data = wec->cap ? STM_CALLOC(vec_ui32_t, wec->cap) : NULL;
	return wec;
}

static inline wec_ui32_t *
wec_ui32_init(uint32_t cap, uint32_t size)
{
	wec_ui32_t *wec = wec_ui32_alloc(cap);
	wec->size = size;
	return wec;
}

static inline void 
wec_ui32_free(wec_ui32_t *wec)
{
	for (uint32_t i = 0; i < wec->size; i++)
		STM_FREE(wec->data[i].data);

	STM_FREE(wec->data);
	STM_FREE(wec);
}

static inline uint32_t 
wec_ui32_size(wec_ui32_t *wec) { return wec->size; }

static inline void
wec_ui32_resize(wec_ui32_t *wec, uint32_t new_size)
{
	wec->size = new_size;
	if (wec->cap >= new_size)
		return;
	wec->data = STM_REALLOC(vec_ui32_t, wec->data, new_size);
	memset(wec->data + wec->cap, 0,
	       sizeof(vec_ui32_t) * (new_size - wec->cap));
	assert(wec->data != NULL);
	wec->cap = new_size;
}

static inline void
wec_ui32_reserve(wec_ui32_t *wec, uint32_t new_cap)
{
	if (wec->cap >= new_cap)
		return;
	wec->data = STM_REALLOC(vec_ui32_t, wec->data, new_cap);
	assert(wec->data != NULL);
	memset(wec->data + wec->cap, 0,
	       sizeof(vec_ui32_t) * (new_cap - wec->cap));
	wec->cap = new_cap;
}

static inline uint32_t
wec_ui32_capacity(wec_ui32_t *wec) { return wec->cap; }

static inline int
wec_ui32_empty(wec_ui32_t *wec) { return wec->size ? 0 : 1; }

static inline void 
wec_ui32_erase(wec_ui32_t *wec)
{
	for (uint32_t i = 0; i < wec->size; i++)
		vec_erase(wec->data + i);
	STM_FREE(wec->data);
	wec->size = 0;
	wec->cap = 0;
}

static inline vec_ui32_t *
wec_ui32_at(wec_ui32_t *wec, uint32_t idx)
{
	assert(idx >= 0 && idx < wec->size);
	return wec->data + idx;
}

static inline vec_ui32_t *
wec_ui32_push_level(wec_ui32_t *wec)
{
	if (wec->size == wec->cap) {
		if (wec->cap < 16)
			wec_ui32_reserve(wec, 16);
		else
			wec_ui32_reserve(wec, 2 * wec->cap);
	}
	wec->size++;
	return wec_ui32_at(wec, wec->size - 1);
}

static inline void
wec_ui32_push_back(wec_ui32_t *wec, vec_ui32_t *value)
{
	if (wec->size == wec->cap) {
		if (wec->cap < 16)
			wec_ui32_reserve(wec, 16);
		else
			wec_ui32_reserve(wec, 2 * wec->cap);
	}
	vec_duplicate(wec->data + wec->size, value);
	wec->size++;
}

static inline void
wec_ui32_assign(wec_ui32_t *wec, uint32_t idx, vec_ui32_t *value)
{
	assert((idx >= 0) && (idx < wec_ui32_size(wec)));
	vec_duplicate(wec->data + idx, value);
}

static inline void
wec_ui32_drop(wec_ui32_t *wec, uint32_t idx)
{
	assert((idx >= 0) && (idx < wec_ui32_size(wec)));
	vec_erase(wec->data + idx);
	memmove(wec->data + idx, wec->data + idx + 1,
		(wec->size - idx - 1) * sizeof(vec_ui32_t));
	wec->size -= 1;
}

static inline void
wec_ui32_clear(wec_ui32_t *wec)
{
	for (uint32_t i = 0; i < wec->size; i++)
		vec_clear(wec->data + i);
	wec->size = 0;
}

static inline void 
wec_ui32_print(wec_ui32_t *wec)
{
	assert(wec != NULL);
	for (uint32_t i = 0; i < wec->size; i++) {
		vec_ui32_t *vec = wec->data + i;
		fprintf(stdout, "%4u: {", i);
		for (uint32_t j = 0; j < vec->size; j++)
			fprintf(stdout, " %u", vec->data[j]);
		fprintf(stdout, " }\n");
	}
}

#endif /* SATOMI__UTILS__WEC__WEC_UI32_H */

