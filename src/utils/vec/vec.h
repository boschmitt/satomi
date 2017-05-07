//===--- vec.h --------------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#ifndef SATOMI__UTILS__VEC__VEC_H
#define SATOMI__UTILS__VEC__VEC_H

#include "vec_i32.h"
#include "vec_ui8.h"
#include "vec_ui32.h"

#define vec_free(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_free,    \
	vec_ui8_t *: vec_ui8_free,    \
	vec_ui32_t *: vec_ui32_free   \
	)(vec)

#define vec_size(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_size,    \
	vec_ui8_t *: vec_ui8_size,    \
	vec_ui32_t *: vec_ui32_size   \
	)(vec)

#define vec_resize(vec, new_size) _Generic((vec), \
	vec_i32_t *: vec_i32_resize,    \
	vec_ui8_t *: vec_ui8_resize,    \
	vec_ui32_t *: vec_ui32_resize   \
	)(vec, new_size)

#define vec_shrink(vec, new_size) _Generic((vec), \
	vec_i32_t *: vec_i32_shrink,    \
	vec_ui8_t *: vec_ui8_shrink,    \
	vec_ui32_t *: vec_ui32_shrink   \
	)(vec, new_size)

#define vec_reserve(vec, new_cap) _Generic((vec), \
	vec_i32_t *: vec_i32_reserve,    \
	vec_ui8_t *: vec_ui8_reserve,    \
	vec_ui32_t *: vec_ui32_reserve   \
	)(vec, new_cap)

#define vec_capacity(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_capacity,    \
	vec_ui8_t *: vec_ui8_capacity,    \
	vec_ui32_t *: vec_ui32_capacity   \
	)(vec)

#define vec_empty(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_empty,    \
	vec_ui8_t *: vec_ui8_empty,    \
	vec_ui32_t *: vec_ui32_empty   \
	)(vec)

#define vec_erase(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_erase,    \
	vec_ui8_t *: vec_ui8_erase,    \
	vec_ui32_t *: vec_ui32_erase   \
	)(vec)

#define vec_at(vec, idx) _Generic((vec), \
	vec_i32_t *: vec_i32_at,    \
	vec_ui8_t *: vec_ui8_at,    \
	vec_ui32_t *: vec_ui32_at   \
	)(vec, idx)

#define vec_at_ptr(vec, idx) _Generic((vec), \
	vec_i32_t *: vec_i32_at_ptr,    \
	vec_ui8_t *: vec_ui8_at_ptr,    \
	vec_ui32_t *: vec_ui32_at_ptr   \
	)(vec, idx)

#define vec_data(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_data,    \
	vec_ui8_t *: vec_ui8_data,    \
	vec_ui32_t *: vec_ui32_data   \
	)(vec)

#define vec_find(vec, value) _Generic((vec), \
	vec_i32_t *: vec_i32_find,    \
	vec_ui8_t *: vec_ui8_find,    \
	vec_ui32_t *: vec_ui32_find   \
	)(vec, value)

#define vec_duplicate(dest, src) _Generic((dest), \
	vec_i32_t *: vec_i32_duplicate,    \
	vec_ui8_t *: vec_ui8_duplicate,    \
	vec_ui32_t *: vec_ui32_duplicate   \
	)(dest, src)

#define vec_copy(dest, src) _Generic((dest), \
	vec_i32_t *: vec_i32_copy,    \
	vec_ui8_t *: vec_ui8_copy,    \
	vec_ui32_t *: vec_ui32_copy   \
	)(dest, src)

#define vec_push_back(vec, value) _Generic((vec), \
	vec_i32_t *: vec_i32_push_back,    \
	vec_ui8_t *: vec_ui8_push_back,    \
	vec_ui32_t *: vec_ui32_push_back   \
	)(vec, value)

#define vec_pop_back(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_pop_back,    \
	vec_ui8_t *: vec_ui8_pop_back,    \
	vec_ui32_t *: vec_ui32_pop_back   \
	)(vec)

#define vec_assign(vec, idx, value) _Generic((vec), \
	vec_i32_t *: vec_i32_assign,    \
	vec_ui8_t *: vec_ui8_assign,    \
	vec_ui32_t *: vec_ui32_assign   \
	)(vec, idx, value)

#define vec_insert(vec, idx, value) _Generic((vec), \
	vec_i32_t *: vec_i32_insert,    \
	vec_ui8_t *: vec_ui8_insert,    \
	vec_ui32_t *: vec_ui32_insert   \
	)(vec, idx, value)

#define vec_drop(vec, idx) _Generic((vec), \
	vec_i32_t *: vec_i32_drop,    \
	vec_ui8_t *: vec_ui8_drop,    \
	vec_ui32_t *: vec_ui32_drop   \
	)(vec, idx)

#define vec_clear(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_clear,    \
	vec_ui8_t *: vec_ui8_clear,    \
	vec_ui32_t *: vec_ui32_clear   \
	)(vec)

#define vec_sort(vec, ascending) _Generic((vec), \
	vec_i32_t *: vec_i32_sort,    \
	vec_ui8_t *: vec_ui8_sort,    \
	vec_ui32_t *: vec_ui32_sort   \
	)(vec, ascending)

#define vec_memory(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_memory,    \
	vec_ui8_t *: vec_ui8_memory,    \
	vec_ui32_t *: vec_ui32_memory   \
	)(vec)

#define vec_print(vec) _Generic((vec), \
	vec_i32_t *: vec_i32_print,    \
	vec_ui8_t *: vec_ui8_print,    \
	vec_ui32_t *: vec_ui32_print   \
	)(vec)

#endif /* SATOMI__UTILS__VEC__VEC_H */
