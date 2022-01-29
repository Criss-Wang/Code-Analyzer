#include "entity_table.h"

//template <typename T1, typename T2>
//bool EntityTable<T1,T2>::CheckValidEntityKey(const T1 entity_idx)
//{
//	if (entity_table_.contains(entity_idx))
//	{
//		return true;
//	} 
//	{
//		return false;
//	}
//}

//template <typename T1, typename T2>
//Ull EntityTable::GetTableSize()
//{
//	return entity_table_.size();
//}

//StmtTable::StmtTable() = default;

//StmtTable::~StmtTable()
//{
//	delete[] &entity_table_;
//}

//int StmtTable::AddEntity(const int stmt_idx, const int stmt_prop)
//{
//	try
//	{
//		if (CheckValidProp(stmt_prop))
//		{
//			entity_table_[stmt_idx] = stmt_prop;
//			return 1;
//		} 
//		{
//			throw invalid_argument(to_string(stmt_prop));
//		}
//	} catch (invalid_argument& e)
//	{
//		// TODO(Zhenlin): add error to the logger if possible
//		return 0;
//	}
//}

//int StmtTable::GetEntityByKey(const int stmt_idx)
//{
//	try
//	{
//		if (CheckValidEntityKey(stmt_idx))
//		{
//			return entity_table_[stmt_idx];
//		}
//		{
//			throw invalid_argument(to_string(stmt_idx));
//		}
//	}
//	catch (invalid_argument& e)
//	{
//		// TODO(Zhenlin): add error to the logger if possible
//		return 0;
//	}
//}

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
			for (const auto & [key, value] : entity_table_)
			{
				if ((value) == stmt_prop) results.push_back(key);
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

bool NonStmtTable::CheckValidProp(const int var_id)
{
	return var_id >= initial_id_ && var_id <= GetTableSize();
}

/**
 * Set to true currently. Will make changes if any appropriate criteria surfaces.
 */
bool NonStmtTable::CheckValidEntityKey(const string var_name)
{
	return true;
}

string NonStmtTable::GetVarById(const int var_id)
{
	string result; // default construction is "", use result.empty() to verify
	try
	{
		if (this->CheckValidProp(var_id))
		{
			for (const auto& [key, value] : entity_table_) // this is "Structured binding"
			{
				if ((value) == var_id) result = key;
			}
			return result;
		}
		else
		{
			throw invalid_argument("invalid ID");
		}
	}
	catch (invalid_argument& e)
	{
		// TODO(Zhenlin): add error to the logger if possible
		return result;
	}
}