#pragma once

#include <vector>

using namespace  std;

class TNode
{
	vector<TNode*> child_nodes_;
	const bool is_leaf_ = true;
public:
	int GetValue()
	{
		return 1;
	}

	vector<TNode*> GetChildNodes();

	bool IsLeaf()
	{
		return true;
	}
};
