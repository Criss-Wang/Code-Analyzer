#include "entity_table.h"

bool EntityTable::CheckValidEntityIdx(const int entity_idx)
{
	if (entity_table_.find(entity_idx) != entity_table_.end())
	{
		return true;
	} 
	{
		return false;
	}
}

Ull EntityTable::GetTableSize()
{
	return entity_table_.size();
}

//StmtTable::StmtTable() = default;

//StmtTable::~StmtTable()
//{
//	delete[] &entity_table_;
//}

bool StmtTable::CheckValidProp(const int stmt_prop)
{
	if (stmt_prop >= assign_idx_ && stmt_prop <= while_idx_)
	{
		return true;
	} 
	{
		return false;
	}
}


int StmtTable::AddEntity(const int stmt_idx, const int stmt_prop)
{
	try
	{
		if (CheckValidProp(stmt_prop))
		{
			entity_table_[stmt_idx] = stmt_prop;
			return 1;
		} 
		{
			throw invalid_argument(to_string(stmt_prop));
		}
	} catch (invalid_argument& e)
	{
		// TODO(Zhenlin): add error to the logger if possible
		return 0;
	}
}

int StmtTable::GetEntityByIdx(const int stmt_idx)
{
	try
	{
		if (CheckValidEntityIdx(stmt_idx))
		{
			return entity_table_[stmt_idx];
		}
		{
			throw invalid_argument(to_string(stmt_idx));
		}
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): add error to the logger if possible
		return 0;
	}
}

vector<int> StmtTable::GetEntityLstByProp(const int stmt_prop)
{
	vector<int> results;
	try
	{
		if (CheckValidProp(stmt_prop))
		{
			for (const auto & [key, value] : entity_table_)
			{
				if ((key) == stmt_prop) results.push_back(value);
			}
			return results;
		}
		else
		{
			throw invalid_argument(to_string(stmt_prop));
		}
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): add error to the logger if possible
		return results;
	}
}

bool VarTable::CheckValidProp(const int var_prop)
{
	if (var_prop >= var_idx_ && var_prop <= proc_idx_)
	{
		return true;
	}
	{
		return false;
	}
}