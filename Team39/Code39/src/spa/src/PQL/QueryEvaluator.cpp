#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "Query.h"
#include "PKB.h"

vector<string> evaluate(Query query) {
	Query::EntityType entType = query.getSelectType();
	Clause[] clauses = query.getClauses();

	if (clauses.length == 0) {
		PKB::getAllEntity(entType);
	}

}

