#include "relation_tables.h"
#include "table.h"

// Helper function for populating the reverse relation for uses or modifies i.e. variable -> [line numbers]
template<typename T1>
bool PopulateReverseRelationship(const vector<string>& keys, const T1 value_to_update, Table<string, vector<T1>>& t) {
  try {
    bool add_success = true;
    for (const string var : keys) {
      if (!t.KeyExistsInTable(var)) {
        add_success = t.AddKeyValuePair(var, { value_to_update }) && add_success;
        continue;
      }
      // If the variable exists in the table, grab the existing vector of statement numbers and update that
      vector<T1> var_to_stmts_lst = t.GetValueByKey(var);
      if (find(var_to_stmts_lst.begin(), var_to_stmts_lst.end(), value_to_update) == var_to_stmts_lst.end()) var_to_stmts_lst.push_back(value_to_update);
      add_success = t.UpdateKeyWithNewValue(var, var_to_stmts_lst) && add_success;
    }
    return add_success;
  } catch (exception& e) {
    return false;
  }
}

bool UsesVariableToStmtsTable::UpdateKeyValuePair(const int value_to_update, const vector<string>& keys) {
  return PopulateReverseRelationship<int>(keys, value_to_update, *this);
}

bool UsesVariableToProcsTable::UpdateKeyValuePair(const string& value_to_update, const vector<string>& keys) {
  return PopulateReverseRelationship<string>(keys, value_to_update, *this);
}

bool ModifiesVariableToStmtsTable::UpdateKeyValuePair(const int value_to_update, const vector<string>& keys) {
  return PopulateReverseRelationship<int>(keys, value_to_update, *this);
}

bool ModifiesVariableToProcsTable::UpdateKeyValuePair(const string& value_to_update, const vector<string>& keys) {
  return PopulateReverseRelationship<string>(keys, value_to_update, *this);
}
