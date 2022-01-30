#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "pkb.h"
#include "entity_table.h"
#include "../TNode.h"

EntityTable<string, int>* Pkb::var_table_ = new NonStmtTable(NonStmtTable::var_id_);
EntityTable<string, int>* Pkb::const_table_ = new NonStmtTable(NonStmtTable::const_id_);
EntityTable<string, int>* Pkb::proc_table_ = new NonStmtTable(NonStmtTable::proc_id_);

// TODO: convert AstTable stub to actual implementation
int AstTable::SetProcToAst(Proc p, TNode* r) {
	return 0;
}

TNode* AstTable::GetRootAst(Proc p) {
	return 0;
}

template<typename T>
vector<T> SearchResult()
{
	vector<T> result;
	return result;
}

vector<int> Pkb::SearchWithAssociations(char assoc_type, bool is_all, bool is_first, int stmt_no)
{
	// TODO(Zhenlin): check if multiple stmt_no's need to be passed in (adjust with int* stmt_no_lst)
	vector<int> result;
	try
	{
		if (assoc_type == parent_rel_)
		{
			return SearchWithParent(is_all, is_first, stmt_no);
		}
		else if (assoc_type == follows_rel_)
		{
			return SearchWithFollows(is_all, is_first, stmt_no);
		}
		{
			throw "Bad type given";
		}
	} catch (const char* txtException)
	{
		return result;
	}
}

vector<int> 