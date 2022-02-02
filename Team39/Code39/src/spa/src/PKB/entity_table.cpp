#include "entity_table.h"

bool StmtTable::CheckValidProp(const int stmt_prop)
{
	return (stmt_prop >= assign_idx_ && stmt_prop <= while_idx_);
}

bool StmtTable::CheckValidEntityKey(const int stmt_idx)
{
	return stmt_idx >= initial_index_;
}

bool StmtTable::MatchProp(const int stored_prop, const int stmt_prop)
{
	return stored_prop == stmt_prop;
}


vector<int> StmtTable::GetStmtLstByProp(const int stmt_prop)
{
	vector<int> results;
	try
	{
		if (this->CheckValidProp(stmt_prop))
		{
			for (const auto& [key, value] : entity_table_)
			{
				if ((value) == stmt_prop) results.push_back(key);
			}
			return results;
		}
		throw invalid_argument(to_string(stmt_prop));
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): [Utility] add error to the logger if possible
		return results;
	}
}


bool NonStmtIdTable::CheckValidProp(const int entity_id)
{
	return entity_id >= GetTableSize();
}

/**
 * Set to true currently. Will make changes if any appropriate criteria surfaces.
 */
bool NonStmtIdTable::CheckValidEntityKey(const string entity_name)
{
	return true;
}

bool NonStmtIdTable::MatchProp(const int stored_id, const int entity_id)
{
	return stored_id == entity_id;
}


int NonStmtIdTable::AddEntityByName(const string& entity_name)
{
	const int new_id = GetTableSize() + initial_id_;
	const int addition_signal = AddEntity(entity_name, new_id);
	return addition_signal;
}

string NonStmtIdTable::GetEntityById(const int entity_id)
{
	string result; // default construction is "", use result.empty() to verify
	try
	{
		for (const auto& [key, value] : entity_table_) // this is "Structured binding"
		{
			if ((value) == entity_id) return key;
			
		}
		
		throw invalid_argument("invalid ID");
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): [Utility] add error to the logger if possible
		return result;
	}
}

string NonStmtIdTable::GetTableType() const
{
	switch (entity_type_id_)
	{
	case var_id_:
		return "Variable";
	case proc_id_:
		return "Procedure";
	case const_id_:
		return "Constant";
	case operator_id_:
		return "Operator";
	default:
		throw invalid_argument("Invalid entity type");
	}
};

/**
 * Set to true currently. Will make changes if any appropriate criteria surfaces.
 */
bool AstTable::CheckValidProp(AST ast)
{
	return true;
}

bool AstTable::CheckValidEntityKey(const Proc proc_id)
{
	return proc_id >= NonStmtIdTable::initial_id_;
}

bool AstTable::MatchProp(AST stored_prop, const AST arg_prop)
{
	return stored_prop.isEqual(arg_prop);
}


TNode* AstTable::GetRootAst(const Proc proc_id)
{
	AST proc_ast = GetPropByKey(proc_id);
	return proc_ast.getRoot();
}

bool ProcRangeTable::CheckValidProp(const pair<int, int> stmt_range)
{
	for (const auto& [key, value] : entity_table_)
	{
		if (value.first <= stmt_range.first && value.second >= stmt_range.first)
		{
			return false;
		}
		if (value.first <= stmt_range.second && value.second >= stmt_range.second)
		{
			return false;
		}
	}
	return true;
}

bool ProcRangeTable::CheckValidEntityKey(const int proc_id)
{
	return proc_id >= NonStmtIdTable::initial_id_;
}

bool ProcRangeTable::MatchProp(pair<int, int> stored_range, pair<int, int> stmt_range)
{
	return stored_range == stmt_range;
}

int ProcRangeTable::AddProcRange(const int proc_id, const pair<int, int> stmt_range)
{
	const int addition_signal = AddEntity(proc_id, stmt_range);
	return addition_signal;
}

int ProcRangeTable::FindProcIdByStmt(const int stmt_no)
{
	try
	{
		for (const auto& [key, value] : entity_table_)
		{
			if (value.first <= stmt_no && value.second >= stmt_no)
			{
				return key;
			}
		}
		throw invalid_argument("statement not in any procedure recorded");
	}
	catch (invalid_argument& e)
	{
		return 0; // meaning stmt_no
	}
}
