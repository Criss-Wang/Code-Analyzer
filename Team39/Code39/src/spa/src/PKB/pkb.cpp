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

CallsTable* Pkb::GetCallsTable() {
  return calls_table_;
}

CallsStarTable* Pkb::GetCallsStarTable() {
  return calls_star_table_;
}

CalledByTable* Pkb::GetCalledByTable() {
  return called_by_table_;
}

CalledByStarTable* Pkb::GetCalledByStarTable() {
  return called_by_star_table_;
}

ModifiesStmtToVariablesTable* Pkb::GetModifiesStmtToVariablesTable() {
  return modifies_stmt_to_variables_table_;
}

ModifiesVariableToStmtsTable* Pkb::GetModifiesVariableToStmtsTable() {
  return modifies_variable_to_stmts_table_;
}

ModifiesProcToVariablesTable* Pkb::GetModifiesProcToVariablesTable() {
  return modifies_proc_to_variables_table_;
}

ModifiesVariableToProcsTable* Pkb::GetModifiesVariableToProcsTable() {
  return modifies_variable_to_procs_table_;
}

UsesStmtToVariablesTable* Pkb::GetUsesStmtToVariablesTable() {
  return uses_stmt_to_variables_table_;
}

UsesVariableToStmtsTable* Pkb::GetUsesVariableToStmtsTable() {
  return uses_variable_to_stmts_table_;
}

UsesProcToVariablesTable* Pkb::GetUsesProcToVariablesTable() {
  return uses_proc_to_variables_table_;
}

UsesVariableToProcsTable* Pkb::GetUsesVariableToProcsTable() {
  return uses_variable_to_procs_table_;
}

CallerTable* Pkb::GetCallerTable() {
  return caller_table_;
}

// Following are for search handlers
bool Pkb::IsCalls(const int proc_1, const int proc_2) const {
  try {
    vector<int> callees = calls_table_->GetValueByKey(proc_1);
    return find(callees.begin(), callees.end(), proc_2) != callees.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsCallsExists() const {
  return calls_table_->GetTableSize() > 0;
}

bool Pkb::IsTransitiveCalls(const int proc_1, const int proc_2) const {
  try {
    vector<int> callees_star = calls_star_table_->GetValueByKey(proc_1);
    return find(callees_star.begin(), callees_star.end(), proc_2) != callees_star.end();
  } catch (exception& e) {
    return false;
  }
}

vector<int> Pkb::GetCallers(const int proc) const {
  try {
    return called_by_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetAllCallers(const int proc) const {
  try {
    return called_by_star_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetCallees(const int proc) const {
  try {
    return calls_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetAllCallees(const int proc) const {
  try {
    return calls_star_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<int>{};
  }
}

bool Pkb::IsParent(const int stmt_1, const int stmt_2) const {
  try {
    vector<int> parent_stmt_lst = child_table_->GetValueByKey(stmt_2);
    return find(parent_stmt_lst.begin(), parent_stmt_lst.end(), stmt_1) != parent_stmt_lst.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsParentExists() const {
  return child_table_->GetTableSize() > 0;
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

// Does not work if the table's value is not a vector
template<typename T1>
vector<pair<int, int>> UnfoldResults(T1 table_to_unfold) {
  vector<pair<int, int>> result;
  for (const auto& [key, val_lst] : table_to_unfold->GetKeyValueLst()) {
    for (auto val_item : val_lst) {
      result.emplace_back(make_pair(key, val_item));
    }
  }
  return result;
}

vector<pair<int, int>> Pkb::GetAllCallsPairs() const {
  try {
    return UnfoldResults<CallsTable*>(calls_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveCallsPairs() const {
  try {
    return UnfoldResults<CallsStarTable*>(calls_star_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllParentPairs() const {
  try {
    return UnfoldResults<ParentTable*>(parent_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveParentPairs() const {
  try {
    return UnfoldResults<ParentStarTable*>(parent_star_table_);
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

bool Pkb::IsFollowsExists() const {
  return follows_table_->GetTableSize() > 0;
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

vector<pair<int, int>> Pkb::GetAllFollowsPairs() const {
  try {
    vector<pair<int, int>> result;
    for (const auto& [key, val] : follows_table_->GetKeyValueLst()) {
      result.emplace_back(make_pair(key, val));
    }
    return result;
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveFollowsPairs() const {
  try {
    return UnfoldResults<FollowsStarTable*>(follows_star_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::IsUsesStmt(const int stmt, const int var_idx) const {
  try {
    vector<int> value = uses_stmt_to_variables_table_->GetValueByKey(stmt);
    return find(value.begin(), value.end(), var_idx) != value.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsUsesStmtExists() const {
  return uses_stmt_to_variables_table_->GetTableSize() > 0;
}

vector<int> Pkb::GetUsesStmtsByVar(const int var_idx) const {
  try {
    return uses_variable_to_stmts_table_->GetValueByKey(var_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetUsesVarByStmt(const int stmt) const {
  try {
    return uses_stmt_to_variables_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<pair<int, int>> Pkb::GetAllUsesStmtVarPairs() const {
  try {
    return UnfoldResults<UsesStmtToVariablesTable*>(uses_stmt_to_variables_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::IsProcUsesVar(const int proc_idx, const int var_idx) const {
  try {
    vector<int> value = uses_proc_to_variables_table_->GetValueByKey(proc_idx);
    return find(value.begin(), value.end(), var_idx) != value.end();
  } catch (exception& e) {
    return false;
  }
}

vector<int> Pkb::GetUsesProcsByVar(const int var_idx) const {
  try {
    return uses_variable_to_procs_table_->GetValueByKey(var_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetUsesVarsByProc(const int proc_idx) const {
  try {
    return uses_proc_to_variables_table_->GetValueByKey(proc_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<pair<int, int>> Pkb::GetAllUsesProcVarPairs() const {
  try {
    return UnfoldResults<UsesProcToVariablesTable*>(uses_proc_to_variables_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::IsModifiesStmt(const int stmt, const int var_idx) const {
  try {
    vector<int> value = modifies_stmt_to_variables_table_->GetValueByKey(stmt);
    return find(value.begin(), value.end(), var_idx) != value.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsModifiesStmtExists() const {
  return modifies_stmt_to_variables_table_->GetTableSize() > 0;
}

vector<int> Pkb::GetModifiesStmtsByVar(const int var_idx) const {
  try {
    return modifies_variable_to_stmts_table_->GetValueByKey(var_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetModifiesVarByStmt(const int stmt) const {
  try {
    return modifies_stmt_to_variables_table_->GetValueByKey(stmt);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<pair<int, int>> Pkb::GetAllModifiesStmtVarPairs() const {
  try {
    return UnfoldResults<ModifiesStmtToVariablesTable*>(modifies_stmt_to_variables_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

bool Pkb::IsProcModifiesVar(const int proc_idx, const int var_idx) const {
  try {
    vector<int> value = modifies_proc_to_variables_table_->GetValueByKey(proc_idx);
    return find(value.begin(), value.end(), var_idx) != value.end();
  } catch (exception& e) {
    return false;
  }
}

vector<int> Pkb::GetModifiesProcsByVar(const int var_idx) const {
  try {
    return modifies_variable_to_procs_table_->GetValueByKey(var_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<int> Pkb::GetModifiesVarsByProc(const int proc_idx) const {
  try {
    return modifies_proc_to_variables_table_->GetValueByKey(proc_idx);
  } catch (exception& e) {
    return vector<int>{};
  }
}

vector<pair<int, int>> Pkb::GetAllModifiesProcVarPairs() const {
  try {
    return UnfoldResults<ModifiesProcToVariablesTable*>(modifies_proc_to_variables_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

unordered_set<int> Pkb::GetAllStmtsWithPattern(const string& pattern) const {
  const string usable_pattern = PatternHelper::PreprocessPattern(pattern);
  constexpr bool is_full = false;
  unordered_set<int> empty_set{};
  const unordered_set<string> res = PatternHelper::GetPatternSetPostfix(usable_pattern, is_full);
  if (res.size() != 1) throw BadResultException();
  const string s = *(res.begin());

  if (!pattern_to_stmts_table_->KeyExistsInTable(s)) return empty_set;
  return pattern_to_stmts_table_->GetValueByKey(s);
}

unordered_set<int> Pkb::GetStmtsWithExactPattern(const string& pattern) const {
  const string usable_pattern = PatternHelper::PreprocessPattern(pattern);
  constexpr bool is_full = false;
  unordered_set<int> empty_set{};
  const unordered_set<string> res = PatternHelper::GetPatternSetPostfix(usable_pattern, is_full);
  if (res.size() != 1) throw BadResultException();
  const string s = *(res.begin());

  if (!exact_pattern_to_stmt_table_->KeyExistsInTable(s)) return empty_set;
  return exact_pattern_to_stmt_table_->GetValueByKey(s);
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

bool Pkb::AddCalls(const string& key, const vector<string>& value) {
  bool add_success = true;
  if (!proc_index_table_->KeyExistsInTable(key)) {
    add_success = AddEntityToSet(EntityIdentifier::kProc, key);
  }
  for (const string proc: value) {
    if (!proc_index_table_->KeyExistsInTable(proc)) {
      add_success = add_success && AddEntityToSet(EntityIdentifier::kProc, proc);
    }
  }
  int key_idx = GetIndexByProc(key);
  vector<int> value_idx_lst;
  for (const string proc: value) {
    value_idx_lst.push_back(GetIndexByProc(proc));
  }
  add_success = add_success && calls_table_->AddKeyValuePair(key_idx, value_idx_lst);
  // Populate the reverse relation
  for (const string called : value) {
    const int called_idx = GetIndexByProc(called);
    try {
      // Get the callers and then add in the new caller
      vector<int> callers = called_by_table_->GetValueByKey(called_idx);
      callers.push_back(key_idx);
      called_by_table_->UpdateKeyWithNewValue(called_idx, callers);
    } catch (InvalidKeyException& e) {
      add_success = called_by_table_->AddKeyValuePair(called_idx, vector<int>{key_idx});
    }
  }

  return add_success;
}

bool Pkb::AddNext(const int key, const int value) {
  bool add_success = next_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  add_success = before_table_->AddKeyValuePair(value, key) && add_success;
  return add_success;
}

bool Pkb::AddModifies(const int key, const vector<string>& value) {
  vector<int> value_idx;
  for (auto var_name: value) {
    value_idx.push_back(GetIndexByVar(var_name));
  }
  bool add_success = modifies_stmt_to_variables_table_->AddKeyValuePair(key, value_idx);
  // Populate the reverse relation
  add_success = add_success && modifies_variable_to_stmts_table_->UpdateKeyValuePair(key, value_idx);
  return add_success;
}

bool Pkb::AddModifiesP(const string& key, const vector<string>& value) {
  bool add_success = true;
  if (!proc_index_table_->KeyExistsInTable(key)) {
    add_success = AddEntityToSet(EntityIdentifier::kProc, key);
  }
  int key_idx = GetIndexByProc(key);

  vector<int> value_idx;
  for (const string var_name: value) {
    value_idx.push_back(GetIndexByVar(var_name));
  }

  add_success = add_success && modifies_proc_to_variables_table_->AddKeyValuePair(key_idx, value_idx);
  // Populate the reverse relation
  add_success = add_success && modifies_variable_to_procs_table_->UpdateKeyValuePair(key_idx, value_idx);
  return add_success;
}

bool Pkb::AddUses(const int key, const vector<string>& value) {
  vector<int> value_idx;
  for (auto var_name : value) {
    value_idx.push_back(GetIndexByVar(var_name));
  }
  bool add_success = uses_stmt_to_variables_table_->AddKeyValuePair(key, value_idx);
  // Populate the reverse relation
  add_success = add_success && uses_variable_to_stmts_table_->UpdateKeyValuePair(key, value_idx);
  return add_success;
}

bool Pkb::AddUsesP(const string& key, const vector<string>& value) {
  bool add_success = true;
  if (!proc_index_table_->KeyExistsInTable(key)) {
    add_success = AddEntityToSet(EntityIdentifier::kProc, key);
  }
  int key_idx = GetIndexByProc(key);

  vector<int> value_idx;
  for (const string var_name: value) {
    value_idx.push_back(GetIndexByVar(var_name));
  }

  add_success = add_success && uses_proc_to_variables_table_->AddKeyValuePair(key_idx, value_idx);
  // Populate the reverse relation
  add_success = add_success && uses_variable_to_procs_table_->UpdateKeyValuePair(key_idx, value_idx);
  return add_success;
}

bool Pkb::AddPattern(bool& add_success, unordered_set<string> pattern_set, Table<string, unordered_set<int>>* table_to_update, const int line_num) {
  for (auto p : pattern_set) {
    if (!table_to_update->KeyExistsInTable(p)) {
      add_success = add_success && table_to_update->AddKeyValuePair(p, unordered_set<int>{line_num});
    } else {
      unordered_set<int> current_set = table_to_update->GetValueByKey(p);
      current_set.insert(line_num);
      add_success = add_success && table_to_update->UpdateKeyWithNewValue(p, current_set);
    }
  }
  return add_success;
}

bool Pkb::UpdateIndexTable(Table<int, string>* index_to_string_table, Table<string, int>* string_to_int_table, const string& entity_value) {
  try {
    vector<string> curr_string_lst = string_to_int_table->GetKeyLst();
    if (find(curr_string_lst.begin(), curr_string_lst.end(), entity_value) != curr_string_lst.end()) return true;
    bool success = string_to_int_table->AddKeyValuePair(entity_value, curr_string_lst.size());
    success = success && index_to_string_table->AddKeyValuePair(curr_string_lst.size(), entity_value);
    return success;
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddPattern(const int line_num, const string& input) {
  // First the SP side should guarantee a valid input is sent
  // We then proceed to parse the set of valid substring patterns
  const string clean_input = PatternHelper::PreprocessPattern(input);

  const unordered_set<string> valid_sub_patterns = PatternHelper::GetPatternSetPostfix(clean_input, true);
  const unordered_set<string> valid_exact_patterns = PatternHelper::GetPatternSetPostfix(clean_input, false);
  bool add_success = stmt_to_patterns_table_->AddKeyValuePair(line_num, valid_sub_patterns);
  add_success = AddPattern(add_success, valid_sub_patterns, pattern_to_stmts_table_, line_num);
  add_success = AddPattern(add_success, valid_exact_patterns, exact_pattern_to_stmt_table_, line_num);
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
      case TableIdentifier::kNext: return AddNext(key, value);
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
      case TableIdentifier::kCaller: return caller_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kPrint: return print_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kPattern: return AddPattern(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const string& key, const vector<string>& value) {
  try {
    if (value.empty()) throw EmptyValueException();
    switch (table_identifier) {
      case TableIdentifier::kCalls: return AddCalls(key, value);
      case TableIdentifier::KModifiesProcToVar: return AddModifiesP(key, value);
      case TableIdentifier::kUsesProcToVar: return AddUsesP(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const string& key, const pair<int, int>& value) {
  try {
    if (!value.first || !value.second) {
      throw EmptyValueException();
    }
    switch (table_identifier) {
      case TableIdentifier::kProcedure: return proc_range_table_->AddKeyValuePair(key, value);
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
      case EntityIdentifier::kIf: {
        if_set_.insert(entity_val);
        return true;
      }
      case EntityIdentifier::kWhile: {
        while_set_.insert(entity_val);
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
        UpdateIndexTable(index_var_table_, var_index_table_, entity_val);
        variable_set_.insert(GetIndexByVar(entity_val));
        return true;
      }
      case EntityIdentifier::kProc: {
        UpdateIndexTable(index_proc_table_, proc_index_table_, entity_val);
        procedure_set_.insert(GetIndexByProc(entity_val));
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

unordered_set<int> Pkb::GetAllEntity(const EntityIdentifier entity_identifier) {
  switch (entity_identifier) {
    case EntityIdentifier::kStmt: return stmt_set_;
    case EntityIdentifier::kAssign: return assign_set_;
    case EntityIdentifier::kRead: return read_set_;
    case EntityIdentifier::kPrint: return print_set_;
    case EntityIdentifier::kCall: return call_set_;
    case EntityIdentifier::kConstant: return constant_set_;
    case EntityIdentifier::kIf: return if_set_;
    case EntityIdentifier::kWhile: return while_set_;
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

bool Pkb::IsVar(const int var_idx) const {
  return variable_set_.find(var_idx) != variable_set_.end();
}

bool Pkb::IsRead(const int stmt_no) const {
  // O(1) find for unordered_set
  return read_set_.find(stmt_no) != read_set_.end();
}

int Pkb::GetVarFromRead(const int stmt_no) const {
  if (!IsRead(stmt_no)) return -1;
  return GetIndexByVar(read_table_->GetValueByKey(stmt_no));
}

vector<int> Pkb::GetReadByVar(const int var_idx) const {
  vector<int> res = {};
  if (!IsVar(var_idx)) return res;
  for (const auto& [key, val] : read_table_->GetKeyValueLst()) {
    if (val == GetVarByIndex(var_idx)) res.push_back(key);
  }
  return res;
}

bool Pkb::IsPrint(const int stmt_no) const {
  return print_set_.find(stmt_no) != print_set_.end();
}

int Pkb::GetVarFromPrint(const int stmt_no) const {
  if (!IsPrint(stmt_no)) return -1;
  return GetIndexByVar(print_table_->GetValueByKey(stmt_no));
}

vector<int> Pkb::GetPrintByVar(const int var_idx) const {
  vector<int> res = {};
  if (!IsVar(var_idx)) return res;
  for (const auto& [key, val] : print_table_->GetKeyValueLst()) {
    if (val == GetVarByIndex(var_idx)) res.push_back(key);
  }
  return res;
}

bool Pkb::IsAssign(const int stmt_no) const {
  return assign_set_.find(stmt_no) != assign_set_.end();
}

int Pkb::GetVarFromAssign(const int stmt_no) const {
  if (!IsAssign(stmt_no)) return -1;
  return GetIndexByVar(assign_table_->GetValueByKey(stmt_no));
}

vector<int> Pkb::GetAssignByVar(const int var_idx) const {
  vector<int> res = {};
  if (!IsVar(var_idx)) return res;
  for (const auto& [key, val] : assign_table_->GetKeyValueLst()) {
    if (val == GetVarByIndex(var_idx)) res.push_back(key);
  }
  return res;
}

bool Pkb::IsCall(const int stmt_no) const {
  return call_set_.find(stmt_no) != call_set_.end();
}

int Pkb::GetProcFromCall(const int stmt_no) const {
  if (!IsCall(stmt_no)) return -1;
  return GetIndexByProc(caller_table_->GetValueByKey(stmt_no));
}

vector<int> Pkb::GetCallFromProc(const int proc_idx) const {
  vector<int> res = {};
  if (!IsProcedure(proc_idx)) return res;
  for (const auto& [key, val] : caller_table_->GetKeyValueLst()) {
    if (val == GetProcByIndex(proc_idx)) res.push_back(key);
  }
  return res;
}

bool Pkb::IsIf(const int stmt_no) const {
  return if_set_.find(stmt_no) != if_set_.end();
}

bool Pkb::IsWhile(const int stmt_no) const {
  return while_set_.find(stmt_no) != while_set_.end();
}

bool Pkb::IsStmt(const int stmt_no) const {
  return stmt_set_.find(stmt_no) != stmt_set_.end();
}

bool Pkb::IsConstant(const int stmt_no) const {
  return constant_set_.find(stmt_no) != constant_set_.end();
}

bool Pkb::IsProcedure(const int proc_idx) const {
  return procedure_set_.find(proc_idx) != procedure_set_.end();
}

vector<pair<int, string>> Pkb::GetAllIndexVarPairs() const {
  return index_var_table_->GetKeyValueLst();
}
vector<pair<string, int>> Pkb::GetAllVarIndexPairs() const {
  return var_index_table_->GetKeyValueLst();
}

string Pkb::GetVarByIndex(const int idx) const {
  try {
    return index_var_table_->GetValueByKey(idx);
  }
  catch (exception& e) {
    return "";
  }
}

int Pkb::GetIndexByVar(const string& var_name) const {
  try {
    return var_index_table_->GetValueByKey(var_name);
  } catch (exception& e) {
    return -1;
  }
}

vector<pair<int, string>> Pkb::GetAllIndexProcPairs() const {
  return index_proc_table_->GetKeyValueLst();
}
vector<pair<string, int>> Pkb::GetAllProcIndexPairs() const {
  return proc_index_table_->GetKeyValueLst();
}

string Pkb::GetProcByIndex(const int idx) const {
  try {
    return index_proc_table_->GetValueByKey(idx);
  } catch (exception& e) {
    return "";
  }
}

int Pkb::GetIndexByProc(const string& proc_name) const {
  try {
    return proc_index_table_->GetValueByKey(proc_name);
  } catch (exception& e) {
    return -1;
  }
}
