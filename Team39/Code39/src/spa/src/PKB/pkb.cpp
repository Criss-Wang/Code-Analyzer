#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

#include "pkb.h"

StmtResults Pkb::SearchWithAssociations(const char assoc_type, const bool is_all, const bool is_first,
                                        const int stmt_no)
{
	try
	{
		if (assoc_type == parent_rel_)
		{
			return SearchWithParent(is_all, is_first, stmt_no);
		}
		if (assoc_type == follows_rel_)
		{
			return SearchWithFollows(is_all, is_first, stmt_no);
		} 
		throw invalid_argument("Bad type given");
	}
	catch (exception& e)
	{
		StmtResults result;
		return result;
	}
}

StmtResults Pkb::SearchWithFollows(const bool is_all, const bool is_first, const int stmt_no)
{
	const int proc_id = proc_range_table_.FindProcIdByStmt(stmt_no);
	TNode* r_node = ast_table_.GetRootAst(proc_id);

	StmtResults result;


	// Create a queue for BFS
	queue<vector<TNode*>> queue;
	queue.push({r_node});

	// Boolean to determine when to stop the BFS
	bool found_stmt = false;

	vector<TNode*> current_stmt_lst;

	while (!queue.empty())
	{
		current_stmt_lst = queue.front();
		queue.pop();

		for (const auto t : current_stmt_lst)
		{
			// if not a stmtlst node or a stmt node, skip
			if (!t->isStmtLst() && !t->isStmt()) continue;
			// If statement data (i.e. line number) matches the stmt_no, stop the BFS
			if (t->isStmt() && stoi(t->getData()) == stmt_no)
			{
				found_stmt = true;
			}
			queue.push(t->getChildNodes());
		}

		if (found_stmt) break;
	}

	// A doubly linked list to process the result
	deque<int> result_dll;

	for (const auto t : current_stmt_lst)
	{
		if (t->isStmt()) result_dll.push_back(stoi(t->getData()));
	}

	if (is_first)
	{
		while (result_dll.front() != stmt_no)
		{
			result_dll.pop_front();
		}
		result_dll.pop_front();
		if (!is_all)
		{
			result.AddResult(result_dll.front());
			return result;
		}
	} else
	{
		while (result_dll.back() != stmt_no)
		{
			result_dll.pop_back();
		}
		result_dll.pop_back();
		if (!is_all)
		{
			result.AddResult(result_dll.back());
			return result;
		}
	}
	while (!result_dll.empty())
	{
		result.AddResult(result_dll.front());
		result_dll.pop_front();
	}
	return result;
}

// TODO(Zhenlin): [SE Practice] Decide if put this auxilary function as private class member fn later
void Dfs(TNode* r_node, const int stmt_no, bool& found_stmt, stack<int>& stmt_stack, TNode*& curr_node)
{
	if (!r_node->isStmt() && !r_node->isStmtLst()) return;

	if (r_node->isStmt())
	{
		int curr_stmt_no = stoi(r_node->getData());
		stmt_stack.push(curr_stmt_no);
		if (curr_stmt_no == stmt_no)
		{
			found_stmt = true;
		}
	}
	for (const auto t : r_node->getChildNodes())
	{
		if (found_stmt) break;
		curr_node = t;

		Dfs(t, stmt_no, found_stmt, stmt_stack, curr_node);
	}
	if (r_node->isStmt() && !found_stmt) stmt_stack.pop();
}

StmtResults Pkb::SearchWithParent(const bool is_all, const bool is_first, const int stmt_no)
{
	const int proc_id = proc_range_table_.FindProcIdByStmt(stmt_no);
	TNode* r_node = ast_table_.GetRootAst(proc_id);

	StmtResults result;

	bool found_stmt = false;
	stack<int> stmt_stack;
	TNode* curr_node = r_node;

	Dfs(r_node, stmt_no, found_stmt, stmt_stack, curr_node);

	if (!is_first)
	{
		stmt_stack.pop();
		if (is_all)
		{
			while (!stmt_stack.empty())
			{
				result.AddResult(stmt_stack.top());
				stmt_stack.pop();
			}
		} else
		{
			result.AddResult(stmt_stack.top());
		}
	} else
	{
		if (is_all)
		{
			queue<TNode*> queue;
			queue.push(curr_node);
			while (!queue.empty())
			{
				TNode* n = queue.front();
				queue.pop();
				for (const auto t : n->getChildNodes())
				{
					if (!t->isStmt() && !t->isStmtLst()) continue;
					if (t->isStmt()) result.AddResult(stoi(t->getData()));
					queue.push(t);
				}
			}
		} else
		{
			for (const auto t : curr_node->getChildNodes())
			{
				if (t->isStmt()) result.AddResult(stoi(t->getData()));
			}
		}
	}
	return result;
}

int Pkb::AddStmtInfo(const int stmt_idx, const string& stmt_type)
{
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
	}
	catch (invalid_argument& e)
	{
		return 0;
	}

	stmt_table_.AddEntity(stmt_idx, stmt_prop);
	return 1;
}

int Pkb::AddNonStmtId(const string& entity_val, const string& entity_type)
{
	try
	{
		if (entity_type == "variable")
		{
			var_table_.AddEntityByName(entity_val);
		}
		else if (entity_type == "constant")
		{
			const_table_.AddEntityByName(entity_val);
		}
		else if (entity_type == "procedure")
		{
			proc_table_.AddEntityByName(entity_val);
		}
		else
		{
			throw invalid_argument("invalid entity type");
		}
	}
	catch (invalid_argument& e)
	{
		return 0;
	}
	return 1;
}

int Pkb::AddProcRange(const string& proc_name, const pair<int, int> stmt_range)
{
	try
	{
		const int proc_id = proc_table_.GetPropByKey(proc_name);
		proc_range_table_.AddProcRange(proc_id, stmt_range);
	}
	catch (invalid_argument& e)
	{
		return 0;
	}
	return 1;
}

int Pkb::AddAst(const string& proc_name, AST ast)
{
	try
	{
		const int proc_id = proc_table_.GetPropByKey(proc_name);
		ast_table_.AddEntity(proc_id, ast);
	}
	catch (invalid_argument& e)
	{
		return 0;
	}
	return 1;
}