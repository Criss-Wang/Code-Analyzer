#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "pkb.h"
#include "entity_table.h"
#include "../TNode.h"

EntityTable<string,int>* Pkb::var_table_ = new NonStmtTable(NonStmtTable::var_id_);

int Pkb::setProcToAST(Proc p, TNode* r) {
	return 0;
}

TNode* Pkb::getRootAST (Proc p){
	return nullptr;
}
