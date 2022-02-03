#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

#include "pkb.h"

int Pkb::AddStmtInfo(const int stmt_idx, const string &stmt_type) {
  // Decided not to use string ENUM as it is even more complicated
  int stmt_prop;

  try {
    if (stmt_type == "assign") {
      stmt_prop = StmtTable::assign_idx_;
    }
    else if (stmt_type == "read") {
      stmt_prop = StmtTable::read_idx_;
    }
    else if (stmt_type == "print") {
      stmt_prop = StmtTable::print_idx_;
    }
    else if (stmt_type == "call") {
      stmt_prop = StmtTable::call_idx_;
    }
    else if (stmt_type == "if") {
      stmt_prop = StmtTable::if_idx_;
    }
    else if (stmt_type == "while") {
      stmt_prop = StmtTable::while_idx_;
    }
    else {
      throw invalid_argument("invalid statement type passed");
    }
  }
  catch (invalid_argument &e) {
    return 0;
  }

  stmt_table_.AddEntity(stmt_idx, stmt_prop);
  return 1;
}

int Pkb::AddNonStmtId(const string &entity_val, const string &entity_type) {
  try {
    if (entity_type == "variable") {
      var_table_.AddEntityByName(entity_val);
    }
    else if (entity_type == "constant") {
      const_table_.AddEntityByName(entity_val);
    }
    else if (entity_type == "procedure") {
      proc_table_.AddEntityByName(entity_val);
    }
    else {
      throw invalid_argument("invalid entity type");
    }
  }
  catch (invalid_argument &e) {
    return 0;
  }
  return 1;
}

int Pkb::AddProcRange(const string &proc_name, const pair<int, int> stmt_range) {
  try {
    const int proc_id = proc_table_.GetPropByKey(proc_name);
    proc_range_table_.AddProcRange(proc_id, stmt_range);
  }
  catch (invalid_argument &e) {
    return 0;
  }
  return 1;
}
