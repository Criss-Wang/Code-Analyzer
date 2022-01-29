#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"

using namespace std;
typedef short Proc;

class TNode;

class PqlArg;

//class EntityTable;  // no need to #include "NonStmtTable.h" as all I need is pointer

class Pkb {

private:
	static vector<int> SearchWithParent(bool is_all, bool is_first, int stmt_no);
	static vector<int> SearchWithFollows(bool is_all, bool is_first, int stmt_no);

	// Getting the AST from the AstTable using the procedure object given
	static TNode* getRootAST (Proc p);

public:
	static vector<int> SearchWithAssociations(string assoc_type, bool is_all, bool is_first, int stmt_no_1);
	static EntityTable<string, int>* var_table_;

	//static AstTable ast_table_;
	static int setProcToAST(Proc p, TNode* r);

};