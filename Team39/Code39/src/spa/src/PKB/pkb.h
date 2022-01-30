#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "entity_table.h"
#include "results.h"
#include "../TNode.h"

using namespace std;
typedef short Proc;

//class TNode;

class PqlArg;

class AstTable
{
private:
	unordered_map<string, TNode*> ast_table_;
public:
	// Getting the AST from the AstTable using the procedure object given
	TNode* GetRootAst(Proc p);

	// The function assumes that the number of procedures is found, so that we build initial array of TNode pointers with fixed sizes
	int SetProcToAst(Proc p, TNode* r);
};

//class EntityTable;  // no need to #include "NonStmtTable.h" as all I need is pointer

class Pkb {

private:
	StmtResults SearchWithParent(bool is_all, bool is_first, int stmt_no);
	StmtResults SearchWithFollows(bool is_all, bool is_first, int stmt_no);

public:
	//Pkb() = default;
	//~Pkb() = default;

	static constexpr char parent_rel_ = 'P';
	static constexpr char follows_rel_ = 'F';

	EntityTable<string, int>* var_table_ = new NonStmtTable(NonStmtTable::var_id_);
	EntityTable<string, int>* const_table_ = new NonStmtTable(NonStmtTable::const_id_);
	EntityTable<string, int>* proc_table_ = new NonStmtTable(NonStmtTable::proc_id_);
	EntityTable<int, int>* stmt_table_ = new StmtTable();

	AstTable ast_table_;

	StmtResults SearchWithAssociations(char assoc_type, bool is_all, bool is_first, int stmt_no);

	int AddStmt(int stmt_idx, const string& stmt_type);

	int AddNonStmt(const string& entity_val, const string& entity_type);
};