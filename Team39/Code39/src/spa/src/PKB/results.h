#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"

template<typename T>
class Results
{
public:
	virtual ~Results() = default;
	virtual int AddResult(T item) = 0;
	virtual int GetResult() = 0;
};

class StmtResults final : public Results<int>
{
private:
	vector<int> stmt_lst_;
public:
	virtual ~StmtResults() override;
	int AddResult(int item) override;
	int GetResult() override;
};

class VarResults final : public Results<string>
{
private:
	vector<string> var_lst_;
public:
	int AddResult(string item) override;
	int GetResult() override;
};