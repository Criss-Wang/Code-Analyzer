#include "pkb.h"
#include "Utility/helper.h"

#include <queue>
#include <stack>

// TODO(Zhenlin): [Performance] Not optimized with the help of traversal order
template<typename T1, typename T2>
void PopulateForP(T1 table_to_refer, T2 table_to_update) {
  for (const auto key : table_to_refer->GetKeyLst()) {
    vector<int> current_key_child_lst;
    queue<int> q;
    q.push(key);
    while (!q.empty()) {
      if (!table_to_refer->KeyExistsInTable(q.front())) {
        q.pop();
        continue;
      }
      for (auto child_key : table_to_refer->GetValueByKey(q.front())) {
        current_key_child_lst.push_back(child_key);
        q.push(child_key);
      }
      q.pop();
    }
    if (table_to_update->KeyExistsInTable(key)) {
      table_to_update->UpdateKeyWithNewValue(key, current_key_child_lst);
    } else {
      table_to_update->AddKeyValuePair(key, current_key_child_lst);
    }
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

int Pkb::PopulateNestedRelationship() {
  try {
    PopulateNestedFollows();
    PopulateNestedParents();
    PopulateModifies();
    PopulateUses();
  } catch (exception& e) {
    return 0;
  }
  return 1;
}

void Pkb::PopulateNestedFollows() {
  PopulateForF<FollowsTable*, FollowsStarTable*>(follows_table_, follows_star_table_);
  PopulateForF<FollowsBeforeTable*, FollowsBeforeStarTable*>(follows_before_table_, follows_before_star_table_);
}

void Pkb::PopulateNestedParents() {
  PopulateForP<ParentTable*, ParentStarTable*>(parent_table_, parent_star_table_);
  PopulateForP<ChildTable*, ChildStarTable*>(child_table_, child_star_table_);
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

void Pkb::PopulateModifies() {
  PopulateNestedModifiesOrUses(*parent_star_table_, *child_star_table_,  *modifies_stmt_to_variables_table_, *modifies_variable_to_stmts_table_);
}

void Pkb::PopulateUses() {
  PopulateNestedModifiesOrUses(*parent_star_table_, *child_star_table_,  *uses_stmt_to_variables_table_, *uses_variable_to_stmts_table_);
}

// Following are for Search handlers

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
    if (!pattern_to_stmt_table_->KeyExistsInTable(s)) return empty_set;
    unordered_set<int> stmt_lst = pattern_to_stmt_table_->GetValueByKey(s);
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
  bool add_success = stmt_to_pattern_table_->AddKeyValuePair(line_num, valid_sub_patterns);
  for (auto p: valid_sub_patterns) {
    if (!pattern_to_stmt_table_->KeyExistsInTable(p)) {
      add_success = add_success && pattern_to_stmt_table_->AddKeyValuePair(p, unordered_set<int>{line_num});
    }
    else {
      unordered_set<int> current_set = pattern_to_stmt_table_->GetValueByKey(p);
      current_set.insert(line_num);
      add_success = add_success && pattern_to_stmt_table_->UpdateKeyWithNewValue(p, current_set);
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
