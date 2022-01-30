#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

#include "pkb.h"

void Bfs(TNode* r)
{
	// Create a queue for BFS
	list<string> queue;
	queue.push_back(r->GetValue());

	list<string>::iterator i;

	while (!queue.empty())
	{
		// Dequeue a vertex from queue and print it
		r = queue.front();
		queue.pop_front();


		for (i = r.begin(); i != adj[s].end(); ++i)
		{
			if (!visited[*i])
			{
				visited[*i] = true;
				queue.push_back(*i);
			}
		}
	}
}

// TODO: convert AstTable stub to actual implementation
int AstTable::SetProcToAst(Proc p, TNode* r)
{
	return 0;
}

TNode* AstTable::GetRootAst(Proc p)
{
	return nullptr;
}

template<typename T>
vector<T> SearchResult()
{
	vector<T> result;
	return result;
}

StmtResults Pkb::SearchWithAssociations(const char assoc_type, const bool is_all, const bool is_first, const int stmt_no)
{
	// TODO(Zhenlin): check if multiple stmt_no's need to be passed in (adjust with int* stmt_no_lst)
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
			throw exception("Bad type given");
		}
	} catch (exception& e)
	{
		StmtResults result;
		return result;
	}
}

StmtResults Pkb::SearchWithParent(bool is_all, bool is_first, int stmt_no)
{
	
}

StmtResults Pkb::SearchWithFollows(bool is_all, bool is_first, int stmt_no)
{
	
}

int Pkb::AddStmt(const int stmt_idx, const string& stmt_type)
{
	// TODO(Zhenlin) : check if const can be applied to this function as requested by the suggestion
	// Decided not to use string ENUM as it is even more complicated
	int stmt_prop;

	try
	{
		if (stmt_type == "assign")
		{
			stmt_prop = StmtTable::assign_idx_;
		}
		else if (stmt_type == "read")
		{
			stmt_prop = StmtTable::read_idx_;
		}
		else if (stmt_type == "print")
		{
			stmt_prop = StmtTable::print_idx_;
		}
		else if (stmt_type == "call")
		{
			stmt_prop = StmtTable::call_idx_;
		}
		else if (stmt_type == "if")
		{
			stmt_prop = StmtTable::if_idx_;
		}
		else if (stmt_type == "while")
		{
			stmt_prop = StmtTable::while_idx_;
		}
		else
		{
			throw invalid_argument("invalid statement type passed");
		}
	} catch (invalid_argument& e)
	{
		return 0;
	}

	stmt_table_->AddEntity(stmt_idx, stmt_prop);
	return 1;
}

int Pkb::AddNonStmt(const string& entity_val, const string& entity_type)
{
	try
	{
		if (entity_type == "variable")
		{
			var_table_->AddEntity(entity_val, NonStmtTable::var_id_);
		} else if (entity_type == "constant")
		{
			const_table_->AddEntity(entity_val, NonStmtTable::const_id_);
		//} else if (entity_type == "procedure")
		//{
		//	proc_table_->AddEntity(entity_val, NonStmtTable::proc_id_);
		} else
		{
			throw invalid_argument("invalid entity type");
		}
	} catch (invalid_argument& e)
	{
		return 0;
	}
	return 1;
}