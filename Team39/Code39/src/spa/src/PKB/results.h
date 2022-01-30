#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"
#include "../TNode.h"

template<typename T>
class Results
{
public:
	virtual int AddResult(T item) = 0;
	virtual int GetResult() = 0;
};

class StmtResults : public Results<int>
{
private:
	vector<int> stmt_lst_;

};