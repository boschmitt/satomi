# Satomi

Satomi is a SAT solver developed for my own education as I venture in the world
of EDA and CAD tools. My intent it to evolve this solver as I learn different
approaches and techniques. 

## Version 2

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

