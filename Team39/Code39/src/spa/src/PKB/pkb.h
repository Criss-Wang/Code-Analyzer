#pragma once

#include <stdio.h>
#include <iostream>
#include <string>

#include "entity_table.h"
#include "results.h"
#include "../Utility/TNode.h"
#include "../Utility/AST.h"

using namespace std;

//class PqlArg;

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

	// TODO(Zhenlin): [Performance] Add more variations using function overloading or generic templates
	// One example is Parent(8, 9) which takes in 2 argument. Faster if we directly traverse the tree only to verify if 8 IsParentOf 9.
	// Thus the stmt_no should be replaced by {stmt_no_1, stmt_no_2} below
	StmtResults SearchWithAssociations(char assoc_type, bool is_all, bool is_first, int stmt_no);

	int AddStmtInfo(int stmt_idx, const string& stmt_type);

	int AddNonStmtId(const string& entity_val, const string& entity_type);

	int AddAst(const string& proc_name, AST ast);

	int AddProcRange(const string& proc_name, pair<int, int> stmt_range);

	int AddProcAdjacency(const string& proc_name, list<string> calling_proc_lst)
	{
		return 0;
	}
};