#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"
#include "../TNode.h"

using namespace std;
typedef short Proc;

//class TNode;

class PqlArg;

class AstTable
{
private:
	// Getting the AST from the AstTable using the procedure object given
	TNode* GetRootAst(Proc p);

	// The function assumes that the number of procedures is found, so that we build initial array of TNode pointers with fixed sizes
	int SetProcToAst(Proc p, TNode* r);
};

//class EntityTable;  // no need to #include "NonStmtTable.h" as all I need is pointer

class Pkb {

private:
	static vector<int> SearchWithParent(bool is_all, bool is_first, int stmt_no);
	static vector<int> SearchWithFollows(bool is_all, bool is_first, int stmt_no);

public:
	static constexpr char parent_rel_ = 'P';
	static constexpr char follows_rel_ = 'F';

	static EntityTable<string, int>* var_table_;
	static EntityTable<string, int>* const_table_;
	static EntityTable<string, int>* proc_table_;
	static AstTable ast_table_;

	static vector<int> SearchWithAssociations(char assoc_type, bool is_all, bool is_first, int stmt_no);

	// This is temporary API for all non statement entities, need to change the name later (cannot think of a good one now)
	static int AddVar(string var_name);

	static int AddStmt(int stmt_no);
};