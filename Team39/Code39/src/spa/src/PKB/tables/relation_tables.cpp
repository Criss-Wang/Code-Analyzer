#include "relation_tables.h"

bool ModifiesVariableToStmtsTable::UpdateKeyValuePair(const int value_to_update, const vector<string>& keys) {
  try {
    bool add_success = true;
    for (const auto var : keys) {
      if (!KeyExistsInTable(var)) {
        add_success = AddKeyValuePair(var, { value_to_update }) && add_success;
        continue;
      }
      vector<int> var_to_stmt_lst = GetValueByKey(var);
      if (find(var_to_stmt_lst.begin(), var_to_stmt_lst.end(), value_to_update) == var_to_stmt_lst.end()) var_to_stmt_lst.push_back(value_to_update);
      add_success = UpdateKeyWithNewValue(var, var_to_stmt_lst) && add_success;
    }
    return add_success;
  } catch (exception& e) {
    return false;
  }
}
