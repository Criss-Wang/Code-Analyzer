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
bool Pkb::IsCalls(const string& proc_1, const string& proc_2) const {
  try {
    vector<string> callees = calls_table_->GetValueByKey(proc_1);
    return find(callees.begin(), callees.end(), proc_2) != callees.end();
  } catch (exception& e) {
    return false;
  }
}

vector<string> Pkb::GetCallers(const string& proc) const {
  try {
    return called_by_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<string>{};
  }
}

vector<string> Pkb::GetCallees(const string& proc) const {
  try {
    return calls_table_->GetValueByKey(proc);
  } catch (exception& e) {
    return vector<string>{};
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

vector<pair<string, string>> Pkb::GetAllCallsPairs() const {
  try {
    return UnfoldResults<CallsTable*, string, string>(calls_table_);
  } catch (exception& e) {
    return vector<pair<string, string>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllParentPairs() const {
  try {
    return UnfoldResults<ParentTable*, int, int>(parent_table_);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

vector<pair<int, int>> Pkb::GetAllTransitiveParentPairs() const {
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
    return UnfoldResults<FollowsStarTable*, int, int>(follows_star_table_);
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
    return UnfoldResults<UsesStmtToVariablesTable*, int, int>(uses_stmt_to_variables_table_);
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
    return UnfoldResults<ModifiesStmtToVariablesTable*, int, int>(modifies_stmt_to_variables_table_);
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
  bool add_success = calls_table_->AddKeyValuePair(key, value);
  // Populate the reverse relation
  for (const string called : value) {
    try {
      // Get the callers and then add in the new caller
      vector<string> callers = called_by_table_->GetValueByKey(called);
      callers.push_back(key);
      called_by_table_->UpdateKeyWithNewValue(called, callers);
    } catch (InvalidKeyException& e) {
      add_success = called_by_table_->AddKeyValuePair(called, vector<string>{key});
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
        variable_set_.insert(entity_val);
        UpdateIndexTable(index_var_table_, var_index_table_, entity_val);
        return true;
      }
      case EntityIdentifier::kProc: {
        procedure_set_.insert(entity_val);
        UpdateIndexTable(index_proc_table_, proc_index_table_, entity_val);
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
    case EntityIdentifier::kIf: return if_set_;
    case EntityIdentifier::kWhile: return while_set_;
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

bool Pkb::IsVar(const string& var_string) const {
  return variable_set_.find(var_string) != variable_set_.end();
}

bool Pkb::IsRead(const int stmt_no) const {
  // O(1) find for unordered_set
  return read_set_.find(stmt_no) != read_set_.end();
}

string Pkb::GetVarFromRead(const int stmt_no) const {
  if (!IsRead(stmt_no)) return "";
  return read_table_->GetValueByKey(stmt_no);
  
}

vector<int> Pkb::GetReadByVar(const string& var_string) const {
  vector<int> res = {};
  if (!IsVar(var_string)) return res;
  for (const auto& [key, val] : read_table_->GetKeyValueLst()) {
    if (val == var_string) res.push_back(key);
  }

  return res;
}

bool Pkb::IsPrint(const int stmt_no) const {
  return print_set_.find(stmt_no) != print_set_.end();
}

bool Pkb::IsCall(const int stmt_no) const {
  return call_set_.find(stmt_no) != call_set_.end();
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

bool Pkb::IsProcedure(const string& proc_name) const {
  return procedure_set_.find(proc_name) != procedure_set_.end();
}

bool Pkb::IsAssign(int stmt_no) const {
  return assign_set_.find(stmt_no) != assign_set_.end();
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
