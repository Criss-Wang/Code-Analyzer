#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "entity_table.h"

template<typename T>
class Results {
  private:
    vector<T> result_lst_;

  public:
    virtual ~Results() = default;

    virtual int AddResult(T item);

    virtual vector<T> GetResult();
};

// TODO(Zhenlin): [Implementation] [SE Practice] customize result fns for PQL evaluator use
class StmtResults final : public Results<int> {
};

class VarResults final : public Results<string> {
};
