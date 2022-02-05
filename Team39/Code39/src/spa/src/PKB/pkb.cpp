#include "pkb.h"

#include <queue>

// TODO(Zhenlin): https://github.com/nus-cs3203/21s2-cp-spa-team-39/issues/20
int Pkb::PopulateNestedRelationship() {
  try {
    //PopulateNestedFollows();
    PopulateNestedParents();
    //PopulateUses();
    PopulateModifies();
  } catch (exception& e) {
    return 0;
  }
  return 1;
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

// TODO(Zhenlin): [Performance] Not optimized with the help of traversal order
// TODO(Zhenlin): [SE Practice] Code duplication exists, but not sure how to resolve
void Pkb::PopulateNestedParents() {
  for (const auto key: parent_table_->GetKeyLst()) {
    vector<int> current_key_child_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      for (auto child_key: parent_table_->GetValueByKey(q.front())) {
        current_key_child_lst.push_back(child_key);
        q.push(child_key);
      }
      q.pop();
    }
    parent_star_table_->AddKeyValuePair(key, current_key_child_lst);
  }

  for (const auto key : child_table_->GetKeyLst()) {
    vector<int> current_key_parent_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      for (auto parent_key : child_table_->GetValueByKey(q.front())) {
        current_key_parent_lst.push_back(parent_key);
        q.push(parent_key);
      }
      q.pop();
    }
    child_star_table_->AddKeyValuePair(key, current_key_parent_lst);
  }
}


void Pkb::PopulateModifies() {
  for (const auto parent_stmt: parent_star_table_->GetKeyLst()) {
    const vector<string> current_stmt_modifies_lst = modifies_stmt_to_variables_table_->GetValueByKey(parent_stmt);
    vector<string> tmp_lst(current_stmt_modifies_lst);
    for (const auto child_stmt: parent_star_table_->GetValueByKey(parent_stmt)) {
      vector<string> child_stmt_modifies_lst = modifies_stmt_to_variables_table_->GetValueByKey(child_stmt);
      // Merge two vectors
      tmp_lst.insert(tmp_lst.end(), child_stmt_modifies_lst.begin(), child_stmt_modifies_lst.end());
      // Remove duplicate elements
      sort(tmp_lst.begin(), tmp_lst.end());
      tmp_lst.erase(unique(tmp_lst.begin(), tmp_lst.end()), tmp_lst.end());
    }
    bool success = modifies_stmt_to_variables_table_->SetKeyValuePair(parent_stmt, tmp_lst);
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
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
  try {
    if (value.empty()) throw InvalidValueException();
    switch (table_identifier) {
    case TableIdentifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
    case TableIdentifier::kParent: {
      bool add_success = parent_table_->AddKeyValuePair(key, value);
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
    if (value.empty()) throw InvalidValueException();
    switch (table_identifier) {
    case TableIdentifier::kIf: return if_table_->AddKeyValuePair(key, value);
    case TableIdentifier::kWhile: return while_table_->AddKeyValuePair(key, value);
    case TableIdentifier::kModifiesStmtToVar: {
      bool add_success = modifies_stmt_to_variables_table_->AddKeyValuePair(key, value);
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
    if (value < 1) throw InvalidValueException();
    switch (table_identifier) {
    case TableIdentifier::kFollowsBy: return follows_by_table_->AddKeyValuePair(key, value);
    case TableIdentifier::kFollowsAfter: return follows_after_table_->AddKeyValuePair(key, value);
    default:
      throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const string& value) {
  try {
    if (value.empty()) throw InvalidValueException();
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

bool Pkb::AddEntityToSet(const EntityIdentifier table_identifier, const int entity_val) {
  try {
    switch (table_identifier) {
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

bool Pkb::AddEntityToSet(const EntityIdentifier table_identifier, const string& entity_val) {
  try {
    switch (table_identifier) {
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

bool Pkb::AddEntityToSet(const EntityIdentifier table_identifier, const set<int>& entity_val) {
  try {
    switch (table_identifier) {
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

unordered_set<int> Pkb::GetAllEntityInt(const EntityIdentifier table_identifier) {
  switch (table_identifier) {
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

unordered_set<string> Pkb::GetAllEntityString(const EntityIdentifier table_identifier) {
  switch (table_identifier) {
  case EntityIdentifier::kVariable: return variable_set_;
  case EntityIdentifier::kProc: return procedure_set_;
  default:
    throw InvalidIdentifierException();
  }
}

unordered_set<set<int>, HashFunction> Pkb::GetAllEntityStmtLst(const EntityIdentifier table_identifier) {
  switch (table_identifier) {
  case EntityIdentifier::kStmtLst: return stmt_list_set_;
  default:
    throw InvalidIdentifierException();
  }
}