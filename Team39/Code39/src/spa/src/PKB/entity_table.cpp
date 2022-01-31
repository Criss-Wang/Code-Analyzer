#include "entity_table.h"

bool StmtTable::CheckValidProp(const int stmt_prop)
{
	return (stmt_prop >= assign_idx_ && stmt_prop <= while_idx_);
}

bool StmtTable::CheckValidEntityKey(const int stmt_idx)
{
	return stmt_idx >= initial_index_;
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
		// TODO(Zhenlin): add error to the logger if possible
		return results;
	}
}


bool NonStmtIdTable::CheckValidProp(const int entity_id)
{
	return entity_id >= initial_id_ && entity_id <= GetTableSize();
}

/**
 * Set to true currently. Will make changes if any appropriate criteria surfaces.
 */
bool NonStmtIdTable::CheckValidEntityKey(const string entity_name)
{
	return true;
}

string NonStmtIdTable::GetEntityById(const int entity_id)
{
	string result; // default construction is "", use result.empty() to verify
	try
	{
		if (this->CheckValidProp(entity_id))
		{
			for (const auto& [key, value] : entity_table_) // this is "Structured binding"
			{
				if ((value) == entity_id) result = key;
			}
			return result;
		}
		throw invalid_argument("invalid ID");
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): add error to the logger if possible
		return result;
	}
}

int NonStmtIdTable::AddEntityByName(const string& entity_name)
{
	const int new_id = GetTableSize() + initial_id_;
	const int addition_signal = AddEntity(entity_name, new_id);
	return addition_signal;
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
	return proc_id >= initial_proc_id_;
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
