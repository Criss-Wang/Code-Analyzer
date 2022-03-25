#include <vector>

#include "table.h"

// Helper function for populating the reverse relation for uses or modifies
bool PopulateReverseRelationship(const vector<int>& keys, const int value_to_update, RelListReverseTable& t) {
  try {
    bool add_success = true;
    for (const int var : keys) {
      if (!t.KeyExistsInTable(var)) {
        add_success = t.AddKeyValuePair(var, { value_to_update }) && add_success;
        continue;
      }

      // If the variable exists in the table, grab the existing vector of statement numbers and update that
      vector<int> var_to_stmts_lst = t.GetValueByKey(var);
      if (find(var_to_stmts_lst.begin(), var_to_stmts_lst.end(), value_to_update) == var_to_stmts_lst.end()) {
        var_to_stmts_lst.push_back(value_to_update);
      }
      add_success = t.UpdateKeyWithNewValue(var, var_to_stmts_lst) && add_success;
    }
    return add_success;
  } catch (exception& e) {
    return false;
  }
}

bool RelListReverseTable::UpdateKeyValuePair(const int value_to_update, const vector<int>& keys) {
  return PopulateReverseRelationship(keys, value_to_update, *this);
}
