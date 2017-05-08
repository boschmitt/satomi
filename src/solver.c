//===--- solver.c -----------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "clause.h"
#include "solver.h"
#include "watch_list.h"
#include "utils/mem.h"

//===------------------------------------------------------------------------===
// Solver internal functions
//===------------------------------------------------------------------------===
static inline uint32_t
solver_decide(solver_t *s)
{
	uint32_t next_var = UNDEF;

	while (next_var == UNDEF || var_value(s, next_var) != VAR_UNASSING) {
		if (vec_size(s->var_order) == 0) {
			next_var = UNDEF;
			return UNDEF;
		}
		next_var = vec_at(s->var_order, 0);
		vec_drop(s->var_order, 0);
	}
	return var2lit(next_var, LIT_FALSE);
}

static inline void
solver_new_decision(solver_t *s, uint32_t lit)
{
	assert(var_value(s, lit2var(lit)) == VAR_UNASSING);
	vec_push_back(s->trail_lim, vec_size(s->trail));
	solver_enqueue(s, lit);
}

/** 
 *
 */
static inline void 
solver_backtrack(solver_t *s)
{
	uint32_t level = solver_dlevel(s) - 1;
	for (uint32_t i = vec_size(s->trail); i-- > vec_at(s->trail_lim, level); ) {
		uint32_t var = lit2var(vec_at(s->trail, i));

		vec_assign(s->assigns, var, VAR_UNASSING);
		vec_push_back(s->var_order, var);
	}
	vec_sort(s->var_order, 1);
	s->i_qhead = vec_at(s->trail_lim, level);
	vec_shrink(s->trail, vec_at(s->trail_lim, level));
	vec_shrink(s->trail_lim, level);
}

//===------------------------------------------------------------------------===
// Solver external functions
//===------------------------------------------------------------------------===
/** 
 *  Creating a clause involves: calculating its size as a number of 32-bits
 *  words (n_words) and asking the clause database to make space for it. The
 *  database will return a reference, an index, to where the clause begins (cref) 
 *  with which we can obtain a clause handler to access and modify its info.
 *
 *  As we add literals to the clause, we must also add the clause to the
 *  ocurrence list of the literals. 
 */
uint32_t
solver_clause_create(solver_t *s, vec_ui32_t *lits)
{
	struct clause *clause;
	uint32_t cref;
	uint32_t n_words;

	assert(vec_size(lits) > 1);

	n_words = 1 + vec_size(lits);
	cref = cdb_append(s->clause_db, n_words);
	clause = clause_read(s, cref);
	clause->size = vec_size(lits);
	memcpy(&(clause->lits[0]), vec_data(lits), sizeof(uint32_t) * vec_size(lits));

	vec_push_back(s->clauses, cref);
	s->stats.n_lits += vec_size(lits);
	return cref;
}

uint32_t
solver_propagate(solver_t *s)
{
	uint32_t conf_cref = UNDEF;
	uint32_t n_propagations = 0;

	while (s->i_qhead < vec_size(s->trail)) {
		uint32_t lit = vec_at(s->trail, s->i_qhead++);
		uint32_t neg_lit;
		uint32_t *lits;
		struct watch_list *ws;
		struct watcher *begin;
		struct watcher *end;
		struct watcher *i, *j;

		n_propagations++;
		watch_list_foreach_bin(s->watches, i, lit) {
			if (var_value(s, lit2var(i->blocker)) == VAR_UNASSING)
				solver_enqueue(s, i->blocker);
			else if (lit_value(s, i->blocker) == LIT_FALSE)
				return i->cref;
		}

		ws = vec_wl_at(s->watches, lit);
		begin = watch_list_array(ws);
		end = begin + watch_list_size(ws);
		for (i = j = begin + ws->n_bin; i < end;) {
			struct clause *clause;
			struct watcher w;

			if (lit_value(s, i->blocker) == LIT_TRUE) {
				*j++ = *i++;
				continue;
			}

			clause = clause_read(s, i->cref);
			lits = &(clause->lits[0]);

			// Make sure the false literal is data[1]:
			neg_lit = lit_neg(lit);
			if (lits[0] == neg_lit)
				STM_SWAP(uint32_t, lits[0], lits[1]);
			assert(lits[1] == neg_lit);

			w.cref = i->cref;
			w.blocker = lits[0];

			/* If 0th watch is true, then clause is already satisfied. */
			if (lits[0] != i->blocker && lit_value(s, lits[0]) == LIT_TRUE)
				*j++ = w;
			else {
				/* Look for new watch */
				for (uint32_t k = 2; k < clause->size; k++) {
					if (lit_value(s, lits[k]) != LIT_FALSE) {
						lits[1] = lits[k];
						lits[k] = neg_lit;
						watch_list_push(vec_wl_at(s->watches, lit_neg(lits[1])), w, 0);
						goto next;
					}
				}

				*j++ = w;

				/* Clause becomes unit under this assignment */
				if (lit_value(s, lits[0]) == LIT_FALSE) {
					conf_cref = i->cref;
					s->i_qhead = vec_size(s->trail);
					i++;
					// Copy the remaining watches:
					while (i < end)
						*j++ = *i++;
				} else
					solver_enqueue(s, lits[0]);
			}
		next:
			i++;
		}

		s->stats.n_inspects += j - watch_list_array(ws);
		watch_list_shrink(ws, j - watch_list_array(ws));
	}
	s->stats.n_propagations += n_propagations;
	return conf_cref;
}

/**
 *
 */
int
solver_search(solver_t *s)
{
	while (1) {
		uint32_t confl_cref = solver_propagate(s);
		uint32_t next_lit;
		if (confl_cref != UNDEF) {
			s->stats.n_conflicts++;
			if (solver_dlevel(s) == 0)
				return SATOMI_UNSAT;
			next_lit = solver_last_decision(s);
			solver_backtrack(s);
			solver_enqueue(s, lit_neg(next_lit));
		} else {
			s->stats.n_decisions++;
			next_lit = solver_decide(s);
			if (next_lit == UNDEF)
				return SATOMI_SAT;
			solver_new_decision(s, next_lit);
		}
	}
}
