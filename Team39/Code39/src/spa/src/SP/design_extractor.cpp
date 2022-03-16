#include "PKB/pkb.h"
#include "./design_extractor.h"

// Helper
template<typename T1, typename T2>
int Dfs(T1 table_to_refer, T2 table_to_update, int key) {
  if (table_to_update->KeyExistsInTable(key)) {
    return key;
  }

  vector<int> children_lst;
  try {
    children_lst = table_to_refer->GetValueByKey(key);
  } catch (InvalidKeyException& e) {
    return key;
  }

  vector<int> ans;
  for (int child_key : children_lst) {
    int end_val = Dfs<T1, T2>(table_to_refer, table_to_update, child_key);
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

template<typename T1, typename T2>
void PopulateForPOrC(T1 table_to_refer, T2 table_to_update) {
  for (int& key : table_to_refer->GetKeyLst()) {
    Dfs<T1, T2>(table_to_refer, table_to_update, key);
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

void PopulateNestedModifiesOrUses(ParentStarTable& parent_star_table, ChildStarTable& child_star_table, Table<int, vector<int>>& t,
  Table<int, vector<int>>& t2) {
  for (const int parent_stmt: parent_star_table.GetKeyLst()) {
    vector<int> variables_lst;
    if (t.KeyExistsInTable(parent_stmt)) {
      variables_lst = t.GetValueByKey(parent_stmt);
    }
    vector<int> tmp_lst(variables_lst);

    for (const int child_stmt: parent_star_table.GetValueByKey(parent_stmt)) {
      if (!t.KeyExistsInTable(child_stmt)) continue;
      // Get the variables associated with the statement number
      vector<int> variables_lst_of_child_stmt = t.GetValueByKey(child_stmt);

      // Merge two vectors
      tmp_lst.insert(tmp_lst.end(), variables_lst_of_child_stmt.begin(), variables_lst_of_child_stmt.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }

    if (tmp_lst.empty()) return;
    bool success = t.UpdateKeyWithNewValue(parent_stmt, tmp_lst);
  }

  // Populate the inverse relation
  for (const int var: t2.GetKeyLst()) {
    vector<int> stmts_lst = t2.GetValueByKey(var);
    vector<int> tmp_lst(stmts_lst);
    for (const int stmt: stmts_lst) {
      if (!child_star_table.KeyExistsInTable(stmt)) continue;
      const vector<int> parent_stmts_lst = child_star_table.GetValueByKey(stmt);
      if (parent_stmts_lst.empty()) continue;
      tmp_lst.insert(tmp_lst.end(), parent_stmts_lst.begin(), parent_stmts_lst.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }

    if (tmp_lst.empty()) return;
    bool success = t2.UpdateKeyWithNewValue(var, tmp_lst);
  }
}

void PopulateNestedModifiesPOrUsesP(CallsStarTable& calls_star_table, Table<int, vector<int>>& t) {
  for (const int proc : t.GetKeyLst()) {
    // Get the variables
    vector<int> variables = t.GetValueByKey(proc);
    // Check what other procedures are called
    vector<int> called_procedures;
    try {
      called_procedures = calls_star_table.GetValueByKey(proc);
    } catch (InvalidKeyException& e) {
      // That means this procedure does not call any other procedures
    }
    for (const int called_proc : called_procedures) {
      // Merge the vectors with new values
      vector<int> new_variables = t.GetValueByKey(called_proc);
      variables.insert(variables.end(), new_variables.begin(), new_variables.end());
    }
    // Remove duplicate elements
    sort(variables.begin(), variables.end());
    variables.erase(unique(variables.begin(), variables.end()), variables.end());

    // Update the key with new vector
    if (called_procedures.size() > 0) {
      t.UpdateKeyWithNewValue(proc, variables);
    }
  }
}

void PopulateReverseNestedModifiesPOrUsesP(CalledByStarTable& called_by_star_table, Table<int, vector<int>>& t) {
  for (const int var : t.GetKeyLst()) {
    // Get the procedures associated with the variable
    vector<int> procedures = t.GetValueByKey(var);
    int initial_procedures_size = procedures.size();

    for (const int proc : procedures) {
      // Check if the procedure is called by others
      vector<int> callers;
      try {
        callers = called_by_star_table.GetValueByKey(proc);
      } catch (InvalidKeyException& e) {
        // That means the procedure is not called by any other procedures
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

int PopulateNestedRelationships(Pkb& pkb) {
  try {
    FollowsTable* follows_table = pkb.GetFollowsTable();
    FollowsStarTable* follows_star_table = pkb.GetFollowsStarTable();
    FollowsBeforeTable* follows_before_table = pkb.GetFollowsBeforeTable();
    FollowsBeforeStarTable* follows_before_star_table = pkb.GetFollowsBeforeStarTable();
    ParentTable* parent_table = pkb.GetParentTable();
    ChildTable* child_table = pkb.GetChildTable();
    ParentStarTable* parent_star_table = pkb.GetParentStarTable();
    ChildStarTable* child_star_table = pkb.GetChildStarTable();
    CallsTable* calls_table = pkb.GetCallsTable();
    CallsStarTable* calls_star_table = pkb.GetCallsStarTable();
    CalledByTable* called_by_table = pkb.GetCalledByTable();
    CalledByStarTable* called_by_star_table = pkb.GetCalledByStarTable();
    ModifiesStmtToVariablesTable* modifies_stmt_to_variables_table = pkb.GetModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable* modifies_variable_to_stmts_table = pkb.GetModifiesVariableToStmtsTable();
    ModifiesProcToVariablesTable* modifies_proc_to_variables_table = pkb.GetModifiesProcToVariablesTable();
    ModifiesVariableToProcsTable* modifies_variable_to_procs_table = pkb.GetModifiesVariableToProcsTable();
    UsesStmtToVariablesTable* uses_stmt_to_variables_table = pkb.GetUsesStmtToVariablesTable();
    UsesVariableToStmtsTable* uses_variable_to_stmts_table = pkb.GetUsesVariableToStmtsTable();
    UsesProcToVariablesTable* uses_proc_to_variables_table = pkb.GetUsesProcToVariablesTable();
    UsesVariableToProcsTable* uses_variable_to_procs_table = pkb.GetUsesVariableToProcsTable();

    // Populate nested follows
    PopulateForF<FollowsTable*, FollowsStarTable*>(follows_table, follows_star_table);
    PopulateForF<FollowsBeforeTable*, FollowsBeforeStarTable*>(follows_before_table, follows_before_star_table);

    // Populate nested parent
    PopulateForPOrC<ParentTable*, ParentStarTable*>(parent_table, parent_star_table);
    PopulateForPOrC<ChildTable*, ChildStarTable*>(child_table, child_star_table);

    // Populate nested calls
    PopulateForPOrC<CallsTable*, CallsStarTable*>(calls_table, calls_star_table);
    PopulateForPOrC<CalledByTable*, CalledByStarTable*>(called_by_table, called_by_star_table);

    // Populate modifies
    PopulateNestedModifiesOrUses(*parent_star_table, *child_star_table,  *modifies_stmt_to_variables_table, *modifies_variable_to_stmts_table);

    // Populate uses
    PopulateNestedModifiesOrUses(*parent_star_table, *child_star_table,  *uses_stmt_to_variables_table, *uses_variable_to_stmts_table);

    // Populate modifiesP
    PopulateNestedModifiesPOrUsesP(*calls_star_table, *modifies_proc_to_variables_table);
    PopulateReverseNestedModifiesPOrUsesP(*called_by_star_table, *modifies_variable_to_procs_table);

    // Populate usesP
    PopulateNestedModifiesPOrUsesP(*calls_star_table, *uses_proc_to_variables_table);
    PopulateReverseNestedModifiesPOrUsesP(*called_by_star_table, *uses_variable_to_procs_table);
  } catch (exception& e) {
    return 0;
  }
  return 1;
}
