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
#include "utils/wec/wec_ui32.h"

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
	/* Variable Information */
	s->occ_lists = wec_ui32_alloc(0);
	s->assigns = vec_ui8_alloc(0);
	/* Temporary data */
	s->temp_lits = vec_ui32_alloc(0);
	/* Search Statistics */
	s->n_nodes = 1;
	return s;
}

void
satomi_destroy(solver_t *s)
{
	vec_free(s->clauses);
	cdb_free(s->clause_db);
	wec_ui32_free(s->occ_lists);
	vec_free(s->assigns);
	vec_free(s->temp_lits);
	STM_FREE(s);
}

void
satomi_add_variable(solver_t *s) 
{
	wec_ui32_push_level(s->occ_lists);
	wec_ui32_push_level(s->occ_lists);
	vec_push_back(s->assigns, LIT_TRUE);
}

int
satomi_add_clause(solver_t *s, uint32_t *lits, uint32_t size)
{
	uint32_t max_var;

	qsort((void *) lits, size, sizeof(uint32_t), stm_ui32_comp_desc);
	max_var = lit2var(lits[0]);
	while (max_var >= (wec_ui32_size(s->occ_lists) >> 1))
		satomi_add_variable(s);

	vec_clear(s->temp_lits);
	uint32_t cref;
	uint32_t prev_lit = UNDEF;
	for (uint32_t i = 0; i < size; i++) {
		if (lits[i] == lit_neg(prev_lit))
			return SATOMI_OK;
		else if (lits[i] != prev_lit) {
			prev_lit = lits[i];
			vec_push_back(s->temp_lits, lits[i]);
		}
	}

	if (vec_size(s->temp_lits) == 0)
		return SATOMI_ERR;
	cref = solver_clause_create(s, s->temp_lits);
	return SATOMI_OK;
}

int
satomi_solve(solver_t *s)
{
	int status = SATOMI_UNDEC;

	assert(s);
	s->init_time = stm_clock();
	status = solver_search(s);
	return status;
}

void
satomi_print_problem_stats(solver_t *s)
{
	double elapsed_time = stm_clock() - s->init_time;
	fprintf(stdout, "Number of vars: %-6u\n",
	        (wec_ui32_size(s->occ_lists) >> 1));
	fprintf(stdout, "Number of clauses: %-10u\n", vec_size(s->clauses));
}


void
satomi_print_search_stats(solver_t *s)
{
	double elapsed_time = stm_clock() - s->init_time;
	fprintf(stdout, "Visited nodes : %-12lld  (%.0f /sec)\n",
	        s->n_nodes, (s->n_nodes/ elapsed_time));
	fprintf(stdout, "CPU time     : %g \n", elapsed_time);
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

void
satomi_print_occ_lists(solver_t *s)
{
	fprintf(stdout, "Print Occurence Lists:\n");
	wec_ui32_print(s->occ_lists);
}
