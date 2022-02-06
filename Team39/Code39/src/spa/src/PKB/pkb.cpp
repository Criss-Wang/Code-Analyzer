#include "pkb.h"

#include <queue>

int Pkb::PopulateNestedRelationship() {
  try {
    PopulateNestedFollows();
    PopulateNestedParents();
    //PopulateUses();
    PopulateModifies();
  } catch (exception& e) {
    return 0;
  }
  return 1;
}

// TODO(Zhenlin): [Performance] Not optimized with the help of traversal order
// TODO(Zhenlin): [SE Practice] Code duplication exists, but not sure how to resolve
void Pkb::PopulateNestedParents() {
  for (const auto key: parent_table_->GetKeyLst()) {
    vector<int> current_key_child_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      if (!parent_table_->KeyExistsInTable(q.front())) {
        q.pop();
        continue;
      }
      for (auto child_key: parent_table_->GetValueByKey(q.front())) {
        current_key_child_lst.push_back(child_key);
        q.push(child_key);
      }
      q.pop();
    }
    if (parent_star_table_->KeyExistsInTable(key)) {
      parent_star_table_->UpdateKeyWithNewValue(key, current_key_child_lst);
    } else {
      parent_star_table_->AddKeyValuePair(key, current_key_child_lst);
    }
  }

  // Populate the reverse nested relationships
  for (const auto key : child_table_->GetKeyLst()) {
    vector<int> current_key_parent_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      if (!child_table_->KeyExistsInTable(q.front())) {
        q.pop();
        continue;
      }
      for (auto parent_key : child_table_->GetValueByKey(q.front())) {
        current_key_parent_lst.push_back(parent_key);
        q.push(parent_key);
      }
      q.pop();
    }
    if (child_star_table_->KeyExistsInTable(key)) {
      child_star_table_->UpdateKeyWithNewValue(key, current_key_parent_lst);
    } else {
      child_star_table_->AddKeyValuePair(key, current_key_parent_lst);
    }
  }
}

void Pkb::PopulateModifies() {
  for (const auto parent_stmt: parent_star_table_->GetKeyLst()) {
    vector<string> current_stmt_modifies_lst;
    if (modifies_stmt_to_variables_table_->KeyExistsInTable(parent_stmt)) {
      current_stmt_modifies_lst = modifies_stmt_to_variables_table_->GetValueByKey(parent_stmt);
    }
    vector<string> tmp_lst(current_stmt_modifies_lst);
    for (const auto child_stmt: parent_star_table_->GetValueByKey(parent_stmt)) {
      vector<string> child_stmt_modifies_lst;
      if (!modifies_stmt_to_variables_table_->KeyExistsInTable(child_stmt)) continue;
      child_stmt_modifies_lst = modifies_stmt_to_variables_table_->GetValueByKey(child_stmt);
      // Merge two vectors
      tmp_lst.insert(tmp_lst.end(), child_stmt_modifies_lst.begin(), child_stmt_modifies_lst.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }
    if (tmp_lst.empty()) return;
    bool success = modifies_stmt_to_variables_table_->UpdateKeyWithNewValue(parent_stmt, tmp_lst);
  }

  for (const auto var: modifies_variable_to_stmts_table_->GetKeyLst()) {
    vector<int> current_stmt_modifying_lst = modifies_variable_to_stmts_table_->GetValueByKey(var);
    vector<int> tmp_lst(current_stmt_modifying_lst);
    for (const auto stmt_modifying: current_stmt_modifying_lst) {
      const vector<int> parent_stmt_lst = child_star_table_->GetValueByKey(stmt_modifying);
      if (parent_stmt_lst.empty()) continue;
      tmp_lst.insert(tmp_lst.end(), parent_stmt_lst.begin(), parent_stmt_lst.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }
    if (tmp_lst.empty()) return;
    bool success = modifies_variable_to_stmts_table_->UpdateKeyWithNewValue(var, tmp_lst);
  }

}

void Pkb::PopulateNestedFollows() {
  // While the stmt_1 exists in the table, keep adding stmt_2 such that follows*(stmt_1, stmt_2) holds into the vector
  for (const int key: follows_table_->GetKeyLst()) {
    int stmt_1 = key;
    vector<int> follows_star_of_stmt1;
    while (follows_table_->KeyExistsInTable(stmt_1)) {
      int stmt_2 = follows_table_->GetValueByKey(stmt_1);
      follows_star_of_stmt1.push_back(stmt_2);
      stmt_1 = stmt_2;
    }
    follows_star_table_->AddKeyValuePair(key, follows_star_of_stmt1);
  }

  // Populate the reverse nested relationships
  for (const int key: follows_before_table_->GetKeyLst()) {
    int stmt_2 = key;
    vector<int> follows_before_star_of_stmt_2;
    while (follows_before_table_->KeyExistsInTable(stmt_2)) {
      int stmt_1 = follows_before_table_->GetValueByKey(stmt_2);
      follows_before_star_of_stmt_2.push_back(stmt_1);
      stmt_2 = stmt_1;
    }
    follows_before_star_table_->AddKeyValuePair(key, follows_before_star_of_stmt_2);
  }
}

bool Pkb::IsParent(const int stmt_1, const int stmt_2) const {
  vector<int> parent_stmt_lst = child_table_->GetValueByKey(stmt_2);
  return find(parent_stmt_lst.begin(), parent_stmt_lst.end(), stmt_1) != parent_stmt_lst.end();
}

bool Pkb::IsTransitiveParent(const int stmt_1, const int stmt_2) const {
  vector<int> parent_stmt_lst = child_star_table_->GetValueByKey(stmt_2);
  return find(parent_stmt_lst.begin(), parent_stmt_lst.end(), stmt_1) != parent_stmt_lst.end();
}

int Pkb::GetParent(const int stmt) const {
  const vector<int> parent_stmt_lst = child_table_->GetValueByKey(stmt);
  return parent_stmt_lst[0];
}

vector<int> Pkb::GetAllParents(const int stmt) const {
  return child_star_table_->GetValueByKey(stmt);
}

vector<int> Pkb::GetChild(const int stmt) const {
  return parent_table_->GetValueByKey(stmt);
}

vector<int> Pkb::GetAllChildren(const int stmt) const {
  return parent_star_table_->GetValueByKey(stmt);
}

bool Pkb::IsFollows(const int stmt_1, const int stmt_2) const {
  return stmt_2 == follows_table_->GetValueByKey(stmt_1);
}

bool Pkb::IsTransitiveFollows(const int stmt_1, const int stmt_2) const {
  vector<int> stmts_lst = follows_star_table_->GetValueByKey(stmt_1);
  return find(stmts_lst.begin(), stmts_lst.end(), stmt_2) != stmts_lst.end();
}

int Pkb::GetStmtRightBefore(const int stmt) const {
  return follows_before_table_->GetValueByKey(stmt);
}

vector<int> Pkb::GetStmtsBefore(const int stmt) const {
  return follows_before_star_table_->GetValueByKey(stmt);
}

int Pkb::GetStmtRightAfter(const int stmt) const {
  return follows_table_->GetValueByKey(stmt);
}

vector<int> Pkb::GetStmtsAfter(const int stmt) const {
  return follows_star_table_->GetValueByKey(stmt);
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kParent: {
        bool add_success = parent_table_->AddKeyValuePair(key, value);
        // Populate the reverse relation
        for (const auto child : value) {
          add_success = child_table_->AddKeyValuePair(child, { key }) && add_success;
        }
        return add_success;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<string>& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kIf: return if_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kWhile: return while_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kModifiesStmtToVar: {
        bool add_success = modifies_stmt_to_variables_table_->AddKeyValuePair(key, value);
        // Populate the reverse relation
        add_success = add_success && modifies_variable_to_stmts_table_->UpdateKeyValuePair(key, value);
        return add_success;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const int value) {
  try {
    if (value < 1) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kFollows: {
        bool add_success = follows_table_->AddKeyValuePair(key, value);
        // Populate the reverse relation
        add_success = follows_before_table_->AddKeyValuePair(value, key) && add_success;
        return add_success;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const string& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kAssign: return assign_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kRead: return read_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kPrint: return print_table_->AddKeyValuePair(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddEntityToSet(const EntityIdentifier entity_identifier, const int entity_val) {
  try {
    switch (entity_identifier) {
      case EntityIdentifier::kStmt: {
        stmt_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kAssign: {
        assign_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kRead: {
        read_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kPrint: {
        print_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kCall: {
        call_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kConstant: {
        constant_set_.insert(entity_val);
        return true;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddEntityToSet(const EntityIdentifier entity_identifier, const string& entity_val) {
  try {
    switch (entity_identifier) {
      case EntityIdentifier::kVariable: {
        variable_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kProc: {
        procedure_set_.insert(entity_val);
        return true;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddEntityToSet(const EntityIdentifier entity_identifier, const set<int>& entity_val) {
  try {
    switch (entity_identifier) {
      case EntityIdentifier::kStmtLst: {
        stmt_list_set_.insert(entity_val);
        return true;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

unordered_set<int> Pkb::GetAllEntityInt(const EntityIdentifier entity_identifier) {
  switch (entity_identifier) {
    case EntityIdentifier::kStmt: return stmt_set_;
    case EntityIdentifier::kAssign: return assign_set_;
    case EntityIdentifier::kRead: return read_set_;
    case EntityIdentifier::kPrint: return print_set_;
    case EntityIdentifier::kCall: return call_set_;
    case EntityIdentifier::kConstant: return constant_set_;
    default:
      throw InvalidIdentifierException();
  }
}

unordered_set<string> Pkb::GetAllEntityString(const EntityIdentifier entity_identifier) {
  switch (entity_identifier) {
    case EntityIdentifier::kVariable: return variable_set_;
    case EntityIdentifier::kProc: return procedure_set_;
    default:
      throw InvalidIdentifierException();
  }
}

unordered_set<set<int>, HashFunction> Pkb::GetAllEntityStmtLst(const EntityIdentifier entity_identifier) {
  switch (entity_identifier) {
    case EntityIdentifier::kStmtLst: return stmt_list_set_;
    default:
      throw InvalidIdentifierException();
  }
}
