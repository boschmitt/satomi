//===--- solver_api.h -------------------------------------------------------===
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
#include "utils/misc.h" 
#include "utils/vec/vec.h"

//===------------------------------------------------------------------------===
// Satoko external functions
//===------------------------------------------------------------------------===
solver_t *
satomi_create()
{
	solver_t *s = STM_CALLOC(solver_t, 1);

	/* Clauses Database */
	s->clauses = vec_ui32_alloc(0);
	s->clause_db = cdb_alloc(0);
	s->watches = vec_wl_alloc(0);
	/* Variable Information */
	s->var_order = vec_ui32_alloc(0);
	s->assigns = vec_ui8_alloc(0);
	/* Assignments */
	s->trail = vec_ui32_alloc(0);
	s->trail_lim = vec_ui32_alloc(0);
	/* Temporary data */
	s->temp_lits = vec_ui32_alloc(0);
	return s;
}

void
satomi_destroy(solver_t *s)
{
	vec_free(s->clauses);
	cdb_free(s->clause_db);
	vec_wl_free(s->watches);
	vec_free(s->var_order);
	vec_free(s->assigns);
	vec_free(s->trail);
	vec_free(s->trail_lim);
	vec_free(s->temp_lits);
	STM_FREE(s);
}

void
satomi_add_variable(solver_t *s) 
{
	uint32_t var = vec_size(s->var_order);
	vec_wl_push(s->watches);
	vec_wl_push(s->watches);
	vec_push_back(s->assigns, VAR_UNASSING);
	vec_push_back(s->var_order, var);
}

int
satomi_add_clause(solver_t *s, uint32_t *lits, uint32_t size)
{
	uint32_t max_var;

	qsort((void *) lits, size, sizeof(uint32_t), stm_ui32_comp_desc);
	max_var = lit2var(lits[0]);
	while (max_var >= vec_size(s->var_order))
		satomi_add_variable(s);

	vec_clear(s->temp_lits);
	uint32_t cref;
	uint32_t prev_lit = UNDEF;
	for (uint32_t i = 0; i < size; i++) {
		if (lits[i] == lit_neg(prev_lit) || lit_value(s, lits[i]) == LIT_TRUE)
			return SATOMI_OK;
		else if (lits[i] != prev_lit && var_value(s, lit2var(lits[i])) == VAR_UNASSING) {
			prev_lit = lits[i];
			vec_push_back(s->temp_lits, lits[i]);
		}
	}

	if (vec_size(s->temp_lits) == 0)
		return SATOMI_ERR;
	if (vec_size(s->temp_lits) == 1) {
		solver_enqueue(s, vec_at(s->temp_lits, 0));
		return (solver_propagate(s) == UNDEF);
	}

	cref = solver_clause_create(s, s->temp_lits);
	clause_watch(s, cref);
	return SATOMI_OK;
}

int
satomi_solve(solver_t *s)
{
	int status = SATOMI_UNDEC;

	assert(s);
	s->stats.init_time = stm_clock();
	status = solver_search(s);
	return status;
}

void
satomi_print_stats(solver_t *s)
{
	double elapsed_time = stm_clock() - s->stats.init_time;
	fprintf(stdout, "conflicts    : %-12lld\n", s->stats.n_conflicts);
	fprintf(stdout, "decisions    : %-12lld  (%.0f /sec)\n", 
	        s->stats.n_decisions, (s->stats.n_decisions/ elapsed_time));
	fprintf(stdout, "propagations : %-12lld  (%.0f /sec)\n",
	        s->stats.n_propagations, (s->stats.n_propagations/ elapsed_time));
	fprintf(stdout, "cpu time     : %g s\n", elapsed_time);
}

//===------------------------------------------------------------------------===
// Debuggin
//===------------------------------------------------------------------------===
void
satomi_print_clauses(solver_t *s)
{
	uint32_t i;
	uint32_t cref;
	fprintf(stdout, "Print Clauses :\n");
	vec_ui32_foreach(s->clauses, cref, i) {
		struct clause *c = clause_read(s, cref);
		clause_print(c);
	}
}
