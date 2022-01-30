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
	unordered_map<int, TNode*> ast_table_;

public:
	// Getting the AST from the AstTable using the procedure object given
	TNode* GetRootAst(int proc_id);

	// The function assumes that the number of procedures is found, so that we build initial array of TNode pointers with fixed sizes
	int SetProcToAst(Proc p, TNode* r);
};

//class EntityTable;  // no need to #include "NonStmtTable.h" as all I need is pointer

class Pkb {

private:
	StmtResults SearchWithParent(bool is_all, bool is_first, int stmt_no);
	StmtResults SearchWithFollows(bool is_all, bool is_first, int stmt_no);

	ProcRangeTable proc_range_table_ = ProcRangeTable();

public:
	//Pkb() = default;
	//~Pkb() = default;

	static constexpr char parent_rel_ = 'P';
	static constexpr char follows_rel_ = 'F';
	
	NonStmtIdTable var_table_ = NonStmtIdTable(NonStmtIdTable::var_id_);
	NonStmtIdTable const_table_ = NonStmtIdTable(NonStmtIdTable::const_id_);
	NonStmtIdTable proc_table_ = NonStmtIdTable(NonStmtIdTable::proc_id_);
	StmtTable stmt_table_ = StmtTable();

	AstTable ast_table_;

	StmtResults SearchWithAssociations(char assoc_type, bool is_all, bool is_first, int stmt_no);

	int AddStmtInfo(int stmt_idx, const string& stmt_type);

	int AddNonStmtId(const string& entity_val, const string& entity_type);

	int AddProcRange(const string& proc_name, pair<int, int> stmt_range);

	int AddProcAdjacency(const string& proc_name, list<string> calling_proc_lst)
	{
		return 0;
	}
};