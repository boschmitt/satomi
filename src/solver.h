//===--- solver.h -----------------------------------------------------------===
//
//                     satomi: Satisfiability solver
//
// This file is distributed under the BSD 2-Clause License.
// See LICENSE for details.
//
//===------------------------------------------------------------------------===
#ifndef SATOMI__SOLVER_H
#define SATOMI__SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cdb.h"
#include "clause.h"
#include "satomi.h"
#include "utils/mem.h"
#include "utils/misc.h"
#include "utils/vec/vec.h"
#include "utils/wec/wec_ui32.h"

enum {
	LIT_FALSE = 1,
	LIT_TRUE = 0,
	VAR_UNASSING = 3
};

#define UNDEF 0xFFFFFFFF

typedef struct solver_t_ solver_t;
struct solver_t_ {
	/* Clauses Database */
	uint32_t n_act_cls;
	vec_ui32_t *clauses;
	struct cdb *clause_db;
	
	/* Variable Information */
	wec_ui32_t *occ_lists;
	vec_ui8_t *assigns;

	vec_ui32_t *temp_lits;

	/* Search Statistics */
	uint64_t n_nodes;
	double init_time;
};

//===------------------------------------------------------------------------===
extern uint32_t solver_clause_create(solver_t *, vec_ui32_t *);
extern int solver_search(solver_t *);

//===------------------------------------------------------------------------===
// Inline var/lit functions
//===------------------------------------------------------------------------===
static inline uint32_t 
var2lit(uint32_t var, uint8_t pol) { return var + var + ((uint32_t) pol != 0); }

static inline uint32_t
lit2var(uint32_t lit) { return lit >> 1; }
//===------------------------------------------------------------------------===
// Inline lit functions
//===------------------------------------------------------------------------===
static inline uint32_t
lit_neg(uint32_t lit) { return lit ^ 1; }

//===------------------------------------------------------------------------===
// Inline clause functions
//===------------------------------------------------------------------------===
static inline struct clause *
clause_read(solver_t *s, uint32_t cref) { return cdb_handler(s->clause_db, cref); }

#endif /* SATOMI__SOLVER_H */
