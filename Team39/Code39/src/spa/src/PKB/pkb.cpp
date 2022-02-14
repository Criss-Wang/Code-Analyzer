#include "pkb.h"
#include "Utility/helper.h"

#include <stack>

FollowsTable* Pkb::GetFollowsTable() {
  return follows_table_;
}

FollowsStarTable* Pkb::GetFollowsStarTable() {
  return follows_star_table_;
}

FollowsBeforeTable* Pkb::GetFollowsBeforeTable() {
  return follows_before_table_;
}

FollowsBeforeStarTable* Pkb::GetFollowsBeforeStarTable() {
  return follows_before_star_table_;
}

ParentTable* Pkb::GetParentTable() {
  return parent_table_;
}

ChildTable* Pkb::GetChildTable() {
  return child_table_;
}

ParentStarTable* Pkb::GetParentStarTable() {
  return parent_star_table_;
}

ChildStarTable* Pkb::GetChildStarTable() {
  return child_star_table_;
}

ModifiesStmtToVariablesTable* Pkb::GetModifiesStmtToVariablesTable() {
  return modifies_stmt_to_variables_table_;
}

ModifiesVariableToStmtsTable* Pkb::GetModifiesVariableToStmtsTable() {
  return modifies_variable_to_stmts_table_;
}

UsesStmtToVariablesTable* Pkb::GetUsesStmtToVariablesTable() {
  return uses_stmt_to_variables_table_;
}

UsesVariableToStmtsTable* Pkb::GetUsesVariableToStmtsTable() {
  return uses_variable_to_stmts_table_;
}

// Following are for search handlers
bool Pkb::IsParent(const int stmt_1, const int stmt_2) const {
  try {
    vector<int> parent_stmt_lst = child_table_->GetValueByKey(stmt_2);
    return find(parent_stmt_lst.begin(), parent_stmt_lst.end(), stmt_1) != parent_stmt_lst.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsTransitiveParent(const int stmt_1, const int stmt_2) const {
  try {
    vector<int> parent_stmt_lst = child_star_table_->GetValueByKey(stmt_2);
    return find(parent_stmt_lst.begin(), parent_stmt_lst.end(), stmt_1) != parent_stmt_lst.end();
  } catch (exception& e) {
    return false;
  }
}

vector<int> Pkb::GetParent(const int stmt) const {
  try {
    const vector<int> parent_stmt_lst = child_table_->GetValueByKey(stmt);
    return { parent_stmt_lst[0] };
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetAllParents(const int stmt) const {
  try {
    return child_star_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }

}

vector<int> Pkb::GetChild(const int stmt) const {
  try {
    return parent_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetAllChildren(const int stmt) const {
  try {
    return parent_star_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

template<typename T1, typename T2, typename T3>
vector<pair<T2, T3>> UnfoldResults(T1 table_to_unfold) {
  vector<pair<T2, T3>> result;
  for (const auto& [key, val_lst] : table_to_unfold->GetKeyValueLst()) {
    for (auto val_item : val_lst) {
      result.emplace_back(make_pair(key, val_item));
    }
  }
  return result;
}

vector<pair<int, int>> Pkb::GetAllParentPair(int stmt) const {
  try {
    return UnfoldResults<ParentTable*, int, int>(parent_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveParentPair(int stmt) const {
  try {
    return UnfoldResults<ParentStarTable*, int, int>(parent_star_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::IsFollows(const int stmt_1, const int stmt_2) const {
  try {
    return stmt_2 == follows_table_->GetValueByKey(stmt_1);
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsTransitiveFollows(const int stmt_1, const int stmt_2) const {
  try {
    vector<int> stmts_lst = follows_star_table_->GetValueByKey(stmt_1);
    return find(stmts_lst.begin(), stmts_lst.end(), stmt_2) != stmts_lst.end();
  } catch (exception& e) {
    return false;
  }
}

vector<int> Pkb::GetStmtRightBefore(const int stmt) const {
  try {
    return { follows_before_table_->GetValueByKey(stmt) };
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetStmtsBefore(const int stmt) const {
  try {
    return follows_before_star_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetStmtRightAfter(const int stmt) const {
  try {
    return { follows_table_->GetValueByKey(stmt) };
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetStmtsAfter(const int stmt) const {
  try {
    return follows_star_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<pair<int, int>> Pkb::GetAllFollowsPairs(int stmt) const {
  try {
    return UnfoldResults<FollowsTable*, int, int>(follows_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveFollowsPairs(int stmt) const {
  try {
    return UnfoldResults<FollowsStarTable*, int, int>(follows_star_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::AddParent(const int key, const vector<int>& value) {
  bool add_success = parent_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  for (const auto child : value) {
    add_success = child_table_->AddKeyValuePair(child, { key }) && add_success;
  }
  return add_success;
}

bool Pkb::AddFollows(const int key, const int value) {
  bool add_success = follows_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  add_success = follows_before_table_->AddKeyValuePair(value, key) && add_success;
  return add_success;
}

bool Pkb::AddModifies(const int key, const vector<string>& value) {
  bool add_success = modifies_stmt_to_variables_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  add_success = add_success && modifies_variable_to_stmts_table_->UpdateKeyValuePair(key, value);
  return add_success;
}

bool Pkb::AddUses(const int key, const vector<string>& value) {
  bool add_success = uses_stmt_to_variables_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  add_success = add_success && uses_variable_to_stmts_table_->UpdateKeyValuePair(key, value);
  return add_success;
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kParent: return AddParent(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

unordered_set<int> Pkb::GetAllStmtWithPattern(const string& pattern) const {
  const string usable_pattern = PatternHelper::PreprocessPattern(pattern);
  unordered_set<int> empty_set{};
  unordered_set<int> res{};

  for (auto s: PatternHelper::GetPatternSet(usable_pattern)) {
    if (!pattern_to_stmts_table_->KeyExistsInTable(s)) return empty_set;
    unordered_set<int> stmt_lst = pattern_to_stmts_table_->GetValueByKey(s);
    if (res.empty()) {
      res = stmt_lst;
    } else {
      for (auto stmt : res) {
        if (stmt_lst.find(stmt) == stmt_lst.end()) res.erase(stmt);
      }
    }
  }
  return res;
}

bool Pkb::AddPattern(const int line_num, const string& input) {
  // First the SP side should guarantee a valid input is sent
  // We then proceed to parse the set of valid substring patterns
  const string clean_input = PatternHelper::PreprocessPattern(input);
  const unordered_set<string> valid_sub_patterns = PatternHelper::GetPatternSet(clean_input);
  bool add_success = stmt_to_patterns_table_->AddKeyValuePair(line_num, valid_sub_patterns);
  for (auto p: valid_sub_patterns) {
    if (!pattern_to_stmts_table_->KeyExistsInTable(p)) {
      add_success = add_success && pattern_to_stmts_table_->AddKeyValuePair(p, unordered_set<int>{line_num});
    }
    else {
      unordered_set<int> current_set = pattern_to_stmts_table_->GetValueByKey(p);
      current_set.insert(line_num);
      add_success = add_success && pattern_to_stmts_table_->UpdateKeyWithNewValue(p, current_set);
    }
  }
  return add_success;
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<string>& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kIf: return if_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kWhile: return while_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kModifiesStmtToVar: return AddModifies(key, value);
      case TableIdentifier::kUsesStmtToVar: return AddUses(key, value);
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
      case TableIdentifier::kFollows: return AddFollows(key, value);
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
      case TableIdentifier::kPattern: return AddPattern(key, value);
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
