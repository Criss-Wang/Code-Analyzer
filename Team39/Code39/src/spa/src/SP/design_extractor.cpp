#include "../PKB/pkb.h"
#include "./design_extractor.h"

using namespace std;

int Dfs(shared_ptr<RelListTable> table_to_refer, shared_ptr<RelListTable> table_to_update, int key) {
  if (table_to_update->KeyExistsInTable(key)) {
    return key;
  }

  if (!table_to_refer->KeyExistsInTable(key)) {
    return key;
  }

  vector<int> children_lst = table_to_refer->GetValueByKey(key);
  vector<int> ans;
  for (int child_key : children_lst) {
    int end_val = Dfs(table_to_refer, table_to_update, child_key);
    ans.push_back(end_val);
    // Add the children of the current key if the key exists
    if (table_to_update->KeyExistsInTable(end_val)) {
      // Merge the vectors of children
      vector<int> value = table_to_update->GetValueByKey(end_val);
      ans.insert(ans.end(), value.begin(), value.end());
    }
  }

  // Add into "cache" if ans is not empty. That means the current key must have children
  if (!ans.empty()) {
    table_to_update->AddKeyValuePair(key, ans);
  }

  return key;
}

void PopulateForPOrC(shared_ptr<RelListTable> table_to_refer, shared_ptr<RelListTable> table_to_update) {
  for (int& key : table_to_refer->GetKeyLst()) {
    Dfs(table_to_refer, table_to_update, key);
  }
}

template<typename T1, typename T2>
void PopulateForF(T1 table_to_refer, T2 table_to_update) {
  // While the stmt_1 exists in the table, keep adding stmt_2 such that follows*(stmt_1, stmt_2) holds into the vector
  for (const int key : table_to_refer->GetKeyLst()) {
    int stmt_1 = key;
    vector<int> follows_star_of_stmt;
    while (table_to_refer->KeyExistsInTable(stmt_1)) {
      int stmt_2 = table_to_refer->GetValueByKey(stmt_1);
      follows_star_of_stmt.push_back(stmt_2);
      stmt_1 = stmt_2;
    }
    table_to_update->AddKeyValuePair(key, follows_star_of_stmt);
  }
}

void PopulateNestedModifiesOrUses(RelListTable& parent_star_table, Table<int, vector<int>>& t) {
  for (const int parent_stmt: parent_star_table.GetKeyLst()) {
    vector<int> variables_lst;
    if (t.KeyExistsInTable(parent_stmt)) {
      variables_lst = t.GetValueByKey(parent_stmt);
    }
    vector<int> tmp_lst(variables_lst);

    for (const int child_stmt: parent_star_table.GetValueByKey(parent_stmt)) {
      if (!t.KeyExistsInTable(child_stmt)) {
        continue;
      }

      // Get the variables associated with the statement number
      vector<int> variables_lst_of_child_stmt = t.GetValueByKey(child_stmt);

      // Merge two vectors
      tmp_lst.insert(tmp_lst.end(), variables_lst_of_child_stmt.begin(), variables_lst_of_child_stmt.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }

    if (tmp_lst.empty()) {
      continue;
    }
    bool success = t.UpdateKeyWithNewValue(parent_stmt, tmp_lst);
  }
}

void PopulateReverseNestedModifiesOrUses(RelListTable& child_star_table, Table<int, vector<int>>& t) {
  for (const int var: t.GetKeyLst()) {
    vector<int> stmts_lst = t.GetValueByKey(var);
    vector<int> tmp_lst(stmts_lst);
    for (const int stmt: stmts_lst) {
      if (!child_star_table.KeyExistsInTable(stmt)) {
        continue;
      }

      const vector<int> parent_stmts_lst = child_star_table.GetValueByKey(stmt);
      if (parent_stmts_lst.empty()) {
        continue;
      }

      tmp_lst.insert(tmp_lst.end(), parent_stmts_lst.begin(), parent_stmts_lst.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }

    if (tmp_lst.empty()) {
      continue;
    }
    bool success = t.UpdateKeyWithNewValue(var, tmp_lst);
  }
}

void PopulateNestedModifiesPOrUsesP(RelListTable& calls_star_table, Table<int, vector<int>>& t) {
  for (const int proc : t.GetKeyLst()) {
    // Get the variables
    vector<int> variables = {};
    if (t.KeyExistsInTable(proc)) {
      variables = t.GetValueByKey(proc);
    }

    int initial_variables_size = variables.size();

    // Check what other procedures are called
    if (!calls_star_table.KeyExistsInTable(proc)) {
      continue;
    }

    vector<int> called_procedures = calls_star_table.GetValueByKey(proc);
    for (const int called_proc : called_procedures) {
      // Merge the vectors with new values
      if (!t.KeyExistsInTable(called_proc)) {
        continue;
      }
      vector<int> new_variables = t.GetValueByKey(called_proc);
      variables.insert(variables.end(), new_variables.begin(), new_variables.end());
    }
    // Remove duplicate elements
    sort(variables.begin(), variables.end());
    variables.erase(unique(variables.begin(), variables.end()), variables.end());

    // Update the key with new vector
    if (variables.size() > initial_variables_size) {
      t.UpdateKeyWithNewValue(proc, variables);
    }
  }
}

void PopulateReverseNestedModifiesPOrUsesP(RelListTable& called_by_star_table, Table<int, vector<int>>& t) {
  for (const int var : t.GetKeyLst()) {
    // Get the procedures associated with the variable
    vector<int> procedures = t.GetValueByKey(var);
    int initial_procedures_size = procedures.size();

    for (const int proc : procedures) {
      // Check if the procedure is called by others
      vector<int> callers = {};
      if (called_by_star_table.KeyExistsInTable(proc)) {
        callers = called_by_star_table.GetValueByKey(proc);
      }

      // Merge the vectors with new values
      procedures.insert(procedures.end(), callers.begin(), callers.end());
    }

    // Remove duplicate elements
    sort(procedures.begin(), procedures.end());
    procedures.erase(unique(procedures.begin(), procedures.end()), procedures.end());

    // Update the key with new vector
    if (initial_procedures_size != procedures.size()) {
      t.UpdateKeyWithNewValue(var, procedures);
    }
  }
}

void PopulateNestedModifiesSOrUsesSForCalls(EntityVarsTable caller_table, RelListTable& child_star_table,
  Table<int, vector<int>> proc_to_variables_table, Table<int, vector<int>>& t, Pkb& pkb) {
  // Get the call statements
  vector<int> call_stmts = caller_table.GetKeyLst();
  // Then loop through and get the specific procedure called at that statement
  for (const int call_stmt : call_stmts) {
    string proc = caller_table.GetValueByKey(call_stmt);
    int proc_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, proc);
    // Get the variables used in that procedure
    if (!proc_to_variables_table.KeyExistsInTable(proc_idx)) {
      continue;
    }

    vector<int> variables_idx = proc_to_variables_table.GetValueByKey(proc_idx);

    // Update the statement to variables table
    t.AddKeyValuePair(call_stmt, variables_idx);

    // Check the parents and update the relevant statement with the new variables
    if (!child_star_table.KeyExistsInTable(call_stmt)) {
      continue;
    }

    vector<int> parents = child_star_table.GetValueByKey(call_stmt);

    for (const int parent : parents) {
      // Try getting the variables associated with parent
      if (!t.KeyExistsInTable(parent)) {
        continue;
      }
      vector<int> parent_variables_idx = t.GetValueByKey(parent);

      // Otherwise merge the vectors with new values
      parent_variables_idx.insert(parent_variables_idx.end(), variables_idx.begin(), variables_idx.end());
      // Remove duplicate elements
      sort(parent_variables_idx.begin(), parent_variables_idx.end());
      parent_variables_idx.erase(unique(parent_variables_idx.begin(), parent_variables_idx.end()), parent_variables_idx.end());

      try {
        t.UpdateKeyWithNewValue(parent, parent_variables_idx);
      } catch (InvalidKeyException& e) {
        // Means that there are no variables associated with parent statement
        t.AddKeyValuePair(parent, variables_idx);
      }
    }
  }
}

void PopulateReverseNestedModifiesSOrUsesSForCalls(EntityVarsTable caller_table, RelListTable& child_star_table,
  Table<int, vector<int>> proc_to_variables_table, Table<int, vector<int>>& t, Pkb& pkb) {
  // First get the call statements
  vector<int> call_stmts = caller_table.GetKeyLst();
  // Then loop through and get the specific procedure called at that statement
  for (const int call_stmt : call_stmts) {
    string proc = caller_table.GetValueByKey(call_stmt);
    int proc_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, proc);
    // Get the variables modified or used in that procedure
    if (!proc_to_variables_table.KeyExistsInTable(proc_idx)) {
      continue;
    }

    vector<int> variables_idx = proc_to_variables_table.GetValueByKey(proc_idx);

    // Loop through the variables
    for (const int var_idx : variables_idx) {
      // Get the stmts associated with this variable
      if (!t.KeyExistsInTable(var_idx)) {
        continue;
      }

      // Ensure that duplicates are not added
      vector<int> stmts_to_update = t.GetValueByKey(var_idx);
      if (find(stmts_to_update.begin(), stmts_to_update.end(), call_stmt) == stmts_to_update.end()) {
        stmts_to_update.push_back(call_stmt);
      }

      t.UpdateKeyWithNewValue(var_idx, stmts_to_update);

      // Get the parents of the current call statement
      if (!child_star_table.KeyExistsInTable(call_stmt)) {
        continue;
      }
      vector<int> parents = child_star_table.GetValueByKey(call_stmt);

      for (const int parent : parents) {
        if (find(stmts_to_update.begin(), stmts_to_update.end(), parent) == stmts_to_update.end()) {
          stmts_to_update.push_back(parent);
        }
      }

      t.UpdateKeyWithNewValue(var_idx, stmts_to_update);
    }
  }
}

int PopulateNestedRelationships(Pkb& pkb) {
  try {
    shared_ptr<RelTable> follows_table = pkb.GetFollowsTable();
    shared_ptr<RelListTable> follows_star_table = pkb.GetFollowsStarTable();
    shared_ptr<RelTable> follows_before_table = pkb.GetFollowsBeforeTable();
    shared_ptr<RelListTable> follows_before_star_table = pkb.GetFollowsBeforeStarTable();
    shared_ptr<RelListTable> parent_table = pkb.GetParentTable();
    shared_ptr<RelListTable> child_table = pkb.GetChildTable();
    shared_ptr<RelListTable> parent_star_table = pkb.GetParentStarTable();
    shared_ptr<RelListTable> child_star_table = pkb.GetChildStarTable();
    shared_ptr<RelListTable> calls_table = pkb.GetCallsTable();
    shared_ptr<RelListTable> calls_star_table = pkb.GetCallsStarTable();
    shared_ptr<RelListTable> called_by_table = pkb.GetCalledByTable();
    shared_ptr<RelListTable> called_by_star_table = pkb.GetCalledByStarTable();
    shared_ptr<RelListTable> modifies_stmt_to_variables_table = pkb.GetModifiesStmtToVariablesTable();
    shared_ptr<RelListReverseTable> modifies_variable_to_stmts_table = pkb.GetModifiesVariableToStmtsTable();
    shared_ptr<RelListTable> modifies_proc_to_variables_table = pkb.GetModifiesProcToVariablesTable();
    shared_ptr<RelListReverseTable> modifies_variable_to_procs_table = pkb.GetModifiesVariableToProcsTable();
    shared_ptr<RelListTable> uses_stmt_to_variables_table = pkb.GetUsesStmtToVariablesTable();
    shared_ptr<RelListReverseTable> uses_variable_to_stmts_table = pkb.GetUsesVariableToStmtsTable();
    shared_ptr<RelListTable> uses_proc_to_variables_table = pkb.GetUsesProcToVariablesTable();
    shared_ptr<RelListReverseTable> uses_variable_to_procs_table = pkb.GetUsesVariableToProcsTable();
    shared_ptr<EntityVarsTable> caller_table = pkb.GetCallerTable();

    // Populate nested follows
    PopulateForF<shared_ptr<RelTable>, shared_ptr<RelListTable>>(follows_table, follows_star_table);
    PopulateForF<shared_ptr<RelTable>, shared_ptr<RelListTable>>(follows_before_table, follows_before_star_table);

    // Populate nested parent
    PopulateForPOrC(parent_table, parent_star_table);
    PopulateForPOrC(child_table, child_star_table);

    // Populate nested calls
    PopulateForPOrC(calls_table, calls_star_table);
    PopulateForPOrC(called_by_table, called_by_star_table);

    // Populate modifies
    PopulateNestedModifiesOrUses(*parent_star_table, *modifies_stmt_to_variables_table);
    PopulateReverseNestedModifiesOrUses(*child_star_table, *modifies_variable_to_stmts_table);

    // Populate uses
    PopulateNestedModifiesOrUses(*parent_star_table, *uses_stmt_to_variables_table);
    PopulateReverseNestedModifiesOrUses(*child_star_table, *uses_variable_to_stmts_table);

    // Populate modifiesP
    PopulateNestedModifiesPOrUsesP(*calls_star_table, *modifies_proc_to_variables_table);
    PopulateReverseNestedModifiesPOrUsesP(*called_by_star_table, *modifies_variable_to_procs_table);

    // Populate usesP
    PopulateNestedModifiesPOrUsesP(*calls_star_table, *uses_proc_to_variables_table);
    PopulateReverseNestedModifiesPOrUsesP(*called_by_star_table, *uses_variable_to_procs_table);

    // Populate ModifiesS with calls
    PopulateNestedModifiesSOrUsesSForCalls(*caller_table, *child_star_table, *modifies_proc_to_variables_table, *modifies_stmt_to_variables_table, pkb);
    PopulateReverseNestedModifiesSOrUsesSForCalls(*caller_table, *child_star_table, *modifies_proc_to_variables_table, *modifies_variable_to_stmts_table, pkb);

    // Populate UsesS with calls
    PopulateNestedModifiesSOrUsesSForCalls(*caller_table, *child_star_table, *uses_proc_to_variables_table, *uses_stmt_to_variables_table, pkb);
    PopulateReverseNestedModifiesSOrUsesSForCalls(*caller_table, *child_star_table, *uses_proc_to_variables_table, *uses_variable_to_stmts_table, pkb);
  } catch (exception& e) {
    throw e;
  }
  return 1;
}
