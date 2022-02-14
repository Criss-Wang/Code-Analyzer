#include <queue>

#include "PKB/pkb.h"
#include "./design_extractor.h"

// TODO(Zhenlin): [Performance] Not optimized with the help of traversal order
template<typename T1, typename T2>
void PopulateForP(T1 table_to_refer, T2 table_to_update) {
  for (const auto key : table_to_refer.GetKeyLst()) {
    vector<int> current_key_child_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      if (!table_to_refer.KeyExistsInTable(q.front())) {
        q.pop();
        continue;
      }
      for (auto child_key : table_to_refer.GetValueByKey(q.front())) {
        current_key_child_lst.push_back(child_key);
        q.push(child_key);
      }
      q.pop();
    }
    if (table_to_update.KeyExistsInTable(key)) {
      table_to_update.UpdateKeyWithNewValue(key, current_key_child_lst);
    } else {
      table_to_update.AddKeyValuePair(key, current_key_child_lst);
    }
  }
}

template<typename T1, typename T2>
void PopulateForF(T1 table_to_refer, T2 table_to_update) {
  // While the stmt_1 exists in the table, keep adding stmt_2 such that follows*(stmt_1, stmt_2) holds into the vector
  for (const int key : table_to_refer.GetKeyLst()) {
    int stmt_1 = key;
    vector<int> follows_star_of_stmt;
    while (table_to_refer.KeyExistsInTable(stmt_1)) {
      int stmt_2 = table_to_refer.GetValueByKey(stmt_1);
      follows_star_of_stmt.push_back(stmt_2);
      stmt_1 = stmt_2;
    }
    table_to_update.AddKeyValuePair(key, follows_star_of_stmt);
  }
}

void PopulateNestedModifiesOrUses(ParentStarTable& parent_star_table, ChildStarTable& child_star_table, Table<int, vector<string>>& t,
  Table<string, vector<int>>& t2) {
  for (const int parent_stmt: parent_star_table.GetKeyLst()) {
    vector<string> variables_lst;
    if (t.KeyExistsInTable(parent_stmt)) {
      variables_lst = t.GetValueByKey(parent_stmt);
    }
    vector<string> tmp_lst(variables_lst);

    for (const int child_stmt: parent_star_table.GetValueByKey(parent_stmt)) {
      if (!t.KeyExistsInTable(child_stmt)) continue;
      // Get the variables associated with the statement number
      vector<string> variables_lst_of_child_stmt = t.GetValueByKey(child_stmt);

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
  for (const string var: t2.GetKeyLst()) {
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

int PopulateNestedRelationships(Pkb& pkb) {
  try {
    FollowsTable follows_table = pkb.GetFollowsTable();
    FollowsStarTable follows_star_table = pkb.GetFollowsStarTable();
    FollowsBeforeTable follows_before_table = pkb.GetFollowsBeforeTable();
    FollowsBeforeStarTable follows_before_star_table = pkb.GetFollowsBeforeStarTable();
    ParentTable parent_table = pkb.GetParentTable();
    ChildTable child_table = pkb.GetChildTable();
    ParentStarTable parent_star_table = pkb.GetParentStarTable();
    ChildStarTable child_star_table = pkb.GetChildStarTable();
    ModifiesStmtToVariablesTable modifies_stmt_to_variables_table = pkb.GetModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable modifies_variable_to_stmts_table = pkb.GetModifiesVariableToStmtsTable();
    UsesStmtToVariablesTable uses_stmt_to_variables_table = pkb.GetUsesStmtToVariablesTable();
    UsesVariableToStmtsTable uses_variable_to_stmts_table = pkb.GetUsesVariableToStmtsTable();

    // Populate nested follows
    PopulateForF<FollowsTable, FollowsStarTable>(follows_table, follows_star_table);
    PopulateForF<FollowsBeforeTable, FollowsBeforeStarTable>(follows_before_table, follows_before_star_table);

    // Populate nested parent
    PopulateForP<ParentTable, ParentStarTable>(parent_table, parent_star_table);
    PopulateForP<ChildTable, ChildStarTable>(child_table, child_star_table);

    // Populate modifies
    PopulateNestedModifiesOrUses(parent_star_table, child_star_table,  modifies_stmt_to_variables_table, modifies_variable_to_stmts_table);

    // Populate uses
    PopulateNestedModifiesOrUses(parent_star_table, child_star_table,  uses_stmt_to_variables_table, uses_variable_to_stmts_table);
  } catch (exception& e) {
    return 0;
  }
  return 1;
}
