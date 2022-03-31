#include "pkb.h"

#include "PQL/query_parser/token.h"
#include "Utility/helper.h"
#include "Utility/CFG/control_flow_graph.h"

#define INVALID_INDEX -1
#define EMPTY_STRING ""
#define EMPTY_INT_LIST vector<int>{}
#define EMPTY_STRING_LIST vector<string>{}

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

shared_ptr<RelTable> Pkb::GetFollowsTable() {
  return follows_table_;
}

shared_ptr<RelListTable> Pkb::GetFollowsStarTable() {
  return follows_star_table_;
}

shared_ptr<RelTable> Pkb::GetFollowsBeforeTable() {
  return follows_before_table_;
}

shared_ptr<RelListTable> Pkb::GetFollowsBeforeStarTable() {
  return follows_before_star_table_;
}

shared_ptr<RelListTable> Pkb::GetParentTable() {
  return parent_table_;
}

shared_ptr<RelListTable> Pkb::GetChildTable() {
  return child_table_;
}

shared_ptr<RelListTable> Pkb::GetParentStarTable() {
  return parent_star_table_;
}

shared_ptr<RelListTable> Pkb::GetChildStarTable() {
  return child_star_table_;
}

shared_ptr<RelListTable> Pkb::GetCallsTable() {
  return calls_table_;
}

shared_ptr<RelListTable> Pkb::GetCallsStarTable() {
  return calls_star_table_;
}

shared_ptr<RelListTable> Pkb::GetCalledByTable() {
  return called_by_table_;
}

shared_ptr<RelListTable> Pkb::GetCalledByStarTable() {
  return called_by_star_table_;
}

shared_ptr<RelListTable> Pkb::GetNextTable() {
  return next_table_;
}

shared_ptr<RelListTable> Pkb::GetBeforeTable() {
  return before_table_;
}

shared_ptr<RelListTable> Pkb::GetModifiesStmtToVariablesTable() {
  return modifies_stmt_to_variables_table_;
}

shared_ptr<RelListReverseTable> Pkb::GetModifiesVariableToStmtsTable() {
  return modifies_variable_to_stmts_table_;
}

shared_ptr<RelListTable> Pkb::GetModifiesProcToVariablesTable() {
  return modifies_proc_to_variables_table_;
}

shared_ptr<RelListReverseTable> Pkb::GetModifiesVariableToProcsTable() {
  return modifies_variable_to_procs_table_;
}

shared_ptr<RelListTable> Pkb::GetUsesStmtToVariablesTable() {
  return uses_stmt_to_variables_table_;
}

shared_ptr<RelListReverseTable> Pkb::GetUsesVariableToStmtsTable() {
  return uses_variable_to_stmts_table_;
}

shared_ptr<RelListTable> Pkb::GetUsesProcToVariablesTable() {
  return uses_proc_to_variables_table_;
}

shared_ptr<RelListReverseTable> Pkb::GetUsesVariableToProcsTable() {
  return uses_variable_to_procs_table_;
}

shared_ptr<EntityVarsTable> Pkb::GetCallerTable() {
  return caller_table_;
}
shared_ptr<EntityVarsTable> Pkb::GetReadTable() {
  return read_table_;
}
shared_ptr<EntityVarsTable> Pkb::GetPrintTable() {
  return print_table_;
}

unordered_set<int> Pkb::GetStmtSet() {
  return stmt_set_;
}

unordered_set<int> Pkb::GetAssignSet() {
  return assign_set_;
}

unordered_set<int> Pkb::GetReadSet() {
  return read_set_;
}

unordered_set<int> Pkb::GetPrintSet() {
  return print_set_;
}

unordered_set<int> Pkb::GetCallSet() {
  return call_set_;
}

unordered_set<int> Pkb::GetIfSet() {
  return if_set_;
}

unordered_set<int> Pkb::GetWhileSet() {
  return while_set_;
}

unordered_set<int> Pkb::GetVarSet() {
  return variable_set_;
}

unordered_set<int> Pkb::GetProcSet() {
  return procedure_set_;
}

unordered_set<int> Pkb::GetConstantSet() {
  return constant_set_;
}

unordered_map<int, vector<int>> Pkb::GetNextInternalMap() {
  return this->next_table_->GetInternalMap();
}

typedef shared_ptr<RelListTable>(Pkb::* GetTableFn)();

const unordered_map<pql::RelationshipTypes, GetTableFn> list_table_map_ = {
  {pql::RelationshipTypes::kCalls, &Pkb::GetCallsTable},
  {pql::RelationshipTypes::kCallsT, &Pkb::GetCallsStarTable},
  {pql::RelationshipTypes::kParent, &Pkb::GetParentTable},
  {pql::RelationshipTypes::kParentT, &Pkb::GetParentStarTable},
  {pql::RelationshipTypes::kFollowsT, &Pkb::GetFollowsStarTable},
  {pql::RelationshipTypes::kUsesS, &Pkb::GetUsesStmtToVariablesTable},
  {pql::RelationshipTypes::kUsesP, &Pkb::GetUsesProcToVariablesTable},
  {pql::RelationshipTypes::kModifiesS, &Pkb::GetModifiesStmtToVariablesTable},
  {pql::RelationshipTypes::kModifiesP, &Pkb::GetModifiesProcToVariablesTable},
  {pql::RelationshipTypes::kNext, &Pkb::GetNextTable}
};

const unordered_map<pql::RelationshipTypes, GetTableFn> reverse_table_map_ = {
  {pql::RelationshipTypes::kCalls, &Pkb::GetCalledByTable},
  {pql::RelationshipTypes::kCallsT, &Pkb::GetCalledByStarTable},
  {pql::RelationshipTypes::kParent, &Pkb::GetChildTable},
  {pql::RelationshipTypes::kParentT, &Pkb::GetChildStarTable},
  {pql::RelationshipTypes::kFollowsT, &Pkb::GetFollowsBeforeStarTable},
  {pql::RelationshipTypes::kNext, &Pkb::GetBeforeTable}
};

typedef shared_ptr<RelListReverseTable>(Pkb::* GetInverseTableFn)();
const unordered_map<pql::RelationshipTypes, GetInverseTableFn> mod_use_reverse_table_map_ = {
  {pql::RelationshipTypes::kUsesP, &Pkb::GetUsesVariableToProcsTable},
  {pql::RelationshipTypes::kUsesS, &Pkb::GetUsesVariableToStmtsTable},
  {pql::RelationshipTypes::kModifiesP, &Pkb::GetModifiesVariableToProcsTable},
  {pql::RelationshipTypes::kModifiesS, &Pkb::GetModifiesVariableToStmtsTable}
};

const unordered_map<pql::RelationshipTypes, TableType> type_map_ = {
  {pql::RelationshipTypes::kCalls, TableType::kRelList},
  {pql::RelationshipTypes::kCallsT, TableType::kRelList},
  {pql::RelationshipTypes::kParent, TableType::kRelList},
  {pql::RelationshipTypes::kParentT, TableType::kRelList},
  {pql::RelationshipTypes::kFollows, TableType::kRelSimple},
  {pql::RelationshipTypes::kFollowsT, TableType::kRelList},
  {pql::RelationshipTypes::kUsesS, TableType::kRelListOrReverse},
  {pql::RelationshipTypes::kUsesP, TableType::kRelListOrReverse},
  {pql::RelationshipTypes::kModifiesS, TableType::kRelListOrReverse},
  {pql::RelationshipTypes::kModifiesP, TableType::kRelListOrReverse},
  {pql::RelationshipTypes::kNext, TableType::kRelList}
};

// Relationships APIs
bool Pkb::IsRelationshipHolds(const pql::RelationshipTypes rel_types, const int key, const int value) {
  try {
    if (type_map_.at(rel_types) == TableType::kRelSimple) {
      const shared_ptr<RelTable> table = GetFollowsTable();
      return value == table->GetValueByKey(key);
    }
    const GetTableFn table_getter = list_table_map_.at(rel_types);
    const shared_ptr<RelListTable> table = (this->*table_getter)();
    vector<int> value_list = table->GetValueByKey(key);
    return find(value_list.begin(), value_list.end(), value) != value_list.end();
  } catch (exception& e) {
    return false;
  }
}

bool Pkb::IsRelationshipExists(const pql::RelationshipTypes rel_types) {
  if (type_map_.at(rel_types) == TableType::kRelSimple) {
    const shared_ptr<RelTable> table = GetFollowsTable();
    return table->GetTableSize() > 0;
  }
  const GetTableFn table_getter = list_table_map_.at(rel_types);
  const shared_ptr<RelListTable> table = (this->*table_getter)();
  return table->GetTableSize() > 0;
}

vector<int> Pkb::GetRelFirstArgument(const pql::RelationshipTypes rel_types, const int second_arg_idx) {
  try {
    if (type_map_.at(rel_types) == TableType::kRelSimple) {
      const shared_ptr<RelTable> table = GetFollowsBeforeTable();
      return { table->GetValueByKey(second_arg_idx) };
    }
    if (type_map_.at(rel_types) == TableType::kRelList) {
      const GetTableFn table_getter = reverse_table_map_.at(rel_types);
      const shared_ptr<RelListTable> table = (this->*table_getter)();
      auto res = table->GetValueByKey(second_arg_idx);
      if (rel_types == pql::RelationshipTypes::kParent) {
        return {res[0]};
      }
      return res;
    }
    const GetInverseTableFn table_getter = mod_use_reverse_table_map_.at(rel_types);
    const shared_ptr<RelListReverseTable> table = (this->*table_getter)();
    return table->GetValueByKey(second_arg_idx);
  } catch (exception& e) {
    return EMPTY_INT_LIST;
  }
}

vector<int> Pkb::GetRelSecondArgument(const pql::RelationshipTypes rel_types, const int first_arg_idx) {
  try {
    if (type_map_.at(rel_types) == TableType::kRelSimple) {
      const shared_ptr<RelTable> table = GetFollowsTable();
      return { table->GetValueByKey(first_arg_idx) };
    }
    const GetTableFn table_getter = list_table_map_.at(rel_types);
    const shared_ptr<RelListTable> table = (this->*table_getter)();
    return  table->GetValueByKey(first_arg_idx);
  } catch (exception& e) {
    return EMPTY_INT_LIST;
  }
}

vector<pair<int, int>> Pkb::GetRelArgumentPairs(const pql::RelationshipTypes rel_types) {
  try {
    if (type_map_.at(rel_types) == TableType::kRelSimple) {
      vector<pair<int, int>> result;
      for (const auto& [key, val] : GetFollowsTable()->GetKeyValueLst()) {
        result.emplace_back(make_pair(key, val));
      }
      return result;
    }
    const GetTableFn table_getter = list_table_map_.at(rel_types);
    const shared_ptr<RelListTable> table = (this->*table_getter)();
    return UnfoldResults<shared_ptr<RelListTable>>(table);
  } catch (exception& e) {
    return vector<pair<int, int>>{};
  }
}

// Pattern Searches APIs
unordered_set<int> Pkb::GetAllStmtsWithPattern(const string& pattern, const bool is_exact) const {
  const string usable_pattern = PatternHelper::PreprocessPattern(pattern);
  constexpr bool is_full = false;
  unordered_set<int> empty_set{};
  const unordered_set<string> res = PatternHelper::GetPatternSetPostfix(usable_pattern, is_full);
  if (res.size() != 1) {
    throw BadResultException();
  }
  const string s = *(res.begin());
  shared_ptr<Table<string, unordered_set<int>>> table;
  if (!is_exact) {
    table = assign_pattern_to_stmts_table_;
  } else {
    table = exact_pattern_to_stmt_table_;
  }
  if (!table->KeyExistsInTable(s)) {
    return empty_set;
  }
  return table->GetValueByKey(s);
}

unordered_set<string> Pkb::GetAllPatternVariablesInStmt(const int stmt_no,
  const TableIdentifier table_identifier) const {
  unordered_set<string> empty_set{};
  shared_ptr<Table<int, unordered_set<string>>> search_table;
  if (table_identifier == TableIdentifier::kIfPattern) {
    search_table = if_stmt_to_pattern_table_;
  } else if (table_identifier == TableIdentifier::kWhilePattern) {
    search_table = while_stmt_to_pattern_table_;
  } else {
    throw InvalidIdentifierException();
  }
  if (!search_table->KeyExistsInTable(stmt_no)) {
    return empty_set;
  }
  return search_table->GetValueByKey(stmt_no);
}

unordered_set<int> Pkb::GetAllStmtsWithPatternVariable(const int pattern_var_idx,
  const TableIdentifier table_identifier) const {
  unordered_set<int> empty_set{};
  shared_ptr<Table<string, unordered_set<int>>> search_table;
  if (table_identifier == TableIdentifier::kIfPattern) {
    search_table = if_pattern_to_stmt_table_;
  } else if(table_identifier == TableIdentifier::kWhilePattern) {
    search_table = while_pattern_to_stmt_table_;
  } else {
    throw InvalidIdentifierException();
  }
  if (!search_table->KeyExistsInTable(GetStringByIndex(IndexTableType::kVar, pattern_var_idx))) {
    return empty_set;
  }
  return search_table->GetValueByKey(GetStringByIndex(IndexTableType::kVar, pattern_var_idx));
}

vector<pair<int, int>> Pkb::GetContainerStmtVarPair(const TableIdentifier table_identifier) const {
  vector<pair<int, int>> result;
  shared_ptr<Table<int, unordered_set<string>>> search_table;
  if (table_identifier == TableIdentifier::kIfPattern) {
    search_table = if_stmt_to_pattern_table_;
  } else if (table_identifier == TableIdentifier::kWhilePattern) {
    search_table = while_stmt_to_pattern_table_;
  } else {
    throw InvalidIdentifierException();
  }
  for (const auto& [key, val_lst] : search_table->GetKeyValueLst()) {
    for (const auto& val_item : val_lst) {
      result.emplace_back(make_pair(key, GetIndexByString(IndexTableType::kVarIndex, val_item)));
    }
  }
  return result;
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
  int key_idx = GetIndexByString(IndexTableType::kProcIndex, key);
  vector<int> value_idx_lst;
  for (const string proc: value) {
    value_idx_lst.push_back(GetIndexByString(IndexTableType::kProcIndex, proc));
  }
  add_success = add_success && calls_table_->AddKeyValuePair(key_idx, value_idx_lst);
  // Populate the reverse relation
  for (const string called : value) {
    const int called_idx = GetIndexByString(IndexTableType::kProcIndex, called);
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

bool Pkb::AddNext(const int key, const vector<int>& value) {
  bool add_success = next_table_->AddKeyValuePair(key, value);
  // Populate reverse relation
  for (const int v : value) {
    if (before_table_->KeyExistsInTable(v)) {
      vector<int> value_to_update = before_table_->GetValueByKey(v);
      value_to_update.push_back(key);
      add_success = add_success && before_table_->UpdateKeyWithNewValue(v, value_to_update);
    } else {
      add_success = add_success && before_table_->AddKeyValuePair(v, vector<int>{key});
    }
  }

  return add_success;
}

bool Pkb::AddModifies(const int key, const vector<string>& value) {
  vector<int> value_idx;
  for (auto var_name: value) {
    value_idx.push_back(GetIndexByString(IndexTableType::kVarIndex, var_name));
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
  int key_idx = GetIndexByString(IndexTableType::kProcIndex, key);

  vector<int> value_idx;
  for (const string var_name: value) {
    value_idx.push_back(GetIndexByString(IndexTableType::kVarIndex, var_name));
  }

  add_success = add_success && modifies_proc_to_variables_table_->AddKeyValuePair(key_idx, value_idx);
  // Populate the reverse relation
  add_success = add_success && modifies_variable_to_procs_table_->UpdateKeyValuePair(key_idx, value_idx);
  return add_success;
}

bool Pkb::AddUses(const int key, const vector<string>& value) {
  vector<int> value_idx;
  for (auto var_name : value) {
    value_idx.push_back(GetIndexByString(IndexTableType::kVarIndex, var_name));
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
  int key_idx = GetIndexByString(IndexTableType::kProcIndex, key);

  vector<int> value_idx;
  for (const string var_name: value) {
    value_idx.push_back(GetIndexByString(IndexTableType::kVarIndex, var_name));
  }

  add_success = add_success && uses_proc_to_variables_table_->AddKeyValuePair(key_idx, value_idx);
  // Populate the reverse relation
  add_success = add_success && uses_variable_to_procs_table_->UpdateKeyValuePair(key_idx, value_idx);
  return add_success;
}

bool Pkb::AddPatternToTable(bool& add_success, const unordered_set<string> pattern_set, shared_ptr<Table<string, unordered_set<int>>> table_to_update, const int line_num) {
  for (auto& p : pattern_set) {
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

bool Pkb::UpdateIndexTable(shared_ptr<Table<int, string>> index_to_string_table, shared_ptr<Table<string, int>> string_to_int_table, const string& entity_value) {
  try {
    vector<string> curr_string_lst = string_to_int_table->GetKeyLst();
    if (find(curr_string_lst.begin(), curr_string_lst.end(), entity_value) != curr_string_lst.end()) return true;
    bool success = string_to_int_table->AddKeyValuePair(entity_value, curr_string_lst.size());
    success = success && index_to_string_table->AddKeyValuePair(curr_string_lst.size(), entity_value);
    return success;
  } catch (exception& e) {
    throw UpdateIndexTableException();
  }
}

bool Pkb::AddPattern(const int line_num, const vector<string>& input_set, const string& input, const TableIdentifier table_identifier) {
  // First the SP side should guarantee a valid input is sent (valid variable names given)
  // We then proceed to parse the set of valid substring patterns
  try {
    bool add_success;

    if (table_identifier == TableIdentifier::kAssignPattern) {
      const string clean_input = PatternHelper::PreprocessPattern(input);
      const unordered_set<string> valid_sub_patterns = PatternHelper::GetPatternSetPostfix(clean_input, true);
      const unordered_set<string> valid_exact_patterns = PatternHelper::GetPatternSetPostfix(clean_input, false);
      add_success = assign_stmt_to_patterns_table_->AddKeyValuePair(line_num, valid_sub_patterns);
      add_success = AddPatternToTable(add_success, valid_sub_patterns, assign_pattern_to_stmts_table_, line_num);
      add_success = AddPatternToTable(add_success, valid_exact_patterns, exact_pattern_to_stmt_table_, line_num);
    } else if (table_identifier == TableIdentifier::kIfPattern) {
      const unordered_set<string> valid_var_names = unordered_set<string>(input_set.begin(), input_set.end());
      add_success = if_stmt_to_pattern_table_->AddKeyValuePair(line_num, valid_var_names);
      add_success = AddPatternToTable(add_success, valid_var_names, if_pattern_to_stmt_table_, line_num);
    } else if (table_identifier == TableIdentifier::kWhilePattern) {
      const unordered_set<string> valid_var_names = unordered_set<string>(input_set.begin(), input_set.end());
      add_success = while_stmt_to_pattern_table_->AddKeyValuePair(line_num, valid_var_names);
      add_success = AddPatternToTable(add_success, valid_var_names, while_pattern_to_stmt_table_, line_num);
    } else {
      throw InvalidIdentifierException();
    }
    return add_success;
  } catch (exception& e) {
    throw e;
  }
}

// Table Insertion APIs
bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
  try {
    if (value.empty()) {
      throw EmptyValueException();
    }

    switch (table_identifier) {
      case TableIdentifier::kConstant:
        return constant_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kParent:
        return AddParent(key, value);
      case TableIdentifier::kNext:
        return AddNext(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<string>& value) {
  try {
    if (value.empty()) {
      throw EmptyValueException();
    }

    switch (table_identifier) {
      case TableIdentifier::kIf:
        return if_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kWhile:
        return while_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kModifiesStmtToVar:
        return AddModifies(key, value);
      case TableIdentifier::kUsesStmtToVar:
        return AddUses(key, value);
      case TableIdentifier::kIfPattern:
        return AddPattern(key, value, EMPTY_STRING, table_identifier);
      case TableIdentifier::kWhilePattern:
        return AddPattern(key, value, EMPTY_STRING, table_identifier);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const int value) {
  try {
    if (value < 1) {
      throw EmptyValueException();
    }

    switch (table_identifier) {
      case TableIdentifier::kFollows:
        return AddFollows(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const int key, const string& value) {
  try {
    if (value.empty()) {
      throw EmptyValueException();
    }

    switch (table_identifier) {
      case TableIdentifier::kAssign:
        return assign_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kRead:
        return read_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kCaller:
        return caller_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kPrint:
        return print_table_->AddKeyValuePair(key, value);
      case TableIdentifier::kAssignPattern:
        return AddPattern(key, EMPTY_STRING_LIST, value, table_identifier);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const string& key, const vector<string>& value) {
  try {
    if (value.empty()) {
      throw EmptyValueException();
    }

    switch (table_identifier) {
      case TableIdentifier::kCalls:
        return AddCalls(key, value);
      case TableIdentifier::kModifiesProcToVar:
        return AddModifiesP(key, value);
      case TableIdentifier::kUsesProcToVar:
        return AddUsesP(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
  }
}

bool Pkb::AddInfoToTable(const TableIdentifier table_identifier, const string& key, const pair<int, int>& value) {
  try {
    if (!value.first || !value.second) {
      throw EmptyValueException();
    }
    switch (table_identifier) {
      case TableIdentifier::kProcedure:
        return proc_range_table_->AddKeyValuePair(key, value);
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddInfoToTableException(static_cast<int>(table_identifier), e.what());
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
    throw AddEntityToSetException(static_cast<int>(entity_identifier), e.what());
  }
}

bool Pkb::AddEntityToSet(const EntityIdentifier entity_identifier, const string& entity_val) {
  try {
    switch (entity_identifier) {
      case EntityIdentifier::kVariable: {
        UpdateIndexTable(index_var_table_, var_index_table_, entity_val);
        variable_set_.insert(GetIndexByString(IndexTableType::kVarIndex, entity_val));
        return true;
      }
      case EntityIdentifier::kProc: {
        UpdateIndexTable(index_proc_table_, proc_index_table_, entity_val);
        procedure_set_.insert(GetIndexByString(IndexTableType::kProcIndex, entity_val));
        return true;
      }
      default:
        throw InvalidIdentifierException();
    }
  } catch (exception& e) {
    throw AddEntityToSetException(static_cast<int>(entity_identifier), e.what());
  }
}

bool Pkb::AddCfg(shared_ptr<cfg::CFG> cfg) {
  try {
    this->cfg_list_.push_back(cfg);
    return true;
  } catch (exception& e) {
    throw e;
  }
}

typedef unordered_set<int>(Pkb::* GetEntityFn)();

const unordered_map<EntityIdentifier, GetEntityFn> entity_map_ = {
  {EntityIdentifier::kStmt, &Pkb::GetStmtSet},
  {EntityIdentifier::kAssign, &Pkb::GetAssignSet},
  {EntityIdentifier::kRead, &Pkb::GetReadSet},
  {EntityIdentifier::kPrint, &Pkb::GetPrintSet},
  {EntityIdentifier::kCall, &Pkb::GetCallSet},
  {EntityIdentifier::kIf, &Pkb::GetIfSet},
  {EntityIdentifier::kWhile, &Pkb::GetWhileSet},
  {EntityIdentifier::kVariable, &Pkb::GetVarSet},
  {EntityIdentifier::kProc, &Pkb::GetProcSet},
  {EntityIdentifier::kConstant, &Pkb::GetConstantSet},
};

unordered_set<int> Pkb::GetAllEntity(const EntityIdentifier entity_identifier) {
  try {
    const GetEntityFn get_entity_fn = entity_map_.at(entity_identifier);
    return (this->*get_entity_fn)();
  } catch (exception& e) {
    throw InvalidIdentifierException();
  }
}

bool Pkb::IsEntity(const EntityIdentifier entity_identifier, const int entity_idx) {
  const GetEntityFn get_entity_fn = entity_map_.at(entity_identifier);
  auto entity_set = (this->*get_entity_fn)();
  // O(1) find for unordered_set
  return entity_set.find(entity_idx) != entity_set.end();
}

typedef shared_ptr<EntityVarsTable>(Pkb::* GetEntityTableFn)();
const unordered_map<EntityIdentifier, GetEntityTableFn> entity_table_map = {
  {EntityIdentifier::kRead, &Pkb::GetReadTable},
  {EntityIdentifier::kPrint, &Pkb::GetPrintTable},
  {EntityIdentifier::kCall, &Pkb::GetCallerTable},
};

int Pkb::GetStringAttribute(const EntityIdentifier entity_identifier, const int stmt_no) {
  try {
    if (!IsEntity(EntityIdentifier::kRead, stmt_no) && !IsEntity(EntityIdentifier::kPrint, stmt_no) && !IsEntity(EntityIdentifier::kCall, stmt_no)) {
      return INVALID_INDEX;
    }
    const GetEntityTableFn table_getter = entity_table_map.at(entity_identifier);
    const shared_ptr<EntityVarsTable> table = (this->*table_getter)();
    auto index_table_type = IndexTableType::kVarIndex;
    if (entity_identifier == EntityIdentifier::kCall) {
      index_table_type = IndexTableType::kProcIndex;
    }
    return GetIndexByString(index_table_type, table->GetValueByKey(stmt_no));
  } catch (exception& e) {
    return INVALID_INDEX;
  }
}

vector<int> Pkb::GetStmtNumByStringAttribute(const EntityIdentifier entity_identifier, const int string_idx) {
  vector<int> res = {};
  const GetEntityTableFn table_getter = entity_table_map.at(entity_identifier);
  const shared_ptr<EntityVarsTable> table = (this->*table_getter)();
  auto index_table_type = IndexTableType::kVar;
  if (!IsEntity(EntityIdentifier::kVariable, string_idx) && !IsEntity(EntityIdentifier::kProc, string_idx)) {
    return res;
  }
  if (entity_identifier == EntityIdentifier::kCall) {
    index_table_type = IndexTableType::kProc;
  }
  for (const auto& [key, val] : table->GetKeyValueLst()) {
    if (val == GetStringByIndex(index_table_type, string_idx)) {
      res.push_back(key);
    }
  }
  return res;
}

string Pkb::GetStringByIndex(const IndexTableType index_table_type, const int idx) const {
  try {
    shared_ptr<IndexToEntityTable> table;
    if (index_table_type == IndexTableType::kProc) {
      table = index_proc_table_;
    } else if (index_table_type == IndexTableType::kVar) {
      table = index_var_table_;
    } else {
      throw InvalidIdentifierException();
    }
    return table->GetValueByKey(idx);
  } catch (exception& e) {
    return EMPTY_STRING;
  }
}

int Pkb::GetIndexByString(const IndexTableType index_table_type, const string& entity_name) const {
  try {
    shared_ptr<EntityToIndexTable> table;
    if (index_table_type == IndexTableType::kProcIndex) {
      table = proc_index_table_;
    } else if (index_table_type == IndexTableType::kVarIndex) {
      table = var_index_table_;
    } else {
      throw InvalidIdentifierException();
    }
    return table->GetValueByKey(entity_name);
  } catch (exception& e) {
    return INVALID_INDEX;
  }
}

vector<pair<int, string>> Pkb::GetAllIndexStringPairs(const IndexTableType index_table_type) const {
  if (index_table_type == IndexTableType::kProc) {
    return index_proc_table_->GetKeyValueLst();
  } else if (index_table_type == IndexTableType::kVar) {
    return index_var_table_->GetKeyValueLst();
  } else {
    throw InvalidIdentifierException();
  }
}
vector<pair<string, int>> Pkb::GetAllStringIndexPairs(const IndexTableType index_table_type) const {
  if (index_table_type == IndexTableType::kProcIndex) {
    return proc_index_table_->GetKeyValueLst();
  } else if (index_table_type == IndexTableType::kVarIndex) {
    return var_index_table_->GetKeyValueLst();
  } else {
    throw InvalidIdentifierException();
  }
}

vector<shared_ptr<cfg::CFG>> Pkb::GetCfgList() {
  return this->cfg_list_;
}
