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
#include "utils/mem.h"

//===------------------------------------------------------------------------===
// Solver internal functions
//===------------------------------------------------------------------------===
/** 
 * This functions assumes that variables are being assinged in an ascending
 * order and clause literals are ordered in a descending order.
 *
 * The propagation of a literal begins by removing all occurences of it's 
 * negated form from the remaining set of clauses. If in this process an empty
 * clause is created, then we need to re-evaluate our assigments. The function
 * returns 0, which singals a conflict(problem) to the search procedure.
 * 
 * After that, all clauses satisfied by this assignment are removed from the 
 * occurence lists of all variables that have not been assigned yet. (Removes 
 * satisfied clauses from the set of remaining clauses).
 */
uint32_t
solver_propagate(solver_t *s, uint32_t lit)
{
	uint32_t i, cref;
	uint32_t neg_lit = lit_neg(lit); 
	vec_ui32_t *occ_list;
	
	s->n_nodes++;

	occ_list = wec_ui32_at(s->occ_lists, neg_lit);
	vec_ui32_foreach(occ_list, cref, i) {
		struct clause *c = clause_read(s, cref);

		if (c->size == 1) {
			uint32_t k;
			vec_ui32_foreach_stop(occ_list, cref, k, i) {
				c = clause_read(s, cref);
				c->size++;
			}
			return 0;
		}
		c->size--;
	}
	occ_list = wec_ui32_at(s->occ_lists, lit);
	vec_ui32_foreach(occ_list, cref, i) {
		struct clause *c = clause_read(s, cref);
		for (uint32_t j = 0; j < (c->size - 1); j++) {
			uint32_t idx = vec_find(wec_ui32_at(s->occ_lists, c->lits[j]), cref);
			vec_drop(wec_ui32_at(s->occ_lists, c->lits[j]), idx);
		}
		s->n_act_cls--;
	}
	return 1;
}

/** 
 * This functions assumes that variables are being assinged in an ascending
 * order and clause literals are ordered in a descending order.
 *
 * The backtraking of a literal begins by adding back all occurences of it's 
 * negated form to the remaining set of clauses. 
 * 
 * After that, all clauses which were satisfied by it are added back to the 
 * occurence lists of all variables that have not been assigned yet. (Adds
 * previously satisfied clauses back the set of remaining clauses).
 */
void
solver_backtrack(solver_t *s, uint32_t lit)
{
	uint32_t i, cref;
	uint32_t neg_lit = lit_neg(lit); 
	vec_ui32_t *occ_list;
	
	occ_list = wec_ui32_at(s->occ_lists, neg_lit);
	vec_ui32_foreach(occ_list, cref, i) {
		struct clause *c = clause_read(s, cref);
		c->size++;
	}
	occ_list = wec_ui32_at(s->occ_lists, lit);
	vec_ui32_foreach(occ_list, cref, i) {
		struct clause *c = clause_read(s, cref);
		for (uint32_t j = 0; j < (c->size - 1); j++) {
			vec_push_back(wec_ui32_at(s->occ_lists, c->lits[j]), cref);
		}
		s->n_act_cls++;
	}
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
	uint32_t lit, i;
	uint32_t cref;
	uint32_t n_words;

	assert(vec_size(lits) >= 1);
	n_words = 1 + vec_size(lits);
	cref = cdb_append(s->clause_db, n_words);
	clause = clause_read(s, cref);
	clause->size = vec_size(lits);
	vec_ui32_foreach(lits, lit, i) {
		clause->lits[i] = lit;
		vec_push_back(s->occ_lists->data + lit, cref);
	}
	vec_push_back(s->clauses, cref);
	s->n_act_cls += 1;
	return cref;
}

/**
 * Search for a solution that satisfies a problem on n variables by first 
 * assigning the positive polarity to variables. When this doesn't work for a
 * variable n, we try the negative polarity, and if the it still doesn't work 
 * we backtrack. This process of assigment and backtrack keeps going until we
 * succeed in finding a satisfiable assignment or run out of possible
 * assignments.
 */
int
solver_search(solver_t *s)
{
	uint32_t level = 0;
	uint32_t lit = 0;

	while (1) { 
		uint32_t status = solver_propagate(s, lit);
		if (status == 0) {
			while (vec_at(s->assigns, level) == LIT_FALSE) {
				if (level == 0)
					return SATOMI_UNSAT;
				vec_assign(s->assigns, level, LIT_TRUE);
				level -= 1;
				lit = (level << 1) | vec_at(s->assigns, level);
				solver_backtrack(s, lit);
			}
			vec_assign(s->assigns, level, LIT_FALSE);
			lit += 1; 
		} else {
			if (s->n_act_cls == 0)
				return SATOMI_SAT;
			level += 1;
			lit = (level << 1);
		}
	}
}
