#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

/*
This is an abstract class for tables to record key info for entities in SIMPLE program
*/
template <typename T1, typename T2>
class EntityTable
{
protected:
	// A hash table for entity with index with values being the property indicators of the entity
	unordered_map<T1, T2> entity_table_;

	// Return the size of the table, cast type from unsigned long long to int
	virtual int GetTableSize()
	{
		return static_cast<int>(entity_table_.size());
	}

private:
	// Check if the entity key passed in is a valid one
	virtual bool CheckValidEntityKey(T1 entity_key) = 0;

	// Check if the entity property passed is a valid one
	virtual bool CheckValidProp(T2 entity_prop) = 0;

	// Return if the table has the element with entity_key
	virtual bool HasEntityWithKey(const T1 entity_key)
	{
		return entity_table_.find(entity_key) != entity_table_.end();
	}

public:
	virtual ~EntityTable() = default;

	// WARNING: not safe if some lines are not inserted. E.g., 1,2,4 are present, but 3 is not. Calling 3 gives an internal error;

	// Add a new entity into the table, returning 1 if successful, and 0 otherwise
	virtual int AddEntity(const T1 entity_key, const T2 entity_prop)
	{
		try
		{
			if (!CheckValidProp(entity_prop))
			{
				throw invalid_argument("Invalid property");
			}
			if (!CheckValidEntityKey(entity_key))
			{
				throw invalid_argument("Invalid key");
			}
			if (HasEntityWithKey(entity_key) && entity_table_[entity_key] != entity_prop)
			{
				//throw invalid_argument(to_string(entity_prop));
				throw invalid_argument("Clashing with stored property [property_content here]");
			}
			entity_table_[entity_key] = entity_prop;
			return 1;
		}
		catch (invalid_argument& e)
		{
			// TODO(Zhenlin): add error to the logger if possible
			return 0;
		}
	}

	// Get an entity via its index
	virtual T2 GetPropByKey(T1 entity_key)
	{
		if (CheckValidEntityKey(entity_key))
		{
			return entity_table_[entity_key];
		}
		{
			//throw invalid_argument(to_string(entity_idx));
			throw invalid_argument("Invalid key");
		}
	}
};


/**
 * The Statement table with keys being the statement index and values being the type of the statement
 */
class StmtTable final : public EntityTable<int, int>
{
private:
	bool CheckValidProp(int stmt_prop) override;
	bool CheckValidEntityKey(int stmt_idx) override;

public:
	// TODO(Zhenlin): Check if replace following with Enum class after code review (can require quite a lot of refactoring though)
	static constexpr int assign_idx_ = 3;
	static constexpr int read_idx_ = 4;
	static constexpr int print_idx_ = 5;
	static constexpr int call_idx_ = 6;
	static constexpr int if_idx_ = 7;
	static constexpr int while_idx_ = 8;

	static constexpr int initial_index_ = 1; // first index, also used as offset

	StmtTable() = default;
	// TODO(Zhenlin): Check if virtual destructor needed here

	// Get the set of entities with the same property via the property indicator value
	vector<int> GetStmtLstByProp(int stmt_prop);
};

/**
 * The table with keys being the non-statement entity name string and values being the index of the variable
 */
class NonStmtIdTable final : public EntityTable<string, int>
{
private:
	int entity_type_id_;

	[[nodiscard]] string GetTableType() const;

	bool CheckValidProp(int entity_id) override;
	bool CheckValidEntityKey(string entity_name) override;

public:
	static constexpr int var_id_ = 1;
	static constexpr int const_id_ = 2;
	static constexpr int operator_id_ = 3;
	static constexpr int proc_id_ = 4;

	static constexpr int initial_id_ = 1;

	// TODO(Zhenlin): Check if virtual destructor needed here
	NonStmtIdTable(const int entity_type) : EntityTable{}, entity_type_id_{entity_type}
	{
	}

	int AddEntityByName(const string& entity_name);
	string GetEntityById(int entity_id);
};

class ProcRangeTable final : public EntityTable<int, pair<int, int>>
{
private:
	bool CheckValidProp(pair<int, int> stmt_range) override;
	bool CheckValidEntityKey(int proc_id) override;

public:
	static constexpr int initial_proc_id_ = 1;

	int AddProcRange(int proc_id, pair<int, int> stmt_range);
	int FindProcIdByStmt(int stmt_no);
};

class ProcAdjacencyTable : public EntityTable<int, list<int>>
{
};
