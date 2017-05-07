# Satomi

Satomi is a SAT solver developed for my own education as I venture in the world
of EDA and CAD tools. My intent it to evolve this solver as I learn different
approaches and techniques. 

## Version 0
This version tries to solve a satisfiability problem on n variables by first 
assigning the **positive** polarity to the nth variable, then using the same
idea recursively on the remaining (n − 1)-variable problem. If this doesn’t 
work, we try the **negative** polarity for the nth variable, and the result will
either succeed or fail. Variables are assingen in ascending order.

The solver is essentially composed of:
1. Assignments Vector: store the current assigment of each variable.
2. Occurences Lists: store for each literal a list of all clauses where it occurs.
3. Clause Database.

### Limitations
Obvious limitations:
* Variables are assigned in ascending order.
* Doesn't take advantage of pure literals.
* Doesn't take advantage of unit clauses.

## Clauses and Clauses Database
Each clause is represented by its current size and by an array of literals,
both of which are continuously allocated. All clauses are stored in one big array
of 32-bits integers, called **clauses database**. Each clause is created by 
reserving the necessary space and returning an index (cref) to its first 32-bit
integer.

A small trick is used to access clauses information: an interface struct is
declared with an zero-sized array at the end:

```c
struct clause {
	uint32_t size;
	uint32_t lits[0];
};
```

Then a clause handler (`struct clause *`) can be retrived by using the index and
casting the pointer to the first 32-bit integer (`uint32_t *`) to the interface
struct.

## References
Papers:
* Davis, P., and Putnam, H. A Computing Procedure for Quantification Theory. 
  Journal of the ACM (1960).
* Davis, P., Logemann, G., and Loveland, D. A Machine Program for Theorem Proving.
  Communications of the ACM (1962) 

Books:
* The Art of Computer Programming, Volume 4, Fascicle 6: Satisfiability by 
  Donald E. Knuth 

