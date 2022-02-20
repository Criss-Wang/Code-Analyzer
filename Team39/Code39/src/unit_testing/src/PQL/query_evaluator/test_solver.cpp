#include "../../spa/src/PQL/query_evaluator/solver.h"

#include "catch.hpp"

//Solver will solve all the clauses with two synonyms involved 
//We focus on two cases:
// - having one such that clause or one pattern clause
// - having one such that clause and one pattern clause