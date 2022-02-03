#pragma once

#include <stdio.h>
#include <iostream>
#include <string>

#include "entity_table.h"
#include "results.h"

using namespace std;

class Pkb {
  private:
    ProcRangeTable proc_range_table_ = ProcRangeTable();

  public:
    NonStmtIdTable var_table_ = NonStmtIdTable(NonStmtIdTable::var_id_);
    NonStmtIdTable const_table_ = NonStmtIdTable(NonStmtIdTable::const_id_);
    NonStmtIdTable proc_table_ = NonStmtIdTable(NonStmtIdTable::proc_id_);

    StmtTable stmt_table_ = StmtTable();

    int AddStmtInfo(int stmt_idx, const string &stmt_type);

    int AddNonStmtId(const string &entity_val, const string &entity_type);

    int AddProcRange(const string &proc_name, pair<int, int> stmt_range);

    int AddProcAdjacency(const string &proc_name, list<string> calling_proc_lst);
};
