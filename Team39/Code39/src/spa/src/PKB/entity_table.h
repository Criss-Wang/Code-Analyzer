#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

typedef unsigned long long Ull;

/*
This is an abstract class for tables to record key infos for entities in SIMPLE program
*/

class EntityTable
{
protected:
	unordered_map<int, int> entity_table_;

	// Check if the property index passed is a valid one
	virtual bool CheckValidEntityIdx(int entity_idx);

	// Return the size of the table
	virtual Ull GetTableSize();

private:
	virtual bool CheckValidProp(int entity_prop) = 0;

public:
	virtual ~EntityTable() = default;

	// A hash table for entity with index with values being the property indicators of the entity
	// WARNING: not safe if some lines are not inserted. E.g., 1,2,4 are present, but 3 is not. Calling 3 gives an internal error;

	// Add a new entity into the table, returning 1 if successful, and 0 otherwise
	virtual int AddEntity(int entity_idx, int entity_prop) = 0;

	// Get an entity via its index
	virtual int GetEntityByIdx(int entity_idx) = 0;

	// Get the set of entities with the same property via the property indicator value
	virtual vector<int> GetEntityLstByProp(int entity_prop) = 0;
};

class StmtTable final : public EntityTable
{
private:
	bool CheckValidProp(int stmt_prop) override;

public:
	// TODO(Zhenlin): Check if replace following with Enum class after code review (can require quite a lot of refactoring though)
	static constexpr int assign_idx_ = 3;
	static constexpr int read_idx_ = 4;
	static constexpr int print_idx_ = 5;
	static constexpr int call_idx_ = 6;
	static constexpr int if_idx_ = 7;
	static constexpr int while_idx_ = 8;
	

	StmtTable() = default;

	// TODO(Zhenlin): Check if virtual destructor needed here

	int AddEntity(int stmt_idx, int stmt_prop) override;

	int GetEntityByIdx(int stmt_idx) override;

	vector<int> GetEntityLstByProp(int stmt_prop) override;

};

class NonStmtTable final : public EntityTable
{
private:
	bool CheckValidProp(int var_prop) override;

public:
	// TODO(Zhenlin): Check if replace following with Enum class after code review (can require quite a lot of refactoring though)
	static constexpr int var_idx_ = 3;
	static constexpr int const_idx_ = 4;
	static constexpr int operator_idx_ = 5;
	static constexpr int proc_idx_ = 6;

	NonStmtTable() = default;

	// TODO(Zhenlin): Check if virtual destructor needed here

	int AddEntity(int var_idx, int var_prop) override;

	int GetEntityByIdx(int var_idx) override;

	vector<int> GetEntityLstByProp(int var_prop) override;
};