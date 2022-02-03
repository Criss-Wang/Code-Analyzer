#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"

template<typename T>
class Results
{
private:
	vector<T> result_lst_;
public:
	virtual ~Results() = default;
	virtual int AddResult(T item){
		try
		{
			result_lst_.push_back(item);
			return 1;
		} catch (exception& e)
		{
			return 0;
		}
	}
	virtual vector<T> GetResult(){
		return result_lst_;
	}
};

// TODO(Zhenlin): [Implementation] [SE Practice] customize result fns for PQL evaluator use
class StmtResults final : public Results<int>
{
};

class VarResults final : public Results<string>
{
};